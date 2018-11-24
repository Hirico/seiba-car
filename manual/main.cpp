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

int direction = 1;  // 前进1 倒车0，影响按w
int leftDirection = 1; //
int rightDirection = 1; // 这两个是用P键后轮转向的时候两个后轮的方向

/* 以下是可以和Seiba交流的、在参数文件里实际保存的参数 */
int leftSpeed = 10; // 大转弯时的后轮速度。此处转弯速度都为正值，方向由Seiba写死左轮后转、右轮前转
int rightSpeed = 10; // 同上
int leftDegree = 25; // 小转弯时的舵机角度，此变量在该程序里一直是正值，文件读写时会自动加负号（文件里存的是负值，以便Seiba读到之后直接拿来用）
int rightDegree = 20; // 小转弯时的舵机角度
int leftStraightSpeed = 10; // 直走时左轮速度
int rightStraightSpeed = 10; // 直走时右轮速度
int bigTurnDegree = -13; // 现在还没有提供改这个数值的方法
long smallestTurnTime = 800; // 大转弯的最短必需时间 deprecated

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
				clog << "this is deprecated, please edit controller parameter file to adjust turn time " << endl;
				break;
			case 'j':
				smallestTurnTime = smallestTurnTime - 100;
				clog << "this is deprecated, please edit controller parameter file to adjust turn time " << endl;
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
