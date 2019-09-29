#include "TurtleActor.h"
#include "World.h"
#include "TiledFloor.h"

#include <cmath>

using namespace Turtle;

const double TurtleActor::pi = 	acos(-1);

TurtleActor::TurtleActor(World &world) :
	Actor("Turtle"),
	m_world{world},
	m_robot{radius},
	m_root{new osg::MatrixTransform}
{
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

	if (!m_internalState.active && m_internalState.unpause)
	{
		m_internalState.unpause = false;
		m_internalState.active = true;
		callback(CallbackType::Active);
	}

	if (!m_internalState.pending || !m_internalState.active)
		return false;

	//Assume all pending actions will finish in this iteration
	m_internalState.pending = false;
	stepPosition(steps);
	stepAngle(steps);
	stepPen();


	//Notification and action
	//-----------------------

	updateTransformMatrix();
	callback(CallbackType::Current);

	//Call this here for it's side effect of setting the image
	stepTileSensor();

	//No more pending actions
	if (!m_internalState.pending)
	{
		//Deactivate if a pause is requested
		m_internalState.active = !m_internalState.pause || m_internalState.unpause;
		m_internalState.unpause = false;

		callback(m_internalState.active ? CallbackType::Active : CallbackType::Paused);
	}

	return true;
}

void TurtleActor::reset()
{
	m_state = {};

	m_internalState.lastPosition = m_state.current.position;

	m_internalState.linearSpeed = 1.0;
	m_internalState.rotationSpeed = 0.05;
	m_internalState.cycleSpeed = 25.0;

	m_internalState.colorCycle = 0;
	m_internalState.penDirty = true;

	m_internalState.pending = false;
	m_internalState.active = true;
	m_internalState.pause = false;
	m_internalState.unpause = false;

	m_tileSensor = QImage(tileSensorSize*2 + 1, tileSensorSize*2 + 1, QImage::Format_ARGB32);

	updateTransformMatrix();
	stepTileSensor();

	callback(CallbackType::Reset);
}

void TurtleActor::setTarget(const Location &target)
{
	const Location final
	{
		m_world.edge(m_state.current.position, target.position, {radius,radius}),
		normalizeAngle(target.angle)
	};

	m_internalState.pending = true;
	m_state.target = final;
	callback(CallbackType::Target);
}

void TurtleActor::move(const Position2D::value_type distance)
{
	m_state.relative.distance = distance;

	Position2D delta
	{
		distance * cos(2*pi*m_state.current.angle),
		distance * sin(2*pi*m_state.current.angle)
	};

	Location target {m_state.current.position + delta, m_state.current.angle};
	setTarget(target);
	callback(CallbackType::Move);
}

void TurtleActor::rotate(const double angle)
{
	m_state.relative.angle = angle;

	Location target {m_state.current.position, m_state.current.angle + angle};
	setTarget(target);
	callback(CallbackType::Rotate);
}

void TurtleActor::setPen(const Pen &pen)
{
	m_internalState.pending = true;
	m_state.pen = pen;
	m_internalState.penDirty |= pen.down;
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
		m_internalState.pending = true;
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
		m_internalState.pending = true;
	}
}

void TurtleActor::stepPen()
{
	if (m_internalState.penDirty)
	{
		m_robot.setTopColor(fromQColor(m_state.pen.color));
		m_robot.setPenColor(fromQColor(m_state.pen.color));
		m_robot.setPenState(m_state.pen.down);
	}

	TilePosition2D currentTile = m_world.floor().toTileIndex(m_state.current.position);

	if (m_state.pen.down && (m_internalState.penDirty || (currentTile != m_internalState.lastPosition)))
	{
		m_internalState.lastPosition = currentTile;
		m_world.floor().setColor(currentTile, m_state.pen.color);
		m_internalState.penDirty = true;
	}

	if (m_internalState.penDirty)
	{
		m_internalState.penDirty = false;
		callback(CallbackType::Pen);
	}
}

void TurtleActor::updateTransformMatrix()
{
	m_root->setMatrix(
				osg::Matrix::rotate(2*pi*m_state.current.angle,0,0,1) *
				osg::Matrix::translate(static_cast<osg::Vec3>(m_state.current.position))
				);

}

void TurtleActor::stepTileSensor()
{
	//Get the tile data
	const auto raw =
			m_world.floor().getTiles(
				m_world.floor().toTileIndex(m_state.current.position),
				tileSensorSize);

	//Axis-centered direction
	enum class Direction
	{
		PositiveX,
		NegativeX,
		PositiveY,
		NegativeY
	} direction;

	//Find the direction
	constexpr double quanta = 1.0 / 8;

	if ((m_state.current.angle >= -quanta) && (m_state.current.angle <= quanta))
		direction = Direction::PositiveX;
	else if ((m_state.current.angle > quanta) && (m_state.current.angle < 3*quanta))
		direction = Direction::PositiveY;
	else if ((m_state.current.angle < -quanta) && (m_state.current.angle > -3*quanta))
		direction = Direction::NegativeY;
	else
		direction = Direction::NegativeX;

	auto get = [&raw, direction] (auto front, auto side)
	{
		switch (direction)
		{
			case Direction::PositiveX: return raw.get(-side,front);
			case Direction::NegativeX: return raw.get(side,-front);
			case Direction::PositiveY: return raw.get(front,side);
			case Direction::NegativeY: return raw.get(-front,-side);
		}

		return QColor();
	};


	TileSensor::Data data;
	for (int front = -tileSensorSize; front <= tileSensorSize; ++front)
		for (int side = -tileSensorSize; side <= tileSensorSize; ++side)
		{
			const QColor color = get(front,side);

			data.push_back(color);

			//Note that the Y axis is inverted
			m_tileSensor.setPixelColor(
						tileSensorSize + side,
						m_tileSensor.height() -1 - (tileSensorSize + front),
						color);
		}

	m_internalState.tileSensor = {data, tileSensorSize};
}

void TurtleActor::callback(CallbackType type)
{
	for (auto & callback : m_callbacks)
		if (callback)
			callback(type);
}
