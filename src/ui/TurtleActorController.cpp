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

void TurtleActorController::command(const Command & data)
{
	if (!data.valid)
		return;

	if (data.reply)
		return;

	emit signalCommand(data);

	commandData = data;
	commandData.reply = true;
	commandData.valid = false;

	switch (commandData.destination)
	{
		case Command::Destination::UI:
			commandUI(commandData);
			emit commandReply(commandData);
			break;

		case Command::Destination::Turtle:
			const bool ok = actor.command(commandData);
			commandData.reply = true;
			if (!ok || (commandData.data.turtle.command == Command::Turtle::Command::Get))
				emit commandReply(commandData);
			else
				//Set as valid since the command was accepted an it will
				// be successfully completed
				commandData.valid = true;
			break;
	}
}

void TurtleActorController::commandUI(Command & data)
{
	switch (data.data.ui.command)
	{
		case Command::UI::Command::Log:
			emit log(
						data.data.ui.text,
						data.data.ui.title,
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
	//Get the current state
	Command stateCommand {};
	stateCommand.valid = true;
	stateCommand.destination = Command::Destination::Turtle;
	stateCommand.data.turtle.command = Command::Turtle::Command::Get;
	stateCommand.data.turtle.target = Command::Turtle::Target::Current;
	actor.commandGet(stateCommand);
	stateCommand.reply = true;

	switch (type)
	{
		case TurtleActor::CallbackType::Reset:
			//After a reset the actor is active
			emit newRunState(true);

			//Restore the remembered pause mode
			actor.pause(pause);

			emit newCurrentState(stateCommand);

			//Make sure any waiters are unblocked
			emit commandReply(stateCommand);
			break;

		case TurtleActor::CallbackType::Active:
			emit newRunState(true);
			emit commandReply(commandData);
			break;

		case TurtleActor::CallbackType::Paused:
			emit newRunState(false);
			break;

		case TurtleActor::CallbackType::Current:
			emit newCurrentState(stateCommand);
			break;
	}
}
