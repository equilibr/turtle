#include "utility.h"

void goTo(ThreadedBrain & brain, int forward, int side, bool keepHeading)
{
	brain.move(forward);
	if (side > 0)
	{
		brain.rotate(0.25);
		brain.move(side);
		if (!keepHeading)
			brain.rotate(-0.25);
	}
	else
	{
		brain.rotate(-0.25);
		brain.move(-side);
		if (!keepHeading)
			brain.rotate(0.25);
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

bool isSensorDark(ThreadedBrain & brain, int forward, int side)
{
	return isDark(brain.tileSensor().get(forward,side));
}

bool isSensorRed(ThreadedBrain & brain, int forward, int side)
{
	return isRed(brain.tileSensor().get(forward,side));
}

bool isSensorGreen(ThreadedBrain & brain, int forward, int side)
{
	return isGreen(brain.tileSensor().get(forward,side));
}

bool isSensorBlue(ThreadedBrain & brain, int forward, int side)
{
	return isBlue(brain.tileSensor().get(forward,side));
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

