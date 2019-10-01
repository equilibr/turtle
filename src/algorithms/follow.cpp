#include "utility.h"

static Direction findDirection(ThreadedBrain &brain)
{
	auto sensor = brain.tileSensor();

	if (isSensorSet(brain, 1, 0))
		return Direction::Forward;

	if (isSensorSet(brain, 0, -1))
		return Direction::Right;

	if (isSensorSet(brain, 0, 1))
		return Direction::Left;

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

void Follow(ThreadedBrain &brain)
{
	//Run as long as we have where to move
	while (brain && advance(brain)) ;
}
