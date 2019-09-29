#include "utility.h"

enum class Direction
{
	None,
	Forward,
	Left,
	Right,
	LeftDiagonal,
	RightDiagonal
};

void logDirection(ThreadedBrain & brain, Direction direction)
{
	switch (direction)
	{
		case Direction::None:
			brain.log("None");
			break;

		case Direction::Forward:
			brain.log("Forward");
			break;

		case Direction::Left:
			brain.log("Left");
			break;

		case Direction::Right:
			brain.log("Right");
			break;

		case Direction::LeftDiagonal:
			brain.log("Left diagonal");
			break;

		case Direction::RightDiagonal:
			brain.log("Right diagonal");
			break;

	}
}

Direction findDirection(ThreadedBrain &brain)
{
	auto sensor = brain.tileSensor();

	if (isSensorDark(brain, 1, 0))
		return Direction::Forward;

	if (isSensorDark(brain, 0, -1))
		return Direction::Left;

	if (isSensorDark(brain, 0, 1))
		return Direction::Right;

	if (isSensorDark(brain, 1, -1))
		return Direction::LeftDiagonal;

	if (isSensorDark(brain, 1, 1))
		return Direction::RightDiagonal;

	return Direction::None;
}

void moveToDirection(ThreadedBrain &brain, Direction direction)
{
	switch (direction)
	{
		case Direction::None:
			//Do nothing
			break;

		case Direction::Forward:
			brain.move();
			break;

		case Direction::Left:
			brain.rotate(0.25);
			brain.move();
			break;

		case Direction::Right:
			brain.rotate(-0.25);
			brain.move();
			break;

		case Direction::LeftDiagonal:
			brain.rotate(0.25);
			brain.move();
			brain.rotate(-0.25);
			brain.move();
			break;

		case Direction::RightDiagonal:
			brain.rotate(-0.25);
			brain.move();
			brain.rotate(0.25);
			brain.move();
			break;
	}
}

bool advance(ThreadedBrain &brain)
{
	Direction direction = findDirection(brain);
	logDirection(brain, direction);
	if (direction == Direction::None)
		return false;

	moveToDirection(brain, direction);
	return true;
}

void Follow(ThreadedBrain &brain)
{
	//Run as long as we have where to move
	while (brain && advance(brain)) ;
}
