#include "utility.h"

enum class Direction
{
	None,
	Forward,
	Reverse,
	Left,
	Right,
	LeftDiagonal,
	RightDiagonal
};

Direction findDirection(ThreadedBrain &brain)
{
	auto sensor = brain.tileSensor();

	if (isSensorSet(brain, 1, 0))
		return Direction::Forward;

	if (isSensorSet(brain, 0, -1))
		return Direction::Left;

	if (isSensorSet(brain, 0, 1))
		return Direction::Right;

	if (isSensorSet(brain, 1, -1))
		return Direction::LeftDiagonal;

	if (isSensorSet(brain, 1, 1))
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

		case Direction::Reverse:
			brain.rotate();
			break;

		case Direction::Left:
			brain.turnLeft();
			brain.move();
			break;

		case Direction::Right:
			brain.turnRight();
			brain.move();
			break;

		case Direction::LeftDiagonal:
			brain.turnLeft();
			brain.move();
			brain.turnRight();
			brain.move();
			break;

		case Direction::RightDiagonal:
			brain.turnRight();
			brain.move();
			brain.turnLeft();
			brain.move();
			break;
	}
}

bool advance(ThreadedBrain &brain)
{
	Direction direction = findDirection(brain);
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
