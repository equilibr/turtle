#include "TurtleActor.h"

#include <cmath>

using namespace Turtle;

const double TurtleActor::pi = 	acos(-1);

TurtleActor::TurtleActor(TiledFloor &floor) :
	Actor("Turtle"),
	m_floor(floor)
{
	m_root = new osg::MatrixTransform;
	m_root->addChild(m_robot.root());

	reset();
}

bool TurtleActor::operator()(int steps)
{
	//Hat color cycle
	//---------------

	float alpha = static_cast<float>(sin(pi * m_internalState.colorCycle));
	m_internalState.colorCycle = fmod(m_internalState.colorCycle + steps * 1e-1 / m_internalState.cycleSpeed, 1);
	m_robot.setHatColor({0,1-alpha,alpha,1});

	if (m_internalState.idle)
		return false;

	m_internalState.idle = true;
	stepPosition(steps);
	stepAngle(steps);
	stepPen();


	//Notification and action
	//-----------------------

	m_root->setMatrix(
				osg::Matrix::rotate(2*pi*m_state.current.angle,0,0,1) *
				osg::Matrix::translate(static_cast<osg::Vec3>(m_state.current.position))
			);

	callbackCurrent(m_internalState.idle);

	return true;
}

void TurtleActor::reset()
{
	m_state = {};

	m_internalState.lastPosition = m_state.current.position;

	m_internalState.linearSpeed = 0.1;
	m_internalState.rotationSpeed = 0.1;
	m_internalState.colorCycle = 25.0;

	m_internalState.colorCycle = 0;
	m_internalState.active = true;
	m_internalState.idle = true;

	callbackTarget(true);
	callbackCurrent(true);
	callbackPen(true);
}

double TurtleActor::normalizeAngle(double angle)
{
	return fmod(angle + 1.5, 1.0) - 0.5;
}

void TurtleActor::stepPosition(int steps)
{
	//Calculate distance to set point
	osg::Vec3 delta = static_cast<osg::Vec3>(m_state.target.position - m_state.current.position);
	double distance = static_cast<double>(delta.length());

	if (distance <= (m_internalState.linearSpeed * steps))
		//The step is small enough to directly set it
		m_state.current.position = m_state.target.position;
	else
	{
		osg::Vec3 unit = delta;
		unit.normalize();
		m_state.current.position += Position2D{unit} * m_internalState.linearSpeed * steps;
		m_internalState.idle = false;
	}
}

void TurtleActor::stepAngle(int steps)
{
	//Calculate the shortest path
	double rDistance = normalizeAngle(m_state.target.angle - m_state.current.angle);
	if (fabs(rDistance) >= 1.0)
		rDistance = 0;
	else if (fabs(rDistance) > 0.5)
		//Go the other way
		rDistance = -rDistance;

	if (fabs(rDistance) <= (m_internalState.rotationSpeed * steps))
		m_state.current.angle = m_state.target.angle;
	else
	{
		m_state.current.angle =
				normalizeAngle(
					m_state.current.angle +
					rDistance  / fabs(rDistance)
					* m_internalState.rotationSpeed * steps);
		m_internalState.idle = false;
	}
}

void TurtleActor::stepPen()
{
	if (m_internalState.dirty)
	{
		m_robot.setTopColor(fromQColor(m_state.pen.color));
		m_robot.setPenColor(fromQColor(m_state.pen.color));
		m_robot.setPenState(m_state.pen.down);
	}

	if (m_state.pen.down && (m_internalState.dirty || (m_state.current.position != m_internalState.lastPosition)))
	{
		m_internalState.lastPosition = m_state.current.position;
		m_floor.setColor(m_state.current.position, m_state.pen.color);
	}

	if (m_internalState.dirty)
	{
		m_internalState.dirty = false;
		callbackPen(true);
	}
}
