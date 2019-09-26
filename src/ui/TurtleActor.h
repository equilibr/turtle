#ifndef TURTLEACTOR_H
#define TURTLEACTOR_H

#include <functional>
#include <vector>
#include <QColor>
#include <osg/MatrixTransform>

#include "Types.h"
#include "Actor.h"
#include "Robot.h"

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

			Pen() : color{Qt::white}, down{false} {}
			explicit Pen(const QColor & color, const bool down) : color{color}, down{down} {}
			explicit Pen(const Pen & pen, const QColor & color) : color{color}, down{pen.down} {}
			explicit Pen(const Pen & pen, bool down) : color{pen.color}, down{down} {}

			bool operator==(const Pen & rhs) const { return (down == rhs.down) && (color == rhs.color);}
			bool operator!=(const Pen & rhs) const { return (down != rhs.down) || (color != rhs.color);}
		};

		struct Location
		{
			Position2D position;
			double angle;

			Location() : position{}, angle{} {}
			explicit Location(const Position2D & position, const double angle) : position{position}, angle{angle} {}
			explicit Location(const Location & location, Position2D & position) : position{position}, angle{location.angle} {}
			explicit Location(const Location & location, double angle) : position{location.position}, angle{angle} {}

			bool operator==(const Location & rhs) const { return (position == rhs.position) && qFuzzyCompare(angle, rhs.angle);}
			bool operator!=(const Location & rhs) const { return !(position == rhs.position) || !qFuzzyCompare(angle, rhs.angle);}
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
			Target,
			Pen,
			Move,
			Rotate
		};

		using Callback = std::function<void(CallbackType)>;
		using Callbacks = std::vector<Callback>;

		TurtleActor(World & world);

		bool operator()(int steps = 1) override;


		//Access functors
		const osg::ref_ptr<osg::Node> root() const { return m_root; }
		const osg::ref_ptr<osg::Node> robotRoot() const { return m_robot.root(); }
		const State & state() const { return m_state; }
		Callbacks & callbacks(void) { return m_callbacks; }

		//Enable to stop processing when becoming idle
		void pause(bool pause) { m_internalState.pause = pause; }

		//Continue running when paused
		void go() { m_internalState.active = true; }

		//Reset to initial settings
		void reset();

		//Directly set the target
		void setTarget(const Location & target);

		//Move on the current heading(angle)
		void move(const Position2D::value_type distance);

		//Rotate by some angle
		void rotate(const double angle);

		//Set the current pen state
		void setPen(const Pen & pen);

	protected:
		static const double pi;
		static double normalizeAngle(double angle);

		void stepPosition(int steps);
		void stepAngle(int steps);
		void stepPen();

		struct InternalState
		{
			Position2D lastPosition;

			//Speed, in units/step
			double linearSpeed;
			double rotationSpeed;
			double cycleSpeed;

			double colorCycle;

			bool penDirty;

			//Actions are pending
			bool pending;

			//The actor is active
			bool active;

			//A pause was requested
			bool pause;
		};

		World & m_world;
		Robot m_robot;
		osg::ref_ptr<osg::MatrixTransform> m_root;

		State m_state;
		InternalState m_internalState;

	private:
		void callback(CallbackType type);
		Callbacks m_callbacks;
	};

}
#endif // TURTLEACTOR_H
