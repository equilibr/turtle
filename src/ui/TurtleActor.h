#ifndef TURTLEACTOR_H
#define TURTLEACTOR_H

#include <functional>
#include <vector>
#include <QColor>
#include <QImage>
#include <osg/MatrixTransform>

#include "Types.h"
#include "Actor.h"
#include "Robot.h"
#include "TileSensor.h"
#include "Command.h"

namespace Turtle
{
	class World;

	class TurtleActor : public Actor
	{
	public:
		struct Pen
		{
			QColor color;
			bool down;
		};

		struct Location
		{
			Position2D position;
			TilePosition2D tile;

			double angle;
			Heading heading;
		};

		struct Relative
		{
			Position2D::value_type distance;
			double angle;
		};

		struct State
		{
			Pen pen;
			Location current;
			Location target;

			Relative relative;
		};

		enum class CallbackType
		{
			Reset,
			Active,
			Paused,
			Current,
		};

		static constexpr double radius = 0.5;
		static constexpr int tileSensorSize = 3;

		using Callback = std::function<void(CallbackType)>;
		using Callbacks = std::vector<Callback>;

		TurtleActor(World & world);

		bool operator()(int steps = 1) override;

		//Try to execute the command
		//Will return false if the command will not be executed
		bool command(Command & data);
		bool commandGet(Command & data) const;
		void commandSet(const Command & data);


		//Access functors
		//---------------
		const osg::ref_ptr<osg::Node> root() const { return m_root; }
		const osg::ref_ptr<osg::Node> robotRoot() const { return m_robot.root(); }
		const QImage & tileSensorImage() const { return m_tileSensor; }
		Callbacks & callbacks(void) { return m_callbacks; }
		double & linearSpeed(void) { return m_internalState.linearSpeed; }
		double & rotationSpeed(void) { return m_internalState.rotationSpeed; }


		//Control functions
		//-----------------

		//Enable to stop processing when becoming idle
		void pause(bool pause) { m_internalState.pause = pause; }

		//Continue running when paused
		void unpause() { m_internalState.unpause = true; }

		//Reset to initial settings
		void reset();

	protected:
		static const double pi;
		static double normalizeAngle(double angle);

		void processSetCommand();

		void stepHat(int steps);
		void stepPosition(int steps);
		void stepAngle(int steps);
		void stepPen();
		void updateState();
		void updateTileSensor();

		struct InternalState
		{
			//Speed, in units/step
			double linearSpeed = 1.0;
			double rotationSpeed = 0.05;
			double cycleSpeed = 25.0;

			double colorCycle;

			//The last position where the pen drawed
			TilePosition2D lastPenPosition;

			//The pen is dirty and should be updated
			bool penDirty;

			//Actions are pending
			bool pending;

			//The actor is active
			bool active;

			//A pause was requested
			bool pause;

			//A pause cancelation was requested
			bool unpause;

			TileSensor tileSensor;
		};

		World & m_world;
		Robot m_robot;
		osg::ref_ptr<osg::MatrixTransform> m_root;

		State m_state;
		InternalState m_internalState;

		QImage m_tileSensor;

		Command commandData;

	private:

		void updateHeading();

		void updateCommandPosition(Command::Turtle & data) const;

		//Given a position in global coordinates transform it to a position in local coordinates
		TilePosition2D positionToLocal(TilePosition2D position) const;

		//Given a position in local coordinates transform it to a position in global coordinates
		TilePosition2D positionToGlobal(const TilePosition2D position) const;
		Position2D positionToGlobal(const Position2D position) const;

		void callback(CallbackType type);
		Callbacks m_callbacks;
	};

}
#endif // TURTLEACTOR_H
