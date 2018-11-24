#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <vector>
#include <time.h>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>

#define PI 3.1415926

/**
 * Mode 0: first straight line
 * Mode 1: first big turn
 * Mode 2: second straight line
 * Mode 3: second big turn
 * Mode 4: THIRD straight line
 * Mode 5: final
 * Mode 6: exit
 * */
#define FIRST_STRAIGHT_MODE 0
#define FIRST_TURN_MODE 1
#define SECOND_STRAIGHT_MODE 2
#define SECOND_TURN_MODE 3
#define THIRD_STRAIGHT_MODE 4
#define FINAL_MODE 5
#define EXIT_MODE 6

// #define _IMAGE_DEBUG

// #define _DEBUG

// #define _VIDEO_DEBUG

// #define _TIME

#define _ACTION

#ifdef _ACTION
#include "GPIOlib.h"
using namespace GPIO;
#endif

using namespace cv;
using namespace std;

#ifdef _VIDEO_DEBUG
const string CAM_PATH = "/home/hirico/record.mp4";
#endif

#ifndef _VIDEO_DEBUG
const string CAM_PATH = "/dev/video1";
#endif

const string IMAGE_FILE_NAME = "firstLeft.png";

/**
 * Mode 0 value
 * */
int CANNY_LOWER_BOUND_0 = 10;
int CANNY_UPPER_BOUND_0 = 50;
int HOUGH_0 = 20;
int dilate_width_0 = 8;
int dilate_height_0 = 5;
int HOUGH_BOTTOM_0 = 15;
int BACK_SPEED_0 = 11;
long BACK_DELAY_0 = 1000;
long normalSpeedTime_0 = 100;
long highSpeedTime_0 = 100;
int highSpeed_0 = 20;

/**
 * Mode 1 value
 * */
int turnTime_1 = 0; // 0: small 1: middle 2: large
long smallestTurnTime_1 = 800;
long middleTurnTime_1 = 1000;
long longTurnTime_1 = 1200;
int bigTurnDegree_1 = 15;

/**
 * Mode 2 value
 * */
int CANNY_LOWER_BOUND_2 = 10;
int CANNY_UPPER_BOUND_2 = 50;
int HOUGH_2 = 20;
int dilate_width_2 = 8;
int dilate_height_2 = 5;
int HOUGH_BOTTOM_2 = 15;
int BACK_SPEED_2 = 11;
long BACK_DELAY_2 = 1000;
long normalSpeedTime_2 = 100;
long highSpeedTime_2 = 100;
int highSpeed_2 = 20;

/**
 * Mode 3 value
 * */
int turnTime_3 = 0; // 0: small 1: middle 2: large
long smallestTurnTime_3 = 800;
long middleTurnTime_3 = 1000;
long longTurnTime_3 = 1200;
int bigTurnDegree_3 = 15;

/**
 * Mode 4 value
 * */
int CANNY_LOWER_BOUND_4 = 10;
int CANNY_UPPER_BOUND_4 = 50;
int HOUGH_4 = 20;
int dilate_width_4 = 8;
int dilate_height_4 = 5;
int HOUGH_BOTTOM_4 = 15;
int BACK_SPEED_4 = 11;
long BACK_DELAY_4 = 1000;
long normalSpeedTime_4 = 100;
long highSpeedTime_4 = 100;
int highSpeed_4 = 20;

/**
 * Mode 5 value
 * */
long finalRushTime = 200;

/** Also controlled by controller file */
int thresholdBorder = 100;
int bigTurn_coolDown = 100;
float offsetDegree = 0.1;
int smallLeftAngle = 20;
int leftAngle = 26;
int largeLeftAngle = 31;
int smallRightAngle = 8;
int rightAngle = 10;
int largeRightAngle = 15;
float smallOffest = 0.04;
float largeOffest = 0.7;
long turnDelay = 200;

/** old value and manual file controlled value */
int CANNY_LOWER_BOUND = 10;
int CANNY_UPPER_BOUND = 50;
int HOUGH_THRESHOLD = 20;

