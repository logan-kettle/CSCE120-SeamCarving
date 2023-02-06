#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cmath>
#include "functions.h"

using namespace std;

// DONE: DO NOT TOUCH
int energy(const Pixel *const*image, int col, int row, int width, int height)
{
  // variable that energies are to be added to
  int energyVal;
  int rxGrad;
  int ryGrad;
  int gxGrad;
  int gyGrad;
  int bxGrad;
  int byGrad;

  // variables for bounds of gradient
  int x_co2 = col + 1;
  int x_co1 = col - 1;
  int y_co2 = row + 1;
  int y_co1 = row - 1;

  // checks if indices are out of bounds before ever called for array
  if (x_co2 > width - 1) {   // if the right bound is greater than final index of array
    x_co2 = 0;              // set the bound to the leftmost in row
  }
  if (x_co1 < 0) {    // if the left bound is negative
    x_co1 = width - 1;    // set bound to rightmost in row
  }
  if (y_co2 > height - 1) {    // if bottom bound exceeds bottom index
      y_co2 = 0;        // set bound to top index
  }
  if (y_co1 < 0) {      // if top bound is negative
      y_co1 = height - 1;   // set bound to bottom index
  }

  // calculates gradients of each color in x direction
  rxGrad = image[x_co2][row].r - image[x_co1][row].r;         // my mind thinks x2 - x1 which is why it might look weird
  gxGrad = image[x_co2][row].g - image[x_co1][row].g;
  bxGrad = image[x_co2][row].b - image[x_co1][row].b;

  // calculates gradients of each color in y direction
  ryGrad = image[col][y_co2].r - image[col][y_co1].r;
  gyGrad = image[col][y_co2].g - image[col][y_co1].g;
  byGrad = image[col][y_co2].b - image[col][y_co1].b;

  // calculates energy (sum of squared gradients in x + likewise in y)
  energyVal = ( (rxGrad * rxGrad) + (gxGrad * gxGrad) + (bxGrad * bxGrad) )
  + ( (ryGrad * ryGrad) + (gyGrad * gyGrad) + (byGrad * byGrad) );

  return energyVal;
}

// determines minimum value in 2 value array
int minEnergy2(int prev_col, int energy1, int energy2) {     
  if (prev_col == 0) {
    if (energy1 < energy2) {
      return prev_col;
    } else if (energy1 > energy2) {
      return prev_col + 1;
    } else {    // equal
      return prev_col;
    }
  } else {          // the only time minEnergy2 is called is with prev_col == 0 and prev_col == width - 1
      if (energy1 < energy2) {
      return prev_col - 1;
      } else if (energy1 > energy2) {
      return prev_col;
      } else {    // equal
      return prev_col;
      }
    }
}

int minEnergy2hor(int prev_col, int height, int energy1, int energy2) {     
  if (prev_col == height - 1) {
    if (energy1 < energy2) {
      return prev_col;
    } else if (energy1 > energy2) {
      return prev_col - 1;
    } else {    // equal
      return prev_col;
    }
  } else {          // the only time minEnergy2 is called is with prev_col == 0 and prev_col == width - 1
      if (energy1 < energy2) {
      return prev_col + 1;
      } else if (energy1 > energy2) {
      return prev_col;
      } else {    // equal
      return prev_col;
      }
    }
}

int minEnergy3hor(int prev_col, int energy1, int energy2, int energy3) {
    if (energy2 < energy1) {
      if (energy3 < energy2) {
        return prev_col - 1;
      } else if (energy2 < energy3) {
        return prev_col;
      } else {              // if the second two are the same, return the middle index
        return prev_col;   
      }
    } else if (energy1 < energy2) {
      if (energy1 < energy3) {
        return prev_col + 1;
      } else {
        return prev_col - 1;
      }
    } else {
      if (energy1 > energy3) {
        return prev_col - 1;
      } else {
        return prev_col;
      }
    }
}

