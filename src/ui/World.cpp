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
	m_mainActor.reset();
	m_floor.clear();
}

void World::setImage(const QImage & image)
{
	//Make sure the image size is 2*N + 1 at both dimention
	// and copy a subset if not.

	auto size = image.size();

	if (!(size.width() % 2))
		size.setWidth(size.width()-1);

	if (!(size.height() % 2))
		size.setHeight(size.height()-1);

	//Calculate the half-size
	Index2D halfSize = Index2D
	{
			static_cast<Index2D::value_type>((size.width() - 1) / 2),
			static_cast<Index2D::value_type>((size.height() - 1) / 2)
	};

	resize(halfSize);
	m_floor.setImage(image.copy({{0,0},size}));
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

Position World::clamp(const Position & position, const Position & margin)
{
	return position.max(boundingBox[0] + margin).min(boundingBox[1] - margin);
}

Position World::edge(const Position &from, const Position &to, const Position & margin)
{
	//If the destination position is completely withing the bounding box
	if (clamp(to, margin) == to)
		//It needs no modifications
		return to;

	//Find the direction, for a "t" value of 1.0
	Position direction = to - from;

	//Find "t" at intersection with the bounds
	Position tLow = (boundingBox[0] + margin - from) / direction;
	Position tHigh = (boundingBox[1] - margin - from) / direction;

	//We're interested only in the forward direction
	Position tForward = tLow.max(tHigh);

	//We're interested in the smallest "t" that intersect with one of the axes
	Position::value_type t = tForward.min();

	//Return the points at the found t
	return from + direction * t;
}