int BOTTOM_CANNY_LOWER_BOUND = 10;
int BOTTOM_CANNY_UPPER_BOUND = 50;
int BOTTOM_HOUGH_THRESHOLD = 15;

int leftStraightSpeed = 10;
int rightStraightSpeed = 10;
int leftDegree = -25;
int rightDegree = 20;

int firstLeftSpeed = 25;
int firstRightSpeed = 25;

int leftBigTurnSpeed = 20;
int rightBigTurnSpeed = 20;
int bigTurnDegree = -13;
long smallestTurnTime = 800;

const int delayLoop = 40;
int nowDelayLoop = 0;

float finalCount = 0;

int stopped = 0;

int current_mode = FIRST_STRAIGHT_MODE;

int start_mode = FIRST_STRAIGHT_MODE;

int turnedNum = 0;

int neededTurnedNum = 2;

int current_coolDown = 0;

void readPara()
{
  ifstream file("../manual/parameter.txt");
  string line;
  if (file.is_open())
  {
    getline(file, line);
    stringstream stream(line);
    stream >> leftStraightSpeed >> rightStraightSpeed >> leftDegree >> rightDegree >> leftBigTurnSpeed >> rightBigTurnSpeed >> bigTurnDegree >> smallestTurnTime;
    clog << "read: " << leftStraightSpeed << " " << rightStraightSpeed << " "
         << leftDegree << " " << rightDegree << " "
         << leftBigTurnSpeed << " " << rightBigTurnSpeed << " "
         << bigTurnDegree << " " << smallestTurnTime << " " << endl;
  }
  file.close();

  ifstream controllerFile("../controller/parameter.txt");
  string line0;
  string line1;
  string line2;
  string line3;
  string line4;
  string line5;
  string line6;
  string line7;
  string line8;
  if (controllerFile.is_open()) {
    getline(controllerFile, line0);
    stringstream stream0(line0);
    stream0 >> CANNY_LOWER_BOUND_0 >> CANNY_UPPER_BOUND_0 >> HOUGH_0 >> dilate_width_0
            >> dilate_height_0 >> HOUGH_BOTTOM_0 >> BACK_SPEED_0 >> BACK_DELAY_0
            >> normalSpeedTime_0 >> highSpeedTime_0 >> highSpeed_0;
    getline(controllerFile, line1);
    stringstream stream1(line1);
    stream1 >> smallestTurnTime_1 >> middleTurnTime_1 >> longTurnTime_1 >> bigTurnDegree_1;
    getline(controllerFile, line2);
    stringstream stream2(line2);
    stream2 >> CANNY_LOWER_BOUND_2 >> CANNY_UPPER_BOUND_2 >> HOUGH_2 >> dilate_width_2
            >> dilate_height_2 >> HOUGH_BOTTOM_2 >> BACK_SPEED_2 >> BACK_DELAY_2
            >> normalSpeedTime_2 >> highSpeedTime_2 >> highSpeed_2;
    getline(controllerFile, line3);
    stringstream stream3(line3);
    stream3 >> smallestTurnTime_3 >> middleTurnTime_3 >> longTurnTime_3 >> bigTurnDegree_3;
    getline(controllerFile, line4);
    stringstream stream4(line4);
    stream4 >> CANNY_LOWER_BOUND_4 >> CANNY_UPPER_BOUND_4 >> HOUGH_4 >> dilate_width_4
            >> dilate_height_4 >> HOUGH_BOTTOM_4 >> BACK_SPEED_4 >> BACK_DELAY_4
            >> normalSpeedTime_4 >> highSpeedTime_4 >> highSpeed_4;
    getline(controllerFile, line5);
    stringstream stream5(line5);
    stream5 >> finalRushTime;
    getline(controllerFile, line6);
    stringstream stream6(line6);
    stream6 >> thresholdBorder >> bigTurn_coolDown >> offsetDegree;
    getline(controllerFile, line7);
    stringstream stream7(line7);
    stream7 >> smallLeftAngle >> leftAngle >> largeLeftAngle >> smallRightAngle >> rightAngle >> largeRightAngle >> smallOffest >> largeOffest;
    getline(controllerFile, line8);
    stringstream stream8(line8);
    stream8 >> turnDelay;
    clog << "controller file read complete" << endl;
    clog << CANNY_LOWER_BOUND_0 << " " << CANNY_UPPER_BOUND_0 << " " << HOUGH_0 << " " << dilate_width_0
            << " " << dilate_height_0 << " " << HOUGH_BOTTOM_0 << " " << BACK_SPEED_0 << " " << BACK_DELAY_0 << " " << endl;
    clog << smallestTurnTime_1 << " " << bigTurnDegree_1 << " " << endl;
    clog << CANNY_LOWER_BOUND_2 << " " << CANNY_UPPER_BOUND_2 << " " << HOUGH_2 << " " << dilate_width_2
            << " " << dilate_height_2 << " " << HOUGH_BOTTOM_2 << " " << BACK_SPEED_2 << " " << BACK_DELAY_2 << " " << endl;
    clog << smallestTurnTime_3 << " " << bigTurnDegree_3 << " " << endl;
    clog << CANNY_LOWER_BOUND_4 << " " << CANNY_UPPER_BOUND_4 << " " << HOUGH_4 << " " << dilate_width_4
            << " " << dilate_height_4 << " " << HOUGH_BOTTOM_4 << " " << BACK_SPEED_4 << " " << BACK_DELAY_4 << " " << endl;
    clog << finalRushTime << " " << endl;
    clog << thresholdBorder << " " << bigTurn_coolDown << " " << offsetDegree << " " << endl;
    clog << smallLeftAngle << " " << leftAngle << " " << largeLeftAngle << " " << smallRightAngle << " " << rightAngle << " " << largeRightAngle << " " << smallOffest << " " << largeOffest << endl;
    clog << turnDelay << endl;
  }
  controllerFile.close();
}

