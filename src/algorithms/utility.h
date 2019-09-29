#ifndef UTILITY_H
#define UTILITY_H

#include "ThreadedBrain.h"

//goto top-right
void gotoTR(ThreadedBrain &brain);

//Move relative to current position
void goTo(ThreadedBrain & brain, int forward, int side, bool keepHeading = true);

//Draw at relative position
void drawAt(ThreadedBrain & brain, int forward, int side, QColor color);


//Test sensor color at the given position
//---------------------------------------

bool isSensorDark(ThreadedBrain & brain, Turtle::TilePosition2D offset);
bool isSensorDark(ThreadedBrain & brain, int forward, int side);

bool isSensorRed(ThreadedBrain & brain, Turtle::TilePosition2D offset);
bool isSensorRed(ThreadedBrain & brain, int forward, int side);

bool isSensorGreen(ThreadedBrain & brain, Turtle::TilePosition2D offset);
bool isSensorGreen(ThreadedBrain & brain, int forward, int side);

bool isSensorBlue(ThreadedBrain & brain, Turtle::TilePosition2D offset);
bool isSensorBlue(ThreadedBrain & brain, int forward, int side);


//Test the color at the given position
//------------------------------------
bool isDark(QColor color, double threshold = 0.5);
bool isRed(QColor color, double threshold = 0.5);
bool isGreen(QColor color, double threshold = 0.5);
bool isBlue(QColor color, double threshold = 0.5);

/**
   @brief Read a number
   @param brain The brain
   @param bits Maximum number of bits to read. Can be zero if markers are used.
   @param reset Go back to origin after reading
   @param markers Use start/stop markers
   @param lookahead The number of steps to look ahead for a start marker
   @param offset The sensor offset to use
   @param valid True when no error detected
   @return The read number
 */

int readNumber(ThreadedBrain & brain,
		int bits = 8,
		bool reset = true,
		bool markers = false,
		int lookahead = 0,
		Turtle::TilePosition2D offset = {},
		bool * valid = nullptr);

/**
   @brief Write a number
   @param brain The brain
   @param number The number to write
   @param bits Number of bits to write
   @param reset Go back to origin after writing
   @param markers Use start/stop markers
   @param valid True when no error detected
 */
void writeNumber(ThreadedBrain & brain,
		int number,
		int bits = 8,
		bool reset = true,
		bool markers = false,
		bool * valid = nullptr);


#endif // UTILITY_H
