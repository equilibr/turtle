#include "TurtleActorController.h"

#include <QInputDialog>

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
	commandData = data;
	commandData.reply = true;
	commandData.valid = false;

	switch (data.destination)
	{
		case Command::Destination::UI:
			commandUI(commandData);
			emit commandReply(commandData);
			break;

		case Command::Destination::Turtle:
			const bool ok = actor.command(commandData);
			if (!ok || (data.data.turtle.command == Command::Turtle::Command::Get))
				emit commandReply(commandData);
			break;
	}
}

void TurtleActorController::commandUI(Command & data)
{
	switch (data.data.ui.command)
	{
		case Command::UI::Command::Log:
			emit log(
						data.data.ui.title,
						data.data.ui.text,
						data.data.ui.level);

			data.valid = true;
			return;

		case Command::UI::Command::GetInt:
			data.data.ui.integer =
					QInputDialog::getInt(
						nullptr,
						data.data.ui.title,
						data.data.ui.text,
						data.data.ui.integer,
						static_cast<int>(data.data.ui.min),
						static_cast<int>(data.data.ui.max),
						static_cast<int>(data.data.ui.step),
						&data.valid);
			return;

		case Command::UI::Command::GetDouble:
			data.data.ui.real =
					QInputDialog::getDouble(
						nullptr,
						data.data.ui.title,
						data.data.ui.text,
						data.data.ui.real,
						data.data.ui.min,
						data.data.ui.max,
						static_cast<int>(data.data.ui.step),
						&data.valid);
			return;

		case Command::UI::Command::GetString:
			data.data.ui.string =
					QInputDialog::getText(
						nullptr,
						data.data.ui.title,
						data.data.ui.text,
						QLineEdit::Normal,
						data.data.ui.string,
						&data.valid);
			return;
	}
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
			emit commandReply(commandData);
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
