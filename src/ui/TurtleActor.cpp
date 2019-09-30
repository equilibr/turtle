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

	processSetCommand();

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
			commandSet(data);
	}

	return true;
}

bool TurtleActor::commandGet(Command & data)
{
	commandData = data;
	updateCommandPosition();

	//Fill data that does not depends on the command
	data.data.turtle.tileSensor = m_internalState.tileSensor;
	data.data.turtle.penDown = m_state.pen.down;

	Location & used =
			(commandData.data.turtle.target == Command::Turtle::Target::Current)
			? m_state.current
			: m_state.target;

	data.data.turtle.position = used.position;
	data.data.turtle.tile = used.tile;
	data.data.turtle.angle = used.angle;
	data.data.turtle.heading = used.heading;

	//Get the current heading
	if (commandData.data.turtle.target == Command::Turtle::Target::Tile)
		data.data.turtle.color = m_world.floor().getColor(commandData.data.turtle.tile);
	else
		data.data.turtle.color = m_state.pen.color;

	data.valid = true;
	return true;
}

void TurtleActor::commandSet(Command & data)
{
	commandData = data;
	commandData.valid = true;
	m_internalState.pending = true;
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

void TurtleActor::processSetCommand()
{
	if (!commandData.valid)
		return;

	//The Get commands should never reach us,
	// but we check for this anyway.
	if (commandData.data.turtle.command != Command::Turtle::Command::Set)
		return;

	//The set bits are only valid for the target and current targets
	if (
		(commandData.data.turtle.target != Command::Turtle::Target::Target)
		&&
		(commandData.data.turtle.target != Command::Turtle::Target::Current)
		)
	{
		commandData.data.turtle.setPosition = false;
		commandData.data.turtle.setHeading = false;
		commandData.data.turtle.setPenColor = false;
		commandData.data.turtle.setPenState = false;
	}

	updateCommandPosition();

	if (commandData.data.turtle.setPosition)
	{
		if (commandData.data.turtle.target == Command::Turtle::Target::Current)
			m_state.current.position = commandData.data.turtle.position;

		//The target is always set since "current" assumes the position
		// will stay that way
		m_state.target.position = commandData.data.turtle.position;
		m_state.target.tile = m_world.floor().toTileIndex(m_state.target.position);
	}

	if (commandData.data.turtle.setHeading)
	{
		if (commandData.data.turtle.target == Command::Turtle::Target::Current)
			m_state.current.angle = commandData.data.turtle.angle;

		//The target is always set since "current" assumes the position
		// will stay that way
		m_state.current.angle = commandData.data.turtle.angle;
	}

	if (commandData.data.turtle.setPenColor)
		m_state.pen.color = commandData.data.turtle.color;

	if (commandData.data.turtle.setPenState)
		m_state.pen.down = commandData.data.turtle.penDown;

	if (commandData.data.turtle.target == Command::Turtle::Target::Tile)
		m_world.floor().setColor(
					commandData.data.turtle.tile,
					commandData.data.turtle.color);

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

		//Set this so the next block will invoke the callback
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

	m_state.current.tile = m_world.floor().toTileIndex(m_state.current.position);
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

void TurtleActor::updateCommandPosition()
{
	//This function updates the command position
	// depenending on their settings
	// and performs bound checks and adjustements

	if (commandData.data.turtle.quantized)
	{
		//NOTE: For now the quantized heading is not handled

		if (!commandData.data.turtle.absolute)
			commandData.data.turtle.tile =
					positionToGlobal(commandData.data.turtle.tile);

		commandData.data.turtle.tile =
				m_world.floor().clamp(commandData.data.turtle.tile);

		commandData.data.turtle.position =
				m_world.floor().toPosition(commandData.data.turtle.tile);

		//Sanity check
		commandData.data.turtle.position =
				m_world.clamp(commandData.data.turtle.position, {radius, radius});
	}
	else
	{
		if (!commandData.data.turtle.absolute)
		{
			commandData.data.turtle.position =
					positionToGlobal(commandData.data.turtle.position);

			commandData.data.turtle.angle = m_state.current.angle + commandData.data.turtle.angle;
		}

		//Position bounds check
		if (commandData.data.turtle.target == Command::Turtle::Target::Target)
			commandData.data.turtle.position =
					m_world.edge(
						m_state.current.position,
						commandData.data.turtle.position,
						{radius,radius});
		else
			commandData.data.turtle.position =
					m_world.clamp(
						commandData.data.turtle.position,
						{radius,radius});

		//Update the quantized position
		commandData.data.turtle.tile =
				m_world.floor().toTileIndex(commandData.data.turtle.position);

		//Sanity check
		commandData.data.turtle.tile =
				m_world.floor().clamp(commandData.data.turtle.tile);

		commandData.data.turtle.angle = normalizeAngle(commandData.data.turtle.angle);
	}
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
