#include "block.h"
#include <cmath>
#include <iostream>

int Block::height() const {
  return data[0].size();
}

int Block::width() const {
  return data.size();
}

void Block::render(PNG &im, int x) const {
  int xcoord = x;
  int ycoord = 0;
  int image_width = im.width();
  int image_height = im.height();
  // Make sure the coordinates of the pixel we are interested is in the scope of image dimension
  for (int i = 0; i < width() && i+xcoord<image_width; i++) {
    for (int j = 0; j < height() && j+ycoord<image_height; j++) {
      HSLAPixel* renderPixel = im.getPixel(i+xcoord, j+ycoord);
      *renderPixel = data[i][j];
    }
    
  }
}

void Block::build(PNG &im, int x, int width) {
  int height = im.height();

  for (int i = 0; i < width; i++) {
    vector<HSLAPixel> columnPix;
    for (int j = 0; j < height; j++) {
      HSLAPixel* buildPixel = im.getPixel(i+x, j);
      columnPix.push_back(*buildPixel);
    }
    data.push_back(columnPix);
  }
  // Implementation with resize will also work!
  // data.resize(width, vector<HSLAPixel>(height));
  // for (int i = 0; i < width; i++) {
  //   for (int j = 0; j < height; j++) {
  //     HSLAPixel* buildPixel = im.getPixel(i+x, j);
  //     data[i][j] = *buildPixel;
  //   }
  // }
}
