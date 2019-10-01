#include "utility.h"
#include "Checker.h"

#include <cmath>

constexpr int maximumBits = 32;

void gotoTR(ThreadedBrain &brain)
{
	brain.setTargetPosition({1000,1000});
	brain.setTargetAngle(-0.5);
	brain.jump({3, 3});
}

bool isSensorSet(ThreadedBrain & brain, Turtle::TilePosition2D offset)
{
	return isSet(brain.tileSensor().get(offset));
}

bool isSensorSet(ThreadedBrain & brain, int forward, int side)
{
	return isSensorSet(brain, {forward, side});
}

bool isSet(QColor color, double threshold)
{
	return !((color.saturationF() < threshold) && (color.valueF() > threshold));
}

bool isSensorColor(ThreadedBrain & brain, Turtle::TilePosition2D offset, QColor test)
{
	return isColor(brain.tileSensor().get(offset), test);
}

bool isSensorColor(ThreadedBrain & brain, int forward, int side, QColor test)
{
	return isSensorColor(brain, {forward, side}, test);
}

bool isColor(QColor color, QColor test, double margin, double threshold)
{
	if (!isSet(color, threshold))
		return false;

	//Test for color similarity
	return fabs((fmod(color.hueF() - test.hueF() + 1.5, 1.0) - 0.5)) <= margin;
}


int readNumber(
		ThreadedBrain & brain,
		int bits,
		bool reset,
		bool markers,
		int lookahead,
		Turtle::TilePosition2D offset,
		bool * valid)
{
	//General status
	bool ok;

	//Number of steps moved
	int steps = 0;

	//Read status
	bool done;

	//The final result
	int result;

	//Number of bits already read
	int readBits;


	//Make sure the inputs are valid
	if ((!markers && !bits) || (bits > maximumBits))
	{
		if (valid)
			*valid = false;

		return 0;
	}

	//Do not read more than allowed
	const int requestedBits = !bits ? maximumBits : bits;

	if (markers)
	{
		//Look for the start marker
		ok = false;

		do
		{
			if (!brain)
			{
				if (valid)
					*valid = false;
				return 0;
			}

			if (isSensorColor(brain, offset, Qt::green))
				ok = true;

			brain.jump();
			steps++;

		} while (!ok && (steps < lookahead));
	}
	else
		ok = true;

	//Read the bits
	done = false;
	result = 0;
	readBits = 0;

	//Check for the end marker
	//This handles the 0-bits case
	if (markers && isSensorColor(brain, offset, Qt::red))
		done = true;

	while (ok && !done)
	{
		if (!brain)
		{
			if (valid)
				*valid = false;
			return 0;
		}

		//Read the current bit
		result |= (isSensorSet(brain, offset) ? 1 : 0) << readBits;
		readBits++;

		brain.jump();
		steps++;

		//Check for the end marker
		if (markers && isSensorColor(brain, offset, Qt::red))
			done = true;

		//If no end marker was found make sure we do not
		// go over the requested bits count
		if (!done && (readBits >= requestedBits))
		{
			done = true;
			//When no marker is used this is the normal stop condition
			ok = !markers;
		}
	}

	if (reset)
		brain.move(-steps);

	if (valid)
		*valid = ok;

	return result;
}

void writeNumber(
		ThreadedBrain & brain,
		int number,
		int bits,
		bool reset,
		bool markers,
		bool *valid)
{
	//Make sure the inputs are valid
	if (bits > maximumBits)
	{
		if (valid)
			*valid = false;

		return;
	}

	//Number of steps moved
	int steps = 0;

	if (markers)
	{
		brain.setPenColor(Qt::green);
		brain.setPenDown(true);
		brain.setPenDown(false);
		brain.jump();
		steps++;
	}

	Checker checker;

	for (int i = 0; i < bits; ++i)
	{
		if (!brain)
		{
			if (valid)
				*valid = false;
			return;
		}

		brain.setPenColor(checker((number & (1 << i))));
		brain.setPenDown(true);
		brain.setPenDown(false);

		brain.jump();
		steps++;
	}

	if (markers)
	{
		brain.setPenColor(Qt::red);
		brain.setPenDown(true);
		brain.setPenDown(false);
	}

	if (reset)
		brain.move(-steps);

	if (valid)
		*valid = true;
}
