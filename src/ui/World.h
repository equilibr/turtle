#ifndef WORLD_H
#define WORLD_H

#include "Types.h"
#include "TiledFloor.h"
#include "Actor.h"
#include "TurtleActor.h"
#include "Scene.h"

#include <vector>
#include <array>

namespace Turtle
{

	//Represents and handles the simulated world
	//This includes both the simulation and graphical data
	class World
	{
	public:
		World();

		void resize(const Index2D & size, const Position2D & tileSize = {1,1});
		void reset();

		//Execute the simulated world step(s)
		//Returns whether something in the world changed
		bool operator()(int steps = 1);

		//Access functors
		TiledFloor & floor() {return m_floor;}
		TurtleActor & mainActor() { return m_mainActor;}
		Scene & scene() {return m_scene;}

		//Clamp the position to the bounding box
		Position clamp(const Position & position, const Position & margin = {});

		//Calculate the final position, inside the bounding box, if moving
		// in a straight line
		Position edge(const Position & from, const Position & to, const Position & margin = {});

	private:
		//The world bounding box
		std::array<Position, 2> boundingBox;

		//Count of simulated steps per simulated second
		double stepsPerSecond = 25.0;

		//The floor of the world
		TiledFloor m_floor;

		//The main actor of the world
		TurtleActor m_mainActor;

		//The 3D scene
		Scene m_scene;

		//The actors
		std::vector<std::reference_wrapper<Actor>> m_actors;

	};

}
#endif // WORLD_H
