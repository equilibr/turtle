#ifndef TURTLEACTOR_H
#define TURTLEACTOR_H

#include <QColor>
#include <osg/MatrixTransform>
#include <functional>

#include "Types.h"
#include "Actor.h"
#include "Robot.h"
#include "TiledFloor.h"

namespace Turtle
{

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

		struct State
		{
			Pen pen;
			Location current;
			Location target;

			//Set when current equals target
			bool idle;
		};

		using Callback = std::function<void(bool)>;

		TurtleActor(TiledFloor & floor);

		bool operator()(int steps = 1) override;


		//Access functors
		const osg::ref_ptr<osg::Node> root() const { return m_root; }
		const State & state() const { return m_state; }

		void setTargetCallback(Callback callback) { m_targetCallback = callback; }
		void setCurrentCallback(Callback callback) { m_currentCallback = callback; }
		void setPenCallback(Callback callback) { m_penCallback = callback; }


		//Reset to initial settings
		void reset();

		void setTarget(const Location & target)
		{
			if (m_state.target == target)
				return;

			m_state.idle = false;
			m_state.target = target;
			callbackTarget(false);
		}

		void setPen(const Pen & pen)
		{
			if (m_state.pen == pen)
				return;

			m_state.idle = false;
			m_state.pen = pen;
			m_internalState.dirty = true;
			callbackPen(false);
		}


	protected:
		static const double pi;
		static double normalizeAngle(double angle);

		void callbackTarget(bool idle) { if (m_targetCallback) m_targetCallback(idle);}
		void callbackCurrent(bool idle) { if (m_currentCallback) m_currentCallback(idle);}
		void callbackPen(bool idle) { if (m_penCallback) m_penCallback(idle);}

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

			bool active;
			bool idle;
			bool dirty;
		};

		TiledFloor & m_floor;
		Robot m_robot;
		osg::ref_ptr<osg::MatrixTransform> m_root;

		State m_state;
		InternalState m_internalState;

		Callback m_targetCallback;
		Callback m_currentCallback;
		Callback m_penCallback;
	};

}
#endif // TURTLEACTOR_H