// determines minimum value in 3 value array, energies listed from left to right
int minEnergy3(int prev_col, int energy1, int energy2, int energy3) {
    if (energy2 < energy1) {
      if (energy3 < energy2) {
        return prev_col + 1;
      } else if (energy2 < energy3) {
        return prev_col;
      } else {              // if the second two are the same, return the middle index
        return prev_col;   
      }
    } else if (energy1 < energy2) {
      if (energy1 < energy3) {
        return prev_col - 1;
      } else {
        return prev_col + 1;
      }
    } else {
      if (energy1 > energy3) {
        return prev_col + 1;
      } else {
        return prev_col;
      }
    }
}


// works for regular cases (not edges) now
int getVerticalSeam(const Pixel *const*image, int start_col, int width, int height, int* seam)
{
  // start with starting column value
  seam[0] = start_col;

  // determine whether lowest energy value is in same column, col - 1, or col + 1, load the column index into seam
  for (int i=1;i<height;i++) {  // starts at second index since first is start_col, goes until final row (height -1)
    if (width-1 == 0) {   // one column image
      seam[i] = 0;
    } else if (seam[i-1] == 0) {    // if edge column, it uses the 2 val function, otherwise 3
      seam[i] = minEnergy2(seam[i-1], energy(image, seam[i-1], i, width, height), energy(image, seam[i-1] + 1, i, width, height));
    } else if (seam[i-1] == width -1) {
      seam[i] = minEnergy2(seam[i-1], energy(image, seam[i-1] - 1, i, width, height), energy(image, seam[i-1], i, width, height));
    } else {
      seam[i] = minEnergy3(seam[i-1], energy(image,seam[i-1] - 1, i, width, height), energy(image,seam[i-1], i, width, height), energy(image,seam[i-1] + 1, i, width, height));
    }
  }

  int sumEnergies = 0;
  for (int i=0;i<height;i++) {
    sumEnergies += energy(image, seam[i], i, width, height);
  }

  return sumEnergies;
}

// TODO Write this function
void removeVerticalSeam(Pixel **image, int& width, int height, int *verticalSeam)
{
  for (int row=0;row<height;row++) {
    for (int col=verticalSeam[row];col<width-1;col++) {   // gives column index of pixel that needs to be removed at particular row
      image[col][row] = image[col+1][row];      // given pixel is replaced by pixel to the right of it
    }
  }
  width--; // adjusts width accordingly
}

// TODO Write this function for extra credit
int getHorizontalSeam(const Pixel *const*image, int start_row, int width, int height, int* seam)
{
  // start with starting column value
  seam[0] = start_row;

  // determine whether lowest energy value is in same column, col - 1, or col + 1, load the column index into seam
  for (int i=1;i<width;i++) {  // starts at second index since first is start_col, goes until final row (height -1)
    if (seam[i-1] == 0) {    // if edge column, it uses the 2 val function, otherwise 3
      seam[i] = minEnergy2hor(seam[i-1], height, energy(image, i, seam[i-1] + 1, width, height), energy(image, i, seam[i-1], width, height));
    } else if (seam[i-1] == height - 1) {
      seam[i] = minEnergy2hor(seam[i-1], height, energy(image, i, seam[i-1], width, height), energy(image, i, seam[i-1] - 1, width, height));
    } else {
      seam[i] = minEnergy3hor(seam[i-1], energy(image,i, seam[i-1] + 1, width, height), energy(image,i, seam[i-1], width, height), energy(image,i, seam[i-1] - 1, width, height));
    }
  }

  int sumEnergies = 0;
  for (int i=0;i<width;i++) {
    sumEnergies += energy(image, i, seam[i], width, height);
  }

  return sumEnergies;
}

// TODO Write this function for extra credit
void removeHorizontalSeam(Pixel **image, int width, int& height, int *horizontalSeam)
{
    for (int col=0;col<width;col++) {
      for (int row=horizontalSeam[col];row<height-1;row++) {   // gives column index of pixel that needs to be removed at particular row
        image[col][row] = image[col][row+1];      // given pixel is replaced by pixel to the right of it
    }
  }
  height--; // adjusts height accordingly
}

