#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <vector>
#include <termios.h>
#include "GPIOlib.h"

#define PI 3.1415926

#define _ACTION

using namespace std;

static struct termios old, newSetting;

/**
 * Mode 0 first straight
 * */
int CANNY_LOWWER_BOUND_0 = 10;
int CANNY_UPPER_BOUND_0 = 50;
int HOUGH_0 = 20;
int dilate_width_0 = 8;
int dilate_height_0 = 5;
int HOUGH_BOTTOM_0 = 15;
int BACK_SPEED_0 = 11;
long BACK_DELAY_0 = 1000;

/**
 * Mode 1 first turn
 * */
long smallestTurnTime_1 = 800;
int bigTurnDegree_1 = 15;

/**
 * Mode 2 second straight
 * */
int CANNY_LOWWER_BOUND_2 = 10;
int CANNY_UPPER_BOUND_2 = 50;
int HOUGH_2 = 20;
int dilate_width_2 = 8;
int dilate_height_2 = 5;
int HOUGH_BOTTOM_2 = 15;
int BACK_SPEED_2 = 11;
long BACK_DELAY_2 = 1000;

/**
 * Mode 3 second turn
 * */
long smallestTurnTime_3 = 800;
int bigTurnDegree_3 = 15;

/**
 * Mode 4 third straight
 * */
int CANNY_LOWWER_BOUND_4 = 10;
int CANNY_UPPER_BOUND_4 = 50;
int HOUGH_4 = 20;
int dilate_width_4 = 8;
int dilate_height_4 = 5;
int HOUGH_BOTTOM_4 = 15;
int BACK_SPEED_4 = 11;
long BACK_DELAY_4 = 1000;

/**
 * Mode 5 final
 * */
long finalRushTime = 200;

void readPara()
{
  ifstream file("parameter.txt");
  string line;
  if (file.is_open())
  {
    getline(file, line);
    stringstream stream(line);
    stream >> leftStraightSpeed >> rightStraightSpeed >> leftDegree >> rightDegree >> leftSpeed >> rightSpeed >> bigTurnDegree >> smallestTurnTime;
    clog << "previous: " << leftStraightSpeed << " " << rightStraightSpeed << " "
         << leftDegree << " " << rightDegree << " "
         << leftSpeed << " " << rightSpeed << " "
         << bigTurnDegree << " " << smallestTurnTime << " " << endl;
    leftDegree = -leftDegree;
  }
}

/* Initialize new terminal i/o settings */
void initTermios(int echo)
{
  tcgetattr(0, &old); /* grab old terminal i/o settings */
  newSetting = old; /* make new settings same as old settings */
  newSetting.c_lflag &= ~ICANON; /* disable buffered i/o */
  newSetting.c_lflag &= echo ? ECHO : ~ECHO; /* set echo mode */
  tcsetattr(0, TCSANOW, &newSetting); /* use these new terminal i/o settings now */
}

/* Restore old terminal i/o settings */
void resetTermios(void)
{
  tcsetattr(0, TCSANOW, &old);
}

/* Read 1 character - echo defines echo mode */
char getch_(int echo)
{
  char ch;
  initTermios(echo);
  ch = getchar();
  resetTermios();
  return ch;
}

/* Read 1 character without echo */
char getch(void)
{
  return getch_(0);
}

/* Read 1 character with echo */
char getche(void)
{
  return getch_(1);
}

using namespace std;

void init() {
	#ifdef _ACTION
	GPIO::init();
	#endif
	clog << "init" << endl;
}

int turnTo(int degree) {
	#ifdef _ACTION
	GPIO::turnTo(degree);
	#endif
	clog << "turn to " << degree << endl;
	return 1;
}

void stopLeft() {
	#ifdef _ACTION
	GPIO::stopLeft();
	#endif
	clog << "stop left" << endl;
}

void stopRight() {
	#ifdef _ACTION
	GPIO::stopRight();
	#endif
	clog << "stop right" << endl;
}

void controlLeft(int speed) {
	#ifdef _ACTION
	GPIO::controlLeft(direction, speed);
	#endif
	clog << "control left " << speed << endl;
}

void controlRight(int speed) {
	#ifdef _ACTION
	GPIO::controlRight(direction, speed);
	#endif
	clog << "control right " << speed << endl;
}

