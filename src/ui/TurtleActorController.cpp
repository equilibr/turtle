#include "TurtleActorController.h"

TurtleActorController::TurtleActorController(
		TurtleActor &actor,
		QObject *parent) :
	QObject(parent),
	actor{actor},
	pause{false}
{
	//Add our callback dispatcher to the actors' callbacks list
	actor.callbacks().push_back([this](TurtleActor::CallbackType type){callback(type);} );
}

void TurtleActorController::setSingleStep(bool enable)
{
	pause = enable;
	actor.pause(pause);
}

void TurtleActorController::continueSingleStep()
{
	actor.unpause();
}

void TurtleActorController::setTargetPosition(Position2D target) const
{
	TurtleActor::Location location = actor.state().target;
	location.position = target;
	actor.setTarget(location);
}

void TurtleActorController::setTargetPositionX(Position2D::value_type target) const
{
	TurtleActor::Location location = actor.state().target;
	location.position.x() = target;
	actor.setTarget(location);
}

void TurtleActorController::setTargetPositionY(Position2D::value_type target) const
{
	TurtleActor::Location location = actor.state().target;
	location.position.y() = target;
	actor.setTarget(location);
}

void TurtleActorController::getCurrentState()
{
	emit newCurrentState(actor.state());
}

void TurtleActorController::setTargetAngle(double target) const
{
	TurtleActor::Location location = actor.state().target;
	location.angle = target;
	actor.setTarget(location);
}

void TurtleActorController::setPenColor(QColor color) const
{
	TurtleActor::Pen pen = actor.state().pen;
	pen.color = color;
	actor.setPen(pen);
}

void TurtleActorController::setPenDown(bool down) const
{
	TurtleActor::Pen pen = actor.state().pen;
	pen.down = down;
	actor.setPen(pen);
}

void TurtleActorController::setMove(Position2D::value_type distance) const
{
	actor.move(distance);
}

void TurtleActorController::setRotate(const double angle) const
{
	actor.rotate(angle);
}

void TurtleActorController::setTile(const QColor color, const TilePosition2D offset, bool absolute)
{
	actor.setTile(color, offset, absolute);
}

void TurtleActorController::getTile(const TilePosition2D offset, bool absolute)
{
	emit newTile(actor.getTile(offset, absolute));
}

void TurtleActorController::getTileSensor()
{
	emit newTileSensor(actor.tileSensor());
}

void TurtleActorController::command(const Command & data)
{
	Command reply = data;
	reply.reply = true;
	reply.valid = false;

	emit commandReply(reply);
}

void TurtleActorController::callback(TurtleActor::CallbackType type)
{
	switch (type)
	{
		case TurtleActor::CallbackType::Reset:
			//After a reset the actor is active
			emit newRunState(true);

			//Restore the remembered pause mode
			actor.pause(pause);

			emit newState(actor.state(), TurtleActor::CallbackType::Reset);
			break;

		case TurtleActor::CallbackType::Active:
			emit newRunState(true);
			break;

		case TurtleActor::CallbackType::Paused:
			emit newRunState(false);
			break;

		case TurtleActor::CallbackType::Current:
		case TurtleActor::CallbackType::Target:
		case TurtleActor::CallbackType::Pen:
		case TurtleActor::CallbackType::Move:
		case TurtleActor::CallbackType::Rotate:
			emit newState(actor.state(), type);
			break;
	}
}