int *findMinVerticalSeam(const Pixel *const*image, int width, int height)
{
  // initialize minSeam and minDistance to seam starting at first col (index 0)
  int *minSeam = new int[height]{0};
  int minDist = getVerticalSeam(image, 0, width, height, minSeam);

  int *candidateSeam = new int[height]{0};
  int candidateDistance = -1; // invalid distance

  // start at second col (index 1) since we initialized with first col (index 0)
  for (int col = 1; col < width; ++col)
  {
    candidateDistance = getVerticalSeam(image, col, width, height, candidateSeam);

    if (candidateDistance < minDist)
    { // new min
      //  swap min & candidate
      minDist = candidateDistance;
      int* temp = candidateSeam;
      candidateSeam = minSeam;
      minSeam = temp;
    }
  }

  // clean up 
  delete [] candidateSeam;

  return minSeam;
}

int *findMinHorizontalSeam(const Pixel *const*image, int width, int height)
{
  // initialize minSeam and minDistance to seam starting at first row (index 0)
  int *minSeam = new int[width]{0};
  int minDistance = getHorizontalSeam(image, 0, width, height, minSeam);

  int *candidateSeam = new int[width]{0};
  int candidateDistance = -1; // invalid distance

  // start at second row (index 1) since we initialized with first row (index 0)
  for (int row = 1; row < height; ++row)
  {
    candidateDistance = getHorizontalSeam(image, row, width, height, candidateSeam);

    if (candidateDistance < minDistance)
    { // new minimum
      //  swap min and candidate seams
      minDistance = candidateDistance;
      int* temp = candidateSeam;
      candidateSeam = minSeam;
      minSeam = temp;
    }
  }

    // clean up 
  delete [] candidateSeam;

  return minSeam;
}

Pixel **createImage(int width, int height)
{
  cout << "Start createImage... " << endl;

  // Create a one dimensional array on the heap of pointers to Pixels
  //    that has width elements (i.e. the number of columns)
  Pixel **image = new Pixel *[width] {}; // initializes to nullptr

  for (int col = 0; col < width; ++col)
  { // loop through each column
    // assign that column to a one dimensional array on the heap of Pixels
    //  that has height elements (i.e. the number of rows)
    try
    {
      image[col] = new Pixel[height];
    }
    catch (std::bad_alloc &e)
    {
      // clean up already allocated arrays
      for (int i = 0; i < col; ++i)
      {
        delete[] image[i];
      }
      delete[] image;
      // rethrow
      throw e;
    }
  }

  // initialize cells
  // cout << "Initializing cells..." << endl;
  for (int row = 0; row < height; ++row)
  {
    for (int col = 0; col < width; ++col)
    {
      // cout << "(" << col << ", " << row << ")" << endl;
      image[col][row] = {0, 0, 0};
    }
  }
  cout << "End createImage... " << endl;
  return image;
}

void deleteImage(Pixel **image, int width)
{
  cout << "Start deleteImage..." << endl;
  // avoid memory leak by deleting the array
  for (int i = 0; i < width; ++i)
  {
    delete[] image[i];
  }
  delete[] image;
  image = nullptr;
  cout << "End deleteImage..." << endl;
}

bool isValidColor(int colorVal)
{
  if (colorVal < 0 || colorVal > 255)
  {
    return false;
  }
  return true;
}

