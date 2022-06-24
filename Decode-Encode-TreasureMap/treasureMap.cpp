#include "treasureMap.h"
#include "queue.h"
using namespace std;

treasureMap::treasureMap(const PNG & baseim, const PNG & mazeim, pair<int,int> s)
{

    start = s;
    base = baseim;
    maze = mazeim;

}

void treasureMap::setGrey(PNG & im, pair<int,int> loc) {

    RGBAPixel* pixel = im.getPixel(loc.first,loc.second);
    pixel->r = 2*((pixel->r)/4);
    pixel->g = 2*((pixel->g)/4);
    pixel->b = 2*((pixel->b)/4);

}

void treasureMap::setLOB(PNG & im, pair<int,int> loc, int d){

    unsigned char mazeValue = d%64;
    unsigned char rValue = mazeValue>>4;
    unsigned char gValue = (mazeValue>>2)&0b11;
    unsigned char bValue = mazeValue&0b11;
    RGBAPixel* pixel = im.getPixel(loc.first,loc.second);
    pixel->r = ((pixel->r)&0b11111100)|rValue;
    pixel->g = ((pixel->g)&0b11111100)|gValue;
    pixel->b = ((pixel->b)&0b11111100)|bValue;
}

PNG treasureMap::renderMap(){
// Make a copy of the base image
PNG map = base;
// Initialize working vectors
vector<vector<bool>> boolTable(base.width(), vector<bool>(base.height(), false));
vector<vector<int>> distTable(base.width(),vector<int>(base.height(),-1));
// Initialize a queue to orchestrate the traversal
Queue<pair<int,int>> queue;
// Doing from the start location
boolTable[start.first][start.second] = true;
distTable[start.first][start.second] = 0;
setLOB(map,start,0);
queue.enqueue(start);
// Breath-first traversal (level-order) if the maze image
while (!queue.isEmpty())
{
    pair<int,int> curr = queue.dequeue();
    vector<pair<int,int>> locs = neighbors(curr);
    for (int i = 0; i<(int)locs.size(); i++) {
        if (good(boolTable,curr,locs[i])) {
            boolTable[locs[i].first][locs[i].second] = true;
            distTable[locs[i].first][locs[i].second] = distTable[curr.first][curr.second]+1;
            setLOB(map,locs[i],distTable[locs[i].first][locs[i].second]);
            queue.enqueue(locs[i]);
        }
    }
}
// Finally, return the image
return map;


}


PNG treasureMap::renderMaze(){

PNG mazeOnBase = base;
int image_width = base.width();
int image_height = base.height();

// Render the maze on the base image
// Initialize working vectors
vector<vector<bool>> boolTable(base.width(), vector<bool>(base.height(), false));
// Initialize a queue to orchestrate the traversal
Queue<pair<int,int>> queue;
// Doing from the start location
boolTable[start.first][start.second] = true;
setGrey(mazeOnBase,start);
queue.enqueue(start);
// Breath-first traversal (level-order) if the maze image
while (!queue.isEmpty())
{
    pair<int,int> curr = queue.dequeue();
    vector<pair<int,int>> locs = neighbors(curr);
    for (int i = 0; i<(int)locs.size(); i++) {
        if (good(boolTable,curr,locs[i])) {
            boolTable[locs[i].first][locs[i].second] = true;
            setGrey(mazeOnBase,locs[i]);
            queue.enqueue(locs[i]);
        }
    }
}

// Draws a red 7px x 7px square at the start location
for (int i = -3; i < 4; i++) {
    if (i+start.first<image_width && i+start.first>=0) {
        for (int j = -3; j < 4; j++) {
            if (j+start.second<image_height && j+start.second>=0) {
               RGBAPixel* pixel = mazeOnBase.getPixel(start.first+i,start.second+j);
                pixel->r = 255;
                pixel->b = 0;
                pixel->g = 0; 
            }
        }
    }
}


// Finally, return the image
return mazeOnBase;

}

bool treasureMap::good(vector<vector<bool>> & v, pair<int,int> curr, pair<int,int> next){

    bool withinImg1 = next.first<(int)base.width()&&next.second<(int)base.height();
    bool withinImg2 = next.first>=0&&next.second>=0;
    if (!withinImg1||!withinImg2)
        return false;
    if (v[next.first][next.second])
        return false;
    bool R = maze.getPixel(curr.first,curr.second)->r==maze.getPixel(next.first,next.second)->r;
    bool G = maze.getPixel(curr.first,curr.second)->g==maze.getPixel(next.first,next.second)->g;
    bool B = maze.getPixel(curr.first,curr.second)->b==maze.getPixel(next.first,next.second)->b;
        return R&&G&&B;

}

vector<pair<int,int>> treasureMap::neighbors(pair<int,int> curr) {

    vector<pair<int,int>> neighbors;
    neighbors.push_back(make_pair(curr.first-1,curr.second));
    neighbors.push_back(make_pair(curr.first,curr.second+1)); 
    neighbors.push_back(make_pair(curr.first+1,curr.second)); 
    neighbors.push_back(make_pair(curr.first,curr.second-1));
    return neighbors;  

}

