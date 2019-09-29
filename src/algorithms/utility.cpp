#include "utility.h"

constexpr int maximumBits = 32;

void goTo(ThreadedBrain & brain, int forward, int side, bool keepHeading)
{
	brain.move(forward);
	if (side > 0)
	{
		brain.rotate(-0.25);
		brain.move(side);
		if (keepHeading)
			brain.rotate(0.25);
	}
	else
	{
		brain.rotate(0.25);
		brain.move(-side);
		if (keepHeading)
			brain.rotate(-0.25);
	}
}

void drawAt(ThreadedBrain & brain, int forward, int side, QColor color)
{
	goTo(brain, forward, side, true);
	brain.setPenColor(color);
	brain.setPenDown(true);
	brain.setPenDown(false);
	goTo(brain, -forward, -side, true);
}

bool isSensorDark(ThreadedBrain & brain, Turtle::TilePosition2D offset)
{
	return isDark(brain.tileSensor().get(offset));
}

bool isSensorDark(ThreadedBrain & brain, int forward, int side)
{
	return isSensorDark(brain, {forward, side});
}

bool isSensorRed(ThreadedBrain & brain, Turtle::TilePosition2D offset)
{
	return isRed(brain.tileSensor().get(offset));
}

bool isSensorRed(ThreadedBrain & brain, int forward, int side)
{
	return isSensorRed(brain, {forward, side});
}

bool isSensorGreen(ThreadedBrain & brain, Turtle::TilePosition2D offset)
{
	return isGreen(brain.tileSensor().get(offset));
}

bool isSensorGreen(ThreadedBrain & brain, int forward, int side)
{
	return isSensorGreen(brain, {forward, side});
}

bool isSensorBlue(ThreadedBrain & brain, Turtle::TilePosition2D offset)
{
	return isBlue(brain.tileSensor().get(offset));
}

bool isSensorBlue(ThreadedBrain & brain, int forward, int side)
{
	return isSensorBlue(brain, {forward, side});
}

bool isDark(QColor color, double threshold)
{
	return (color.redF() + color.greenF() + color.blueF()) / 3 < threshold;
}

bool isRed(QColor color, double threshold)
{
	return color.redF() > threshold;
}

bool isGreen(QColor color, double threshold)
{
	return color.greenF() > threshold;
}

bool isBlue(QColor color, double threshold)
{
	return color.blueF() > threshold;
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

			if (isSensorGreen(brain, offset))
				ok = true;

			brain.move();
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
	if (markers && isSensorRed(brain, offset))
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
		result |= (isSensorDark(brain, offset) ? 1 : 0) << readBits;
		readBits++;

		brain.move();
		steps++;

		//Check for the end marker
		if (markers && isSensorRed(brain, offset))
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

	brain.setPenDown(true);

	if (markers)
	{
		brain.setPenColor(Qt::green);
		brain.move();
		steps++;
	}

	for (int i = 0; i < bits; ++i)
	{
		if (!brain)
		{
			if (valid)
				*valid = false;
			return;
		}

		if (number & (1 << i))
			brain.setPenColor(Qt::black);
		else
			brain.setPenColor(Qt::white);

		brain.move();
		steps++;
	}

	if (markers)
		brain.setPenColor(Qt::red);

	brain.setPenDown(false);

	if (reset)
		brain.move(-steps);

	if (valid)
		*valid = true;
}