/**
 * Mode 0: first staright line
 * */
bool first_isTurnLeft = false;
int first_lineFrameCount = 0;
void firstStraightProcessing(const Mat &image) {
  /* Determine the current turn direcion */
  Mat contours;
  Canny(image, contours, CANNY_LOWER_BOUND_0, CANNY_UPPER_BOUND_0);
  float rightMaxRad = PI / 2;
  float rightMaxRho = 0;
  float minRho = 0;
  float minRad = PI / 2;
  vector<Vec2f> lines;
  HoughLines(contours, lines, 1, PI / 180, HOUGH_0);
  for (vector<Vec2f>::const_iterator it = lines.begin(); it != lines.end(); ++it)
  {
    float rho = (*it)[0];   //First element is distance rho
    float theta = (*it)[1]; //Second elem0ent is angle theta
    float angle = theta * 180 / PI;
    if ((angle > 5 && angle < 80) || (angle > 100 && angle < 175))
    {
      if (theta > PI / 2 && theta > rightMaxRad)
      {
        rightMaxRad = theta;
        rightMaxRho = rho;
      }
      if (theta < minRad && theta < PI / 2)
      {
        minRad = theta;
        minRho = rho;
      }
    }
  }

  float right = PI - rightMaxRad;
  if (minRad > right + offsetDegree)
  {
    first_isTurnLeft = true;
  }
  else if (minRad < right - offsetDegree)
  {
    first_isTurnLeft = false;
  }
  else
  {
    first_isTurnLeft = !first_isTurnLeft;
  }
  #ifdef _ACTION
  clog << "turn " << (first_isTurnLeft ? "left" : "right") << endl;
  if (normalSpeedTime_0 > 0) {
    controlLeft(FORWARD, leftStraightSpeed);
    controlRight(FORWARD, rightStraightSpeed);
    normalSpeedTime_0 --;
  } else {
    if (highSpeedTime_0 > 0) {
      controlLeft(FORWARD, highSpeed_0);
      controlRight(FORWARD, highSpeed_0);
      highSpeedTime_0 --;
    } else {
      controlLeft(FORWARD, leftStraightSpeed);
      controlRight(FORWARD, rightStraightSpeed);
    }
  }
  if (first_isTurnLeft)
  {
    if (minRad > right + largeOffest) {
      turnTo(-largeLeftAngle);
    } else if (minRad > right + smallOffest) {
      turnTo(-leftAngle);
    } else{
      turnTo(-smallLeftAngle);
    }
  }
  else
  {
    if (minRad < right - largeOffest) {
      turnTo(largeRightAngle);
    } else if (minRad < right - smallOffest) {
      turnTo(rightAngle);
    } else{
      turnTo(smallRightAngle);
    }
  }
  #endif

  /* Determine the next state */
  // prepare bottom image (middle 50% of the main image), to detect some sure line, e.g. final
  Rect bottomRoi(image.cols / 4, 0, image.cols / 2, image.rows / 2 * 1);
  Mat bottomImage = image(bottomRoi);
  Mat dilate_element = getStructuringElement(MORPH_RECT, Size(dilate_width_0, dilate_height_0));
  Mat bottomDilate;
  dilate(bottomImage, bottomDilate, dilate_element);
  Mat bottomContours;
  Canny(bottomDilate, bottomContours, CANNY_LOWER_BOUND_0, CANNY_LOWER_BOUND_0);
  int bottomLineDetected = 0;
  float bottomRho = 0;
  float bottomRad = 0;
  vector<Vec2f> bottomLines;
  HoughLines(bottomContours, bottomLines, 1, PI / 180, HOUGH_BOTTOM_0);
  for (vector<Vec2f>::const_iterator it = bottomLines.begin(); it != bottomLines.end(); ++it)
  {
    float rho = (*it)[0];
    float theta = (*it)[1];
    float angle = theta * 180 / PI;
    if (angle > 75 && angle < 105)
    {
      bottomLineDetected = 1;
      bottomRho = rho;
      bottomRad = theta;
      break;
    }
  }
  if (bottomLineDetected)
  {
    if (current_coolDown <= 0)
    {
      current_mode = FIRST_TURN_MODE;
      first_lineFrameCount = 0;
      #ifdef _ACTION
      turnTo(-14);
      controlLeft(BACKWARD, 15);
      controlRight(BACKWARD, 15);
      delay(BACK_DELAY_0);
      stopLeft();
      stopRight();
      #endif
    }
  }
  else
  {
    first_lineFrameCount = 0;
    current_mode = FIRST_STRAIGHT_MODE;
  }

  /* Debug images */
  #ifdef _DEBUG
  clog << "turn " << (first_isTurnLeft ? "left" : "right") << endl;
  imshow("canny", contours);
  imshow("bottomCanny", bottomContours);
  imshow("bottomDilate", bottomDilate);
  Mat result;
  image.copyTo(result);
  //point of intersection of the line with first row
  Point pt1(rightMaxRho / cos(rightMaxRad), 0);
  //point of intersection of the line with last row
  Point pt2((rightMaxRho - result.rows * sin(rightMaxRad)) / cos(rightMaxRad), result.rows);
  //Draw a line
  line(result, pt1, pt2, Scalar(0, 255, 255), 3, CV_AA);
  //point of intersection of the line with first row
  Point pt3(minRho / cos(minRad), 0);
  //point of intersection of the line with last row
  Point pt4((minRho - result.rows * sin(minRad)) / cos(minRad), result.rows);
  //Draw a line
  line(result, pt3, pt4, Scalar(0, 255, 255), 3, CV_AA);
  imshow("result", result);
  #endif
}

