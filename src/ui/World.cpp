#include "World.h"

using namespace Turtle;

World::World() :
	m_scene(m_floor.root())
{
}

bool World::operator()()
{
	return true;
}

