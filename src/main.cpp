#include "main.h"

#include <cmath>
#include <vector>
#include <array>

const double pi = acos(-1);

using Point = struct { double x,y; };
using Segment = struct { Point from, to; };
using Segments = std::vector<Segment>;

static double currentX;
static double currentY;
static double heading;


void moveTo(ThreadedBrain & brain, double x, double y)
{
	const double dX = x - currentX;
	const double dY = y - currentY;

	//Calculate true heading
	const double trueHeading = atan2(dY, dX) / (2 * pi);

	//Calculate heading delta
	const double dHeading = fmod(trueHeading - heading + 1.5, 1.0) - 0.5;
	brain.rotate(dHeading);

	//Calculate motion distance
	const double distance = hypot(dX, dY);
	brain.move(distance);

	currentX = x;
	currentY = y;
	heading = trueHeading;
}

void drawCircle(ThreadedBrain & brain, double radius)
{
	//Calculate approximate step size
	const double stepSize = 1.0 / (2.0 * pi * radius);

	brain.log(
				QString("Drawing circle with radius of %1, using %2 steps.")
				.arg(radius)
				.arg(1.0 / stepSize));

	//Move to the starting point and start drwaing
	moveTo(brain,radius,0);
	brain.setPenDown(true);

	double angle = 0.0;
	while(angle < 1.0)
	{
		if (!brain)
			return;

		//Calculate next coordinate
		double x = radius * cos(2*pi*angle);
		double y = radius * sin(2*pi*angle);

		brain.setPenColor(angle,1);
		moveTo(brain, x, y);
		angle += stepSize;
	}

	brain.setPenDown(false);
	moveTo(brain,0,0);
}

void drawSpiral(ThreadedBrain & brain, double radius, double distance, double twists)
{
	//Calculate approximate step size
	const double stepSize = 1.0 / (2.0 * pi * radius);

	brain.log(
				QString("Drawing spiral with radius of %1, %3 twists at distance %4, using %2 steps.")
				.arg(radius)
				.arg(1.0 / stepSize * twists)
				.arg(twists)
				.arg(distance));

	//Move to the starting point and start drwaing
	moveTo(brain,radius,0);
	brain.setPenDown(true);

	double angle = 0.0;
	while(angle < twists)
	{
		if (!brain)
			return;

		//Calculate next coordinate
		double x = (radius - distance*angle) * cos(2*pi*angle);
		double y = (radius - distance*angle) * sin(2*pi*angle);

		brain.setPenColor(angle/twists,1);
		moveTo(brain, x, y);
		angle += stepSize;
	}

	brain.setPenDown(false);
	moveTo(brain,0,0);
}


void drawSextagram(ThreadedBrain & brain, double radius)
{
	brain.log(
				QString("Drawing sextagram with radius of %1.")
				.arg(radius));

	brain.setPenColor(0,0,1);

	for (int j = 0; j < 2; ++j)
	{
		brain.setPenDown(false);
		int i = 0;
		do
		{
			if (!brain)
				return;

			const double angle = 1.0 * (2*i+j) / 6 + 1.0/12;

			//Calculate next coordinate
			double x = radius * cos(2*pi*angle);
			double y = radius * sin(2*pi*angle);

			moveTo(brain, x, y);
			brain.setPenDown(true);
		} while(++i < 4);
	}

	brain.setPenDown(false);
	moveTo(brain,0,0);
}

void drawVectors(ThreadedBrain & brain, Segments lines, Point offset = {})
{
	for (auto segment : lines)
	{
		if (!brain)
			return;

		brain.setPenDown(false);
		moveTo(brain,
			   segment.from.x + offset.x,
			   segment.from.y + offset.y);

		brain.setPenDown(true);
		moveTo(brain,
			   segment.to.x + offset.x,
			   segment.to.y + offset.y);
	}

	brain.setPenDown(false);
}

void drawI(ThreadedBrain & brain, double height, Point offset = {})
{
	Segments segments
	{
		{{-height/2,height},{height/2,height}},
		{{0,height},{0,-height}},
		{{-height/2,-height},{height/2,-height}},
	};

	brain.log("Drawing I");
	brain.setPenColor(0,0,0);
	drawVectors(brain, segments, offset);
	moveTo(brain,0,0);
}

void MainBrain(ThreadedBrain & brain)
{
	currentX = 0;
	currentY = 0;
	heading = 0;

	const double radius = brain.getDouble("Radius",{},49);
	drawI(brain, radius/8);
	drawSextagram(brain, radius);
	drawCircle(brain, radius);
	drawSpiral(brain,radius/2, 2, 5);
}