/** Note the leftDegree parameter is positive while save its negative */
void savePara(int leftStraightSpeed, int rightStraightSpeed,
  int leftDegree, int rightDegree, int leftTurnSpeed, int rightTurnSpeed,
  int turnDegree, int smallestTurnTime) {
    ofstream file;
    file.open("parameter.txt", ios::out | ios::trunc);
    file << leftStraightSpeed << " "<< rightStraightSpeed << " "
      << -leftDegree << " "<< rightDegree << " "
      << leftTurnSpeed << " "<< rightTurnSpeed << " "
      << turnDegree << " "<< smallestTurnTime << " " << endl;
    file.close();
  }

int main() {
  readPara();
	init();
	while (true) {
		int c = getch();
		switch (c) {
			case 'w':
				controlLeft(leftStraightSpeed);
				controlRight(rightStraightSpeed);
				break;
			case 's':
				stopLeft();
				stopRight();
				break;
			case 'a':
				turnTo(-leftDegree);
				break;
			case 'd':
				turnTo(rightDegree);
				break;
			case 'u':
				smallestTurnTime = smallestTurnTime + 100;
				clog << "smallest turn time: " << smallestTurnTime << endl;
				break;
			case 'j':
				smallestTurnTime = smallestTurnTime - 100;
				clog << "smallest turn time: " << smallestTurnTime << endl;
				break;
			case 'q':
				leftDegree = (leftDegree > 44) ? leftDegree : leftDegree + 1;
				clog << "left degree " << leftDegree << endl;
				break;
			case 'e':
				rightDegree = (rightDegree > 44) ? rightDegree : rightDegree + 1;
				clog << "right degree " << rightDegree << endl;
				break;
			case 'x':
				turnTo(0);
				break;
			case ' ':
				direction = direction == 1 ? 0 : 1;
				clog << "direction " << direction << endl;
				break;
      case 'p':
        #ifdef _ACTION
        GPIO::controlLeft(leftDirection, leftSpeed);
        GPIO::controlRight(rightDirection, rightSpeed);
        #endif
        break;
      case '1':
        leftDirection = 1;
        clog << "left direction " << leftDirection << endl;
        break;
      case '2':
        leftDirection = 0;
        clog << "left direction " << leftDirection << endl;
        break;
      case '3':
        rightDirection = 1;
        clog << "right direction " << rightDirection << endl;
        break;
      case '4':
        rightDirection = 0;
        clog << "right direction " << rightDirection << endl;
        break;
      case '5':
        leftSpeed = leftSpeed > 49? leftSpeed: leftSpeed + 1;
        clog << "left Speed" << leftSpeed << endl;
        break;
      case '6':
        leftSpeed = leftSpeed < 1? leftSpeed: leftSpeed - 1;
        clog << "left Speed" << leftSpeed << endl;
        break;
      case '7':
        rightSpeed = rightSpeed > 49? rightSpeed: rightSpeed + 1;
        clog << "right Speed" << rightSpeed << endl;
        break;
      case '8':
        rightSpeed = rightSpeed < 1? rightSpeed: rightSpeed - 1;
        clog << "right Speed" << rightSpeed << endl;
        break;
      case 'z':
        leftDegree = (leftDegree < 1) ? leftDegree : leftDegree - 1;
				clog << "left degree " << leftDegree << endl;
        break;
      case 'c':
        rightDegree = (rightDegree < 1) ? rightDegree : rightDegree - 1;
				clog << "right degree " << rightDegree << endl;
        break;
      case 'l':
        savePara(leftStraightSpeed, rightStraightSpeed, leftDegree, rightDegree, leftSpeed, rightSpeed, bigTurnDegree, smallestTurnTime);
        clog << "save: " << leftStraightSpeed << " "
          << rightStraightSpeed << " " << -leftDegree << " "
          << rightDegree << " "<< leftSpeed << " "
          << rightSpeed << " "<< bigTurnDegree << " " << smallestTurnTime << " " << endl;
        break;
      case 'v':
        leftStraightSpeed ++;
        clog << "left straight speed: " << leftStraightSpeed << endl;
        break;
      case 'b':
        leftStraightSpeed --;
        clog << "left straight speed: " << leftStraightSpeed << endl;
        break;
      case 'n':
        rightStraightSpeed ++;
        clog << "right straight speed: " << rightStraightSpeed << endl;
        break;
      case 'm':
        rightStraightSpeed --;
        clog << "right straight speed: " << rightStraightSpeed << endl;
        break;
			default:
				stopLeft();
				stopRight();
				break;
		}
	}
	return 0;
}
