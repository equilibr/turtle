#ifndef WORLD_H
#define WORLD_H

#include "Types.h"
#include "TiledFloor.h"
#include "Scene.h"

namespace Turtle
{

	//Represents and handles the simulated world
	//This includes both the simulation and graphical data
	class World
	{
	public:
		World();

		//Execute a simulated world step
		//Returns whether something in the world changed
		bool operator()();

		//Access functors
		Scene & scene() {return m_scene;}
		TiledFloor & floor() {return m_floor;}

	private:
		//The one sided (distance from origin to edge) size of the world
		Position2D halfSize;

		//Count of simulated steps per simulated second
		double stepsPerSecond = 25.0;

		//The floor of the world
		TiledFloor m_floor;

		//The 3D scene
		Scene m_scene;


		//The main turtle robot

	};

}
#endif // WORLD_H
