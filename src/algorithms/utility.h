#ifndef UTILITY_H
#define UTILITY_H

#include "ThreadedBrain.h"

//Move relative to current position
void goTo(ThreadedBrain & brain, int forward, int side, bool keepHeading = true);

//Draw at relative position
void drawAt(ThreadedBrain & brain, int forward, int side, QColor color);


//Test sensor color at the given position
//---------------------------------------

bool isSensorDark(ThreadedBrain & brain, int forward, int side);
bool isSensorRed(ThreadedBrain & brain, int forward, int side);
bool isSensorGreen(ThreadedBrain & brain, int forward, int side);
bool isSensorBlue(ThreadedBrain & brain, int forward, int side);

//Test the color at the given position
//------------------------------------
bool isDark(QColor color, double threshold = 0.5);
bool isRed(QColor color, double threshold = 0.5);
bool isGreen(QColor color, double threshold = 0.5);
bool isBlue(QColor color, double threshold = 0.5);


#endif // UTILITY_H
