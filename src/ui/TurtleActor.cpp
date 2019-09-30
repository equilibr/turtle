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
	stepHat(steps);

	if (!m_internalState.active && m_internalState.unpause)
	{
		m_internalState.unpause = false;
		m_internalState.active = true;
		callback(CallbackType::Active);
	}

	if (!m_internalState.pending || !m_internalState.active)
		return false;

	//Assume all pending actions might finish in this iteration
	m_internalState.pending = false;

	processCommand();

	stepPosition(steps);
	stepAngle(steps);
	updateState();
	stepPen();

	callback(CallbackType::Current);

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

bool TurtleActor::command(Command & data)
{
	switch (data.data.turtle.command)
	{
		case Command::Turtle::Command::Get:
			return commandGet(data);
		case Command::Turtle::Command::Set:
			return commandSet(data);
	}

	return false;
}

bool TurtleActor::commandGet(Command & data)
{
	//Fill data that does not depends on the command
	data.data.turtle.tileSensor = m_internalState.tileSensor;
	data.data.turtle.penDown = m_state.pen.down;

	//Get the current heading


	data.valid = true;
	return true;
}

bool TurtleActor::commandSet(Command & data)
{
	commandData = data;

	//Analyze the command for errors and conditions and set the pending flag acordingly


	m_internalState.pending |= commandData.valid;
	return m_internalState.pending;
}

void TurtleActor::reset()
{
	m_state = {};

	m_internalState.colorCycle = 0;

	m_internalState.lastPenPosition = m_state.current.position;

	m_internalState.penDirty = true;

	m_internalState.pending = false;
	m_internalState.active = true;
	m_internalState.pause = false;
	m_internalState.unpause = false;

	m_tileSensor = QImage(tileSensorSize*2 + 1, tileSensorSize*2 + 1, QImage::Format_ARGB32);

	updateState();
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
	m_internalState.penDirty = true;
}

void TurtleActor::setTile(const QColor color,
		const TilePosition2D offset, bool absolute)
{
	m_state.touchPen.color = color;
	m_state.touchPen.down = true;
	m_state.touchPen.offset = offset;
	m_internalState.touchAbsolute = absolute;

	m_internalState.pending = true;
}

QColor TurtleActor::getTile(const TilePosition2D offset, bool absolute)
{
	TilePosition2D touchTile;
	if (absolute)
		touchTile = offset;
	else
		touchTile = m_state.tilePosition + positionToGlobal(offset);

	return m_world.floor().getColor(touchTile);
}

double TurtleActor::normalizeAngle(double angle)
{
	return fmod(angle + 1.5, 1.0) - 0.5;
}

void TurtleActor::processCommand()
{
	if (!commandData.valid)
		return;

	//World position
	Position2D worldPosition;

	//Tile index
	TilePosition2D tilePosition;

	//Angle
	double angle;

	if (commandData.data.turtle.absolute)
	{
		worldPosition = commandData.data.turtle.position;
		tilePosition = commandData.data.turtle.tile;
		angle = commandData.data.turtle.angle;
	}
	else
	{
		worldPosition = m_state.current.position
				+ positionToGlobal(commandData.data.turtle.position);

		tilePosition = m_state.tilePosition
				+ positionToGlobal(commandData.data.turtle.tile);

		angle =
				normalizeAngle(m_state.current.angle + commandData.data.turtle.angle);
	}







	commandData.valid = false;
}

