#include "utility.h"

#include <array>

static Direction findDirection(ThreadedBrain &brain)
{
	auto sensor = brain.tileSensor();

	if (isSensorSet(brain, 0, -1))
		return Direction::Right;

	if (isSensorSet(brain, 1, 0))
		return Direction::Forward;

	if (isSensorSet(brain, 0, 1))
		return Direction::Left;

	if (isSensorSet(brain, -1, 0))
		return Direction::Reverse;

	return Direction::None;
}

static bool advance(ThreadedBrain &brain)
{
	Direction direction = findDirection(brain);
	if (direction == Direction::None)
		return false;

	moveToDirection(brain, direction);
	return true;
}

void MazeSolverWall(ThreadedBrain &brain)
{
	std::array<QColor, 4> colors =
	{
		Qt::black,
		Qt::blue,
		Qt::magenta,
		Qt::red
	};

	//Find the top right corner to know where
	// the maze ends.
	Turtle::Position2D positionTopRight {1000,1000};
	while (brain)
	{
		brain.setTargetPosition(positionTopRight, true);
		const Turtle::Position2D current = brain.getCurrentPosition();
		if (current < positionTopRight)
		{
			positionTopRight = current;
			break;
		}
		positionTopRight *= 2;
	};

	//Go to the lower left and start searching for the entrance
	brain.setTargetPosition(-positionTopRight, true);
	brain.setTargetAngle(0, true);

	brain.log("Searching for entrance");
	while (brain)
	{
		if (isSensorSet(brain, {0,0}))
			break;

		brain.jump();

		if (brain.getCurrentPosition().x() >= positionTopRight.x())
		{
			brain.log("<font color=\"red\">No entrance found!</font>");
			return;
		}
	}

	brain.turnLeft();
	brain.setDirectionalTile(Qt::red, {0,0});
	//At this point we're at the maze entrance, looking into the maze

	//Solve the maze using the right hand rule

	//Run as long as we have where to move and we're not at the end
	while (brain)
	{
		if (!advance(brain))
		{
			brain.log("<font color=\"red\">No more possible moves.</font>");
			break;
		}

		if (brain.getCurrentPosition().y() >= positionTopRight.y())
		{
			brain.setDirectionalTile(Qt::green, {0,0});
			brain.log("<font color=\"green\">Reached end of maze!</font>");
			break;
		}

		if (isSensorColor(brain, {0,0}, Qt::red))
		{
			brain.log("<font color=\"red\">Looping!</font>");
			break;
		}

		for (size_t i = 0; i < colors.size()-1; ++i)
			if (isSensorColor(brain, {0,0}, colors[i]))
			{
				brain.setDirectionalTile(colors[i+1], {0,0});
				break;
			}
	}
}
