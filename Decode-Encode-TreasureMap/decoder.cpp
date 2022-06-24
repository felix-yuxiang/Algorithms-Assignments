#include "decoder.h"
#include "queue.h"
#include "stack.h"
using namespace std;

decoder::decoder(const PNG & tm, pair<int,int> s)
   :start(s),mapImg(tm) {

// Initialize working vectors
vector<vector<bool>> boolTable(mapImg.width(), vector<bool>(mapImg.height(), false));
vector<vector<int>> distTable(mapImg.width(),vector<int>(mapImg.height(),-1));
vector<vector<pair<int,int>>> solutionPathCandidates(mapImg.width(),vector<pair<int,int>>(mapImg.height(),make_pair(-1,-1)));
// Initialize a queue to orchestrate the traversal
Queue<pair<int,int>> queue;
int max = -1;
pair<int,int> end; 
// Doing from the start location
boolTable[start.first][start.second] = true;
distTable[start.first][start.second] = 0;
queue.enqueue(start);
// Breath-first traversal (level-order)
// int count=0; 
while (!queue.isEmpty())
{
    pair<int,int> curr = queue.dequeue();
    vector<pair<int,int>> locs = neighbors(curr);
    
    // if (count<50){
    //     cout<<"("<<curr.first <<","<<curr.second<<")"<<endl;
    //     count++;
    // }
    
    for (int i = 0; i<(int)locs.size(); i++) {
        if (good(boolTable,distTable,curr,locs[i])) {
            boolTable[locs[i].first][locs[i].second] = true;
            distTable[locs[i].first][locs[i].second] = distTable[curr.first][curr.second]+1;
            if (distTable[locs[i].first][locs[i].second]>=max) {
                max = distTable[locs[i].first][locs[i].second];
                end.first = locs[i].first;
                end.second = locs[i].second;
            }
            solutionPathCandidates[locs[i].first][locs[i].second].first = curr.first;
            solutionPathCandidates[locs[i].first][locs[i].second].second = curr.second;
            queue.enqueue(locs[i]);
        }
    }
}
    dist = distTable[end.first][end.second];

// find the treasure
// int max = -1;
// pair<int,int> end; 
// for(int i=0; i<(int)boolTable.size();i++) {
//    for (int j=0; j<(int)boolTable[i].size();j++) {
//       int dist = abs(i-start.first)+abs(j-start.second);
//       if (boolTable[i][j]) {
//          if (dist>max)
//          max = dist;
//          end.first = i;
//          end.second = j;
//       }
//    }
// }
// find the optimal path
Stack<pair<int,int>> paths;
while(end.first!=-1) {
   paths.push(end);
   end = solutionPathCandidates[end.first][end.second];
}
while(!paths.isEmpty()) {
      pathPts.push_back(paths.pop());
}
    
}

PNG decoder::renderSolution(){

   PNG solution = mapImg;
   for(int i=0; i<(int)pathPts.size();i++) {
      RGBAPixel* pixel = solution.getPixel(pathPts[i].first,pathPts[i].second);
      pixel->r = 255;
      pixel->g = 0;
      pixel->b = 0;
   }
return solution;

}

PNG decoder::renderMaze(){

PNG maze = mapImg;
int image_width = maze.width();
int image_height = maze.height();

// Initialize working vectors
vector<vector<bool>> boolTable(maze.width(), vector<bool>(maze.height(), false));
vector<vector<int>> distTable(maze.width(),vector<int>(maze.height(),-1));
// Initialize a queue to orchestrate the traversal
Queue<pair<int,int>> queue;
// Doing from the start location
boolTable[start.first][start.second] = true;
distTable[start.first][start.second] = 0;
setGrey(maze,start);
queue.enqueue(start);
// Breath-first traversal (level-order) if the maze image
while (!queue.isEmpty())
{
    pair<int,int> curr = queue.dequeue();
    vector<pair<int,int>> locs = neighbors(curr);
    for (int i = 0; i<(int)locs.size(); i++) {
        if (good(boolTable,distTable,curr,locs[i])) {
            boolTable[locs[i].first][locs[i].second] = true;
            distTable[locs[i].first][locs[i].second] = distTable[curr.first][curr.second]+1;
            setGrey(maze,locs[i]);
            queue.enqueue(locs[i]);
        }
    }
}


// Draws a red 7px x 7px square at the start location
for (int i = -3; i < 4; i++) {
    if (i+start.first<image_width && i+start.first>=0) {
        for (int j = -3; j < 4; j++) {
            if (j+start.second<image_height && j+start.second>=0) {
               RGBAPixel* pixel = maze.getPixel(start.first+i,start.second+j);
                pixel->r = 255;
                pixel->b = 0;
                pixel->g = 0; 
            }
        }
    }
}


// Finally, return the image
return maze;


}

void decoder::setGrey(PNG & im, pair<int,int> loc){

   RGBAPixel* pixel = im.getPixel(loc.first,loc.second);
    pixel->r = 2*((pixel->r)/4);
    pixel->g = 2*((pixel->g)/4);
    pixel->b = 2*((pixel->b)/4);

}

pair<int,int> decoder::findSpot(){

   return pathPts.back();
}

int decoder::pathLength(){

   return dist+1;

}

bool decoder::good(vector<vector<bool>> & v, vector<vector<int>> & d, pair<int,int> curr, pair<int,int> next){
// 1.Within the image
    bool withinImg1 = next.first<(int)mapImg.width()&&next.second<(int)mapImg.height();
    bool withinImg2 = next.first>=0&&next.second>=0;
    if (!withinImg1||!withinImg2)
        return false;
// 2.Unvisited 
    if (v[next.first][next.second])
        return false;
// 3. Coloured so that lower order bits fit the pattern prescribed by the encoder
   RGBAPixel* pixel = mapImg.getPixel(next.first,next.second);
    return compare(*pixel, d[curr.first][curr.second]);
    

}

vector<pair<int,int>> decoder::neighbors(pair<int,int> curr) {

   vector<pair<int,int>> neighbors;
    neighbors.push_back(make_pair(curr.first-1,curr.second));
    neighbors.push_back(make_pair(curr.first,curr.second+1)); 
    neighbors.push_back(make_pair(curr.first+1,curr.second)); 
    neighbors.push_back(make_pair(curr.first,curr.second-1));
    return neighbors; 

}


bool decoder::compare(RGBAPixel p, int d){

   // v = b1b2b3b4b5b6
   unsigned int rValue = (p.r&0b11)<<4;
   unsigned int gValue = (p.g&0b11)<<2;
   unsigned int bValue = p.b&0b11;
   int valueP = rValue|gValue|bValue;
   return valueP==((d+1)%64);


}