Pixel ** loadImage(string filename, int &width, int &height)
{
  cout << "Start loadImage..." << endl;
  // remove
  ifstream ifs(filename);
  if (!ifs.is_open())
  {
    throw std::invalid_argument("Failed to open input file (" + filename + ")");
  }

  string type;
  ifs >> type; // should be P3
  if (toupper(type.at(0)) != 'P' || type.at(1) != '3')
  {
    throw std::domain_error("Not PPM type P3 (" + type + ")");
  }
  ifs >> width;
  cout << "w and h: " << width << " " << height << endl;
  if (ifs.fail())
  {
    throw std::domain_error("Read non-integer value for width");
  }
  if (width <= 0)
  {
    ostringstream oss;
    oss << "Width in file must be greater than 0 (" << width << ")";
    throw std::domain_error(oss.str());
  }

  ifs >> height;
  if (ifs.fail())
  {
    cout << "Read non-integer value for height" << endl;
  }
  if (height <= 0)
  {
    ostringstream oss;
    oss << "Height in file must be greater than 0 (" << height << ")";
    throw std::domain_error(oss.str());
  }

  int colorMax = 0;
  ifs >> colorMax;
  if (ifs.fail())
  {
    throw std::domain_error("Read non-integer value for max color value");
  }
  if (colorMax != 255)
  {
    ostringstream oss;
    oss << "Max color value must be 255 (" << colorMax << ")";
    throw std::domain_error(oss.str());
  }

  // load image throws exceptions but we will let them pass through
  Pixel **image = createImage(width, height);

  for (int row = 0; row < height; ++row)
  {
    for (int col = 0; col < width; ++col)
    {
      // cout << "Pixel(" << col << ", " << row << ")" << endl;
      ifs >> image[col][row].r;
      if (ifs.fail() && !ifs.eof())
      { // bad input that is not caused by being at the end of the file
        throw std::domain_error("Read non-integer value for red");
      }
      if (!isValidColor(image[col][row].r))
      {
        ostringstream oss;
        oss << "Invalid color value for red (" << image[col][row].r << ")";
        throw std::domain_error(oss.str());
      }
      if (ifs.eof()) {
        throw std::domain_error("Expected value for red, but at end of file");
      }

      ifs >> image[col][row].g;
      if (ifs.fail() && !ifs.eof())
      { // bad input that is not caused by being at the end of the file
        throw std::domain_error("Read non-integer value for green");
      }
      if (!isValidColor(image[col][row].r))
      {
        ostringstream oss;
        oss << "Invalid color value for green (" << image[col][row].r << ")"; // changed + to <<
        throw std::domain_error(oss.str());
      }
      if (ifs.eof()) {
        throw std::domain_error("Expected value for green, but at end of file");
      }

      ifs >> image[col][row].b;
      if (ifs.fail() && !ifs.eof())
      { // bad input that is not caused by being at the end of the file
        throw std::domain_error("Read non-integer value for blue");
      }
      if (!isValidColor(image[col][row].r))
      {
        ostringstream oss;
        oss << "Invalid color value for blue (" << image[col][row].r << ")";     // changed + to <<
        throw std::domain_error(oss.str());
      }
      if (ifs.eof()) {
        throw std::domain_error("Expected value for blue, but at end of file");
      }
    }
  }
  cout << "End loadImage..." << endl;
  return image;
}

void outputImage(string filename, const Pixel *const *image, int width, int height)
{
  cout << "Start outputImage..." << endl;
  // remove code
  // declare/define and open output file stream with filename
  ofstream ofs(filename);
  // ensure file is open
  if (!ofs.is_open())
  {
    throw std::invalid_argument("Error: failed to open output file - " + filename);
  }
  ofs << "P3" << endl;
  ofs << width << " " << height << endl;
  ofs << 255 << endl;
  for (int row = 0; row < height; ++row)
  {
    for (int col = 0; col < width; ++col)
    {
      ofs << image[col][row].r << " ";
      ofs << image[col][row].g << " ";
      ofs << image[col][row].b << " ";
    }
    ofs << endl;
  }
  cout << "End outputImage..." << endl;
}




/*
QUESTIONS
- necessary to "edit launch.json and add test vaues for command line arguments"?
- expand on relationship between pointer and array (bc can't pointer be single value?)
- how is int* seam an array when you sometimes have to call arr[]?
- what is allocating practically? could you reexplain what it means to create new and then delete, and what exactly memory leaking is
- "if you allocate something, like loading a map, you must also deallocate it"
*/

// pointer like po box 57 or po boxes starting at 57

// not new variables live on the stack go away after running, must know size of array at compile time
// new is mjust used for arrays and classes 
// memory leaking is when you use new but not delete