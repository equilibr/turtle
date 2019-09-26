#include "World.h"

#include "TurtleActor.h"

#include <algorithm>

using namespace Turtle;

World::World() :
	m_mainActor(*this)
{
	m_actors.push_back(m_mainActor);

	//Add all graphical elements to the scene
	m_scene.addChild(m_floor.root());
	m_scene.addChild(m_mainActor.root());
}

void World::resize(const Index2D & size, const Position2D & tileSize)
{
	m_floor.reset(size, tileSize);

	boundingBox[0] = -m_floor.halfSize();
	boundingBox[1] = m_floor.halfSize();
}

void World::reset()
{
	m_floor.clear();
	m_mainActor.reset();
}

bool World::operator()(int steps)
{
	bool dirty = false;

	//Execute all the actors
	std::for_each(
				m_actors.begin(), m_actors.end(),
				[&dirty, steps](auto & actor) { dirty |= actor(steps);});

	return dirty;
}

Position World::edge(const Position &from, const Position &to)
{
	//If the destination position is completely withing the bounding box
	if (clamp(to) == to)
		//It needs no modifications
		return to;

	//Reference taken from here:
	//https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-box-intersection

	//Find the direction, for a "t" value of 1.0
	Position direction = to - from;

	//Find "t" at intersection with the bounds
	Position tLow = (boundingBox[0] - from) / direction;
	Position tHigh = (boundingBox[1] - from) / direction;

	//We're interested only in the forward direction
	Position tForward = tLow.max(tHigh);

	//We're interested in the smallest "t" that intersect with one of the axes
	Position::value_type t = tForward.min();

	//Return the points at the found t
	return from + direction * t;
}