/**
 * Mode 1: first big turn
 *  */
bool first_delayLocked = false;
void firstTurnProcessing(const Mat &image)
{
  /* Determine the current action */
  #ifdef _ACTION
  clog << "big turn" << endl;
  /* Big turn */
  turnTo(bigTurnDegree_1);
  controlLeft(FORWARD, leftBigTurnSpeed);
  controlRight(FORWARD, rightBigTurnSpeed);
  if (!first_delayLocked) {
    delay(smallestTurnTime_1);
    first_delayLocked = true;
  }
  #endif

  /* Determine the next state */
  turnedNum ++;
  current_mode = SECOND_STRAIGHT_MODE;
  #ifdef _ACTION
  current_coolDown = bigTurn_coolDown;
  #endif
  first_delayLocked = false;

  /* Debug images */
  #ifdef _DEBUG
  clog << "big turn" << endl;
  current_mode = start_mode;
  #endif
}

/**
 * Mode 2: second straight line
 *  */
bool second_isTurnLeft = false;
int second_lineFrameCount = 0;
void secondStraightProcessing(const Mat &image)
{
  /* Determine the current turn direcion */
  Mat contours;
  Canny(image, contours, CANNY_LOWER_BOUND_2, CANNY_UPPER_BOUND_2);
  float rightMaxRad = PI / 2;
  float rightMaxRho = 0;
  float minRho = 0;
  float minRad = PI / 2;
  vector<Vec2f> lines;
  HoughLines(contours, lines, 1, PI / 180, HOUGH_2);
  for (vector<Vec2f>::const_iterator it = lines.begin(); it != lines.end(); ++it)
  {
    float rho = (*it)[0];   //First element is distance rho
    float theta = (*it)[1]; //Second element is angle theta
    float angle = theta * 180 / PI;
    if ((angle > 5 && angle < 80) || (angle > 100 && angle < 175))
    {
      if (theta > PI / 2 && theta > rightMaxRad)
      {
        rightMaxRad = theta;
        rightMaxRho = rho;
      }
      if (theta < minRad && theta < PI / 2)
      {
        minRad = theta;
        minRho = rho;
      }
    }
  }

  float right = PI - rightMaxRad;
  if (minRad > right + offsetDegree)
  {
    second_isTurnLeft = true;
  }
  else if (minRad < right - offsetDegree)
  {
    second_isTurnLeft = false;
  }
  else
  {
    second_isTurnLeft = !second_isTurnLeft;
  }
  #ifdef _ACTION
  clog << "turn " << (second_isTurnLeft ? "left" : "right") << endl;
  if (normalSpeedTime_2 > 0) {
    controlLeft(FORWARD, leftStraightSpeed);
    controlRight(FORWARD, rightStraightSpeed);
    normalSpeedTime_2 --;
  } else {
    if (highSpeedTime_2 > 0) {
      controlLeft(FORWARD, highSpeed_2);
      controlRight(FORWARD, highSpeed_2);
      highSpeedTime_2 --;
    } else {
      controlLeft(FORWARD, leftStraightSpeed);
      controlRight(FORWARD, rightStraightSpeed);
    }
  }
  if (second_isTurnLeft)
  {
    if (minRad > right + largeOffest) {
      turnTo(-largeLeftAngle);
    } else if (minRad > right + smallOffest) {
      turnTo(-leftAngle);
    } else{
      turnTo(-smallLeftAngle);
    }
  }
  else
  {
    if (minRad < right - largeOffest) {
      turnTo(largeRightAngle);
    } else if (minRad < right - smallOffest) {
      turnTo(rightAngle);
    } else{
      turnTo(smallRightAngle);
    }
  }
  #endif

  /* Determine the next state */
  // prepare bottom image (middle 50% of the main image), to detect some sure line, e.g. final

  Rect bottomRoi(image.cols / 4, image.rows / 8, image.cols / 2, image.rows / 3 * 2);
  Mat bottomImage = image(bottomRoi);
  Mat dilate_element = getStructuringElement(MORPH_RECT, Size(dilate_width_2, dilate_height_2));
  Mat bottomDilate;
  dilate(bottomImage, bottomDilate, dilate_element);
  Mat bottomContours;
  Canny(bottomDilate, bottomContours, CANNY_LOWER_BOUND_2, CANNY_UPPER_BOUND_2);
  int bottomLineDetected = 0;
  float bottomRho = 0;
  float bottomRad = 0;
  vector<Vec2f> bottomLines;
  HoughLines(bottomContours, bottomLines, 1, PI / 180, HOUGH_BOTTOM_2);
  for (vector<Vec2f>::const_iterator it = bottomLines.begin(); it != bottomLines.end(); ++it)
  {
    float rho = (*it)[0];
    float theta = (*it)[1];
    float angle = theta * 180 / PI;
    if (angle > 75 && angle < 105)
    {
      bottomLineDetected = 1;
      bottomRho = rho;
      bottomRad = theta;
      break;
    }
  }
  if (bottomLineDetected)
  {
    if (current_coolDown <= 0)
    {
      current_mode = SECOND_TURN_MODE;
      second_lineFrameCount = 0;
      #ifdef _ACTION
      turnTo(-14);
      controlLeft(BACKWARD, 15);
      controlRight(BACKWARD, 15);
      delay(BACK_DELAY_2);
      stopLeft();
      stopRight();
      #endif
    }
  }
  else
  {
    second_lineFrameCount = 0;
    current_mode = SECOND_STRAIGHT_MODE;
  }

  /* Debug images */
  #ifdef _DEBUG
  clog << "turn " << (second_isTurnLeft ? "left" : "right") << endl;
  imshow("canny", contours);
  imshow("bottom", bottomImage);
  imshow("bottomCanny", bottomContours);
  imshow("bottomDilate", bottomDilate);
  Mat result;
  image.copyTo(result);
  //point of intersection of the line with first row
  Point pt1(rightMaxRho / cos(rightMaxRad), 0);
  //point of intersection of the line with last row
  Point pt2((rightMaxRho - result.rows * sin(rightMaxRad)) / cos(rightMaxRad), result.rows);
  //Draw a line
  line(result, pt1, pt2, Scalar(0, 255, 255), 3, CV_AA);
  //point of intersection of the line with first row
  Point pt3(minRho / cos(minRad), 0);
  //point of intersection of the line with last row
  Point pt4((minRho - result.rows * sin(minRad)) / cos(minRad), result.rows);
  //Draw a line
  line(result, pt3, pt4, Scalar(0, 255, 255), 3, CV_AA);
  imshow("result", result);
  #endif
}