void TurtleActor::stepHat(int steps)
{
	float alpha = static_cast<float>(sin(pi * m_internalState.colorCycle));
	m_internalState.colorCycle = fmod(m_internalState.colorCycle + steps * 1e-1 / m_internalState.cycleSpeed, 1);
	m_robot.setHatColor({0,1-alpha,alpha,1});
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

	if (m_state.pen.down && (m_internalState.penDirty || (m_state.tilePosition != m_internalState.lastPenPosition)))
	{
		m_internalState.lastPenPosition = m_state.tilePosition;
		m_world.floor().setColor(m_state.tilePosition, m_state.pen.color);
		m_internalState.penDirty = true;
	}

	if (m_state.touchPen.down)
	{
		m_state.touchPen.down = false;

		TilePosition2D touchTile;
		if (m_internalState.touchAbsolute)
			touchTile = m_state.touchPen.offset;
		else
			touchTile =
					m_state.tilePosition +
					positionToGlobal(m_state.touchPen.offset, m_state.heading);

		m_world.floor().setColor(touchTile, m_state.touchPen.color);
		m_internalState.penDirty = true;
	}

	if (m_internalState.penDirty)
	{
		m_internalState.penDirty = false;
		callback(CallbackType::Pen);
	}
}

void TurtleActor::updateState()
{
	m_root->setMatrix(
				osg::Matrix::rotate(2*pi*m_state.current.angle,0,0,1) *
				osg::Matrix::translate(static_cast<osg::Vec3>(m_state.current.position))
				);

	m_state.tilePosition = m_world.floor().toTileIndex(m_state.current.position);
	updateHeading();
	updateTileSensor();
}

void TurtleActor::updateTileSensor()
{
	//Get the tile data
	const auto raw = m_world.floor().getTiles(m_state.tilePosition, tileSensorSize);

	TileSensor::Data data;
	for (int front = -tileSensorSize; front <= tileSensorSize; ++front)
		for (int side = -tileSensorSize; side <= tileSensorSize; ++side)
		{
			const auto color = raw.get(positionToLocal({front, side}));

			data.push_back(color);

			//Note that the Y axis is inverted
			m_tileSensor.setPixelColor(
						tileSensorSize + side,
						m_tileSensor.height() -1 - (tileSensorSize + front),
						color);
		}

	m_internalState.tileSensor = {data, tileSensorSize};
}

void TurtleActor::updateHeading()
{
	//Find the direction
	constexpr double quanta = 1.0 / 8;

	if ((m_state.current.angle >= -quanta) && (m_state.current.angle <= quanta))
		m_state.heading = Heading::PositiveX;
	else if ((m_state.current.angle > quanta) && (m_state.current.angle < 3*quanta))
		m_state.heading = Heading::PositiveY;
	else if ((m_state.current.angle < -quanta) && (m_state.current.angle > -3*quanta))
		m_state.heading = Heading::NegativeY;
	else
		m_state.heading = Heading::NegativeX;
}

TilePosition2D TurtleActor::positionToLocal(const TilePosition2D position) const
{
	const auto front = position.x();
	const auto side = position.y();

	switch (m_state.heading)
	{
		case Heading::PositiveX: return {-side,front};
		case Heading::NegativeX: return {side,-front};
		case Heading::PositiveY: return {front,side};
		case Heading::NegativeY: return {-front,-side};
	}

	return {front, side};
}

TilePosition2D TurtleActor::positionToGlobal(const TilePosition2D position) const
{
	const auto front = position.x();
	const auto side = position.y();

	switch (m_state.heading)
	{
		case Heading::PositiveX: return {front, -side};
		case Heading::NegativeX: return {-front, side};
		case Heading::PositiveY: return {side,front};
		case Heading::NegativeY: return {-side,-front};
	}

	return {front, side};
}

Position2D TurtleActor::positionToGlobal(const Position2D position) const
{
	const Position2D delta
	{
		position.x() * cos(2*pi*m_state.current.angle)
				+ position.y() * sin(2*pi*m_state.current.angle),

		position.x() * sin(2*pi*m_state.current.angle)
				+ position.y() * cos(2*pi*m_state.current.angle)
	};

	return m_state.current.position + delta;
}

void TurtleActor::callback(CallbackType type)
{
	for (auto & callback : m_callbacks)
		if (callback)
			callback(type);
}