/**
 * Mode 3: second big turn
 *  */
bool second_delayLocked = false;
bool needTurnStraight_3 = true;
void secondTurnProcessing(const Mat &image)
{
  /* Determine the current action */
    /* Determine the current action */
  #ifdef _ACTION
  clog << "big turn" << endl;
  /* Big turn */
  turnTo(bigTurnDegree_3);
  controlLeft(FORWARD, leftBigTurnSpeed);
  controlRight(FORWARD, rightBigTurnSpeed);
  if (!second_delayLocked) {
    delay(smallestTurnTime_3);
    second_delayLocked = true;
  }
  #endif

  /* Determine the next state */
  turnedNum++;
  current_mode = THIRD_STRAIGHT_MODE;
  current_coolDown = bigTurn_coolDown;
  second_delayLocked = false;

  /* Debug images */
  #ifdef _DEBUG
  clog << "big turn" << endl;
  current_mode = start_mode;
  #endif
}

/**
 * Mode 4: third straight line
 *  */
bool third_isTurnLeft = false;
int third_lineFrameCount = 0;
void thirdStraightProcessing(const Mat &image)
{
  /* Determine the current turn direcion */
  Mat contours;
  Canny(image, contours, CANNY_LOWER_BOUND_4, CANNY_UPPER_BOUND_4);
  float rightMaxRad = PI / 2;
  float rightMaxRho = 0;
  float minRho = 0;
  float minRad = PI / 2;
  vector<Vec2f> lines;
  HoughLines(contours, lines, 1, PI / 180, HOUGH_4);
  for (vector<Vec2f>::const_iterator it = lines.begin(); it != lines.end(); ++it)
  {
    float rho = (*it)[0];   //First element is distance rho
    float theta = (*it)[1]; //Second element is angle theta
    float angle = theta * 180 / PI;
    if ((angle > 5 && angle < 80) || (angle > 100 && angle < 175))
    {
      if (theta > PI / 2 && theta > rightMaxRad)
      {
        rightMaxRad = theta;
        rightMaxRho = rho;
      }
      if (theta < minRad && theta < PI / 2)
      {
        minRad = theta;
        minRho = rho;
      }
    }
  }

  float right = PI - rightMaxRad;
  if (minRad > right + offsetDegree)
  {
    third_isTurnLeft = true;
  }
  else if (minRad < right - offsetDegree)
  {
    third_isTurnLeft = false;
  }
  else
  {
    third_isTurnLeft = !third_isTurnLeft;
  }
  #ifdef _ACTION
  clog << "turn " << (third_isTurnLeft ? "left" : "right") << endl;
  if (normalSpeedTime_4 > 0) {
    controlLeft(FORWARD, leftStraightSpeed);
    controlRight(FORWARD, rightStraightSpeed);
    normalSpeedTime_4 --;
  } else {
    if (highSpeedTime_4 > 0) {
      controlLeft(FORWARD, highSpeed_4);
      controlRight(FORWARD, highSpeed_4);
      highSpeedTime_4 --;
    } else {
      controlLeft(FORWARD, leftStraightSpeed);
      controlRight(FORWARD, rightStraightSpeed);
    }
  }
  if (third_isTurnLeft)
  {
    if (minRad > right + largeOffest) {
      turnTo(-largeLeftAngle);
    } else if (minRad > right + smallOffest) {
      turnTo(-leftAngle);
    } else{
      turnTo(-smallLeftAngle);
    }
  }
  else
  {
    if (minRad < right - largeOffest) {
      turnTo(largeRightAngle);
    } else if (minRad < right - smallOffest) {
      turnTo(rightAngle);
    } else{
      turnTo(smallRightAngle);
    }
  }
  #endif

  /* Determine the next state */
  // prepare bottom image (middle 50% of the main image), to detect some sure line, e.g. final
  Rect bottomRoi(image.cols / 4, image.rows / 8, image.cols / 2, image.rows / 3 * 2);
  Mat bottomImage = image(bottomRoi);
  Mat dilate_element = getStructuringElement(MORPH_RECT, Size(dilate_width_4, dilate_height_4));
  Mat bottomDilate;
  dilate(bottomImage, bottomDilate, dilate_element);
  Mat bottomContours;
  Canny(bottomDilate, bottomContours, CANNY_LOWER_BOUND_4, CANNY_UPPER_BOUND_4);
  int bottomLineDetected = 0;
  float bottomRho = 0;
  float bottomRad = 0;
  vector<Vec2f> bottomLines;
  HoughLines(bottomContours, bottomLines, 1, PI / 180, HOUGH_BOTTOM_4);
  for (vector<Vec2f>::const_iterator it = bottomLines.begin(); it != bottomLines.end(); ++it)
  {
    float rho = (*it)[0];
    float theta = (*it)[1];
    float angle = theta * 180 / PI;
    if (angle > 75 && angle < 105)
    {
      bottomLineDetected = 1;
      bottomRho = rho;
      bottomRad = theta;
      break;
    }
  }
  if (bottomLineDetected)
  {
    third_lineFrameCount++;
    if (third_lineFrameCount >= 2)
    {
      if (current_coolDown <= 0)
      {
        current_mode = FINAL_MODE;
        third_lineFrameCount = 0;
      }
    }
  }
  else
  {
    third_lineFrameCount = 0;
    current_mode = THIRD_STRAIGHT_MODE;
  }

  /* Debug images */
  #ifdef _DEBUG
  clog << "turn " << (third_isTurnLeft ? "left" : "right") << endl;
  imshow("canny", contours);
  imshow("bottomCanny", bottomContours);
  imshow("bottomDilate", bottomDilate);
  Mat result;
  image.copyTo(result);
  //point of intersection of the line with first row
  Point pt1(rightMaxRho / cos(rightMaxRad), 0);
  //point of intersection of the line with last row
  Point pt2((rightMaxRho - result.rows * sin(rightMaxRad)) / cos(rightMaxRad), result.rows);
  //Draw a line
  line(result, pt1, pt2, Scalar(0, 255, 255), 3, CV_AA);
  //point of intersection of the line with first row
  Point pt3(minRho / cos(minRad), 0);
  //point of intersection of the line with last row
  Point pt4((minRho - result.rows * sin(minRad)) / cos(minRad), result.rows);
  //Draw a line
  line(result, pt3, pt4, Scalar(0, 255, 255), 3, CV_AA);
  imshow("result", result);
  #endif
}

/**
 * Mode 5: final mode
 *  */
void finalProcessing(const Mat &image)
{
  /* Determine the current action */
  #ifdef _ACTION
  clog << "final" << endl;
  controlLeft(BACKWARD, leftStraightSpeed + 1);
  controlRight(BACKWARD, rightStraightSpeed + 1);
  stopLeft();
  stopRight();
  turnTo(5);
  turnTo(-13);

  controlLeft(FORWARD, leftStraightSpeed + 1);
  controlRight(FORWARD, rightStraightSpeed + 1);
  delay(finalRushTime);
  controlLeft(BACKWARD, leftStraightSpeed + 1);
  controlRight(BACKWARD, rightStraightSpeed + 1);
  stopLeft();
  stopRight();
  #endif

  /* Determine the next state*/
  current_mode = EXIT_MODE;

  /* Debug images */
  #ifdef _DEBUG
  clog << "final" << endl;
  current_mode = start_mode;
  turnedNum = 0;
  #endif
}

int main()
{
  readPara();
  clog << "select a start mode: \n0 first straight\n1 first turn\n2 second straight\n3 second turn\n4 third straight\n5 final" << endl;
  start_mode = getchar() - '0';
  current_mode = start_mode;
  clog << "mode " << current_mode << " start" << endl;
  #ifdef _ACTION
  init();
  turnTo(0);
  #endif

  VideoCapture capture(CAM_PATH);
  //If this fails, try to open as a video camera, through the use of an integer param
  if (!capture.isOpened())
  {
    capture.open(atoi(CAM_PATH.c_str()));
  }

  Mat image;
  clock_t t;

  while (true)
  {
    if (current_coolDown > 0)
    {
      current_coolDown--;
    }

    #ifndef _IMAGE_DEBUG
    /* Get the blurred-gray-contour result */
    capture >> image;
    if (image.empty())
      break;
    #endif

    #ifdef _IMAGE_DEBUG
    image = imread(IMAGE_FILE_NAME, CV_LOAD_IMAGE_ANYCOLOR);
    #endif

    #ifdef _TIME
    t = clock();
    #endif

    // prepare main image (middle 70%)
    resize(image, image, Size(120, 90));
    Rect roi(0, image.rows / 10 * 2, image.cols, image.rows / 10 * 7);
    Mat image2 = image(roi);
    Mat midImage;
    cvtColor(image2, midImage, CV_BGR2GRAY);
    Mat midImage2;
    GaussianBlur(midImage, midImage2, Size(3, 3), 0);

    Mat thresholdImage;
    threshold(midImage2, thresholdImage, thresholdBorder, 200, 0);

    #ifdef _TIME
    t = clock() - t;
    clog << "preprocessing time: " << (float)t/CLOCKS_PER_SEC << "s; ";
    clock_t process_start;
    process_start = clock();
    #endif

    // debug images
    #ifdef _DEBUG
    imshow("Gaussian", midImage2);
    imshow("threshold", thresholdImage);
    #endif

    /* State Machine */
    switch (current_mode)
    {
    case FIRST_STRAIGHT_MODE:
      firstStraightProcessing(thresholdImage);
      break;
    case FIRST_TURN_MODE:
      firstTurnProcessing(thresholdImage);
      break;
    case SECOND_STRAIGHT_MODE:
      secondStraightProcessing(thresholdImage);
      break;
    case SECOND_TURN_MODE:
      secondTurnProcessing(thresholdImage);
      break;
    case THIRD_STRAIGHT_MODE:
      thirdStraightProcessing(thresholdImage);
      break;
    case FINAL_MODE:
      finalProcessing(thresholdImage);
      break;
    case EXIT_MODE:
      clog << "exit" << endl;
      return 0;
      break;
    default:
      clog << "!ERROR: invalid current mode " << current_mode << endl;
      break;
    }
    #ifdef _TIME
    t = clock() - process_start;
    clog << "processing time: " << ((float)t)/CLOCKS_PER_SEC << "s" << endl;
    #endif

    #ifdef _VIDEO_DEBUG
    waitKey(60);
    #endif

    #ifndef _VIDEO_DEBUG
    waitKey(1);
    #endif
  }

  return 0;
}
