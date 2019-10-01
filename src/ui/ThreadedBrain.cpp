#include "ThreadedBrain.h"

#include <QInputDialog>

#include "main.h"

using namespace Turtle;

int ThreadedBrain::getInteger(QString title, QString label, int input, bool * ok)
{
	Command command {};
	command.valid = true;
	command.destination = Command::Destination::UI;
	command.data.ui.command = Command::UI::Command::GetInt;
	command.data.ui.title = title;
	command.data.ui.text = label;
	command.data.ui.integer = input;
	command.data.ui.min = -2147483647;
	command.data.ui.max = 2147483647;
	command.data.ui.step = 1;

	command = sendCommand(command);
	if (ok) *ok = command.valid;
	return command.data.ui.integer;
}

double ThreadedBrain::getDouble(QString title, QString label, double input, bool * ok)
{
	Command command {};
	command.valid = true;
	command.destination = Command::Destination::UI;
	command.data.ui.command = Command::UI::Command::GetDouble;
	command.data.ui.title = title;
	command.data.ui.text = label;
	command.data.ui.real = input;
	command.data.ui.min = -2147483647;
	command.data.ui.max = 2147483647;
	command.data.ui.step = 0.1;

	command = sendCommand(command);
	if (ok) *ok = command.valid;
	return command.data.ui.real;
}

QString ThreadedBrain::getString(QString title, QString label, QString input, bool * ok)
{
	Command command {};
	command.valid = true;
	command.destination = Command::Destination::UI;
	command.data.ui.command = Command::UI::Command::GetString;
	command.data.ui.title = title;
	command.data.ui.text = label;
	command.data.ui.string = input;

	command = sendCommand(command);
	if (ok) *ok = command.valid;
	return command.data.ui.string;
}

void ThreadedBrain::log(QString text)
{
	Command command {};
	command.valid = true;
	command.destination = Command::Destination::UI;
	command.data.ui.command = Command::UI::Command::Log;
	command.data.ui.text = text;

	sendCommand(command);
}

void ThreadedBrain::setTargetPosition(Turtle::Position2D target)
{
	Command command {};
	command.valid = true;
	command.destination = Command::Destination::Turtle;
	command.data.turtle.command = Command::Turtle::Command::Set;
	command.data.turtle.target = Command::Turtle::Target::Target;
	command.data.turtle.absolute = true;
	command.data.turtle.quantized = false;
	command.data.turtle.setPosition = true;

	command.data.turtle.position = target;

	sendCommand(command);
}

void ThreadedBrain::setTargetAngle(double target)
{
	Command command {};
	command.valid = true;
	command.destination = Command::Destination::Turtle;
	command.data.turtle.command = Command::Turtle::Command::Set;
	command.data.turtle.target = Command::Turtle::Target::Target;
	command.data.turtle.absolute = true;
	command.data.turtle.quantized = false;
	command.data.turtle.setHeading = true;

	command.data.turtle.angle = target;

	sendCommand(command);
}

void ThreadedBrain::setPenColor(QColor color)
{
	Command command {};
	command.valid = true;
	command.destination = Command::Destination::Turtle;
	command.data.turtle.command = Command::Turtle::Command::Set;
	command.data.turtle.target = Command::Turtle::Target::Target;
	command.data.turtle.setPenColor = true;

	command.data.turtle.color = color;

	sendCommand(command);
}

void ThreadedBrain::setPenDown(bool down)
{
	Command command {};
	command.valid = true;
	command.destination = Command::Destination::Turtle;
	command.data.turtle.command = Command::Turtle::Command::Set;
	command.data.turtle.target = Command::Turtle::Target::Target;
	command.data.turtle.setPenState = true;

	command.data.turtle.penDown = down;

	sendCommand(command);
}

void ThreadedBrain::jump(Position2D distance)
{
	Command command {};
	command.valid = true;
	command.destination = Command::Destination::Turtle;
	command.data.turtle.command = Command::Turtle::Command::Set;
	command.data.turtle.target = Command::Turtle::Target::Current;
	command.data.turtle.absolute = false;
	command.data.turtle.quantized = false;
	command.data.turtle.setPosition = true;

	command.data.turtle.position = distance;

	sendCommand(command);
}

void ThreadedBrain::move(double forward, double sideways)
{
	Command command {};
	command.valid = true;
	command.destination = Command::Destination::Turtle;
	command.data.turtle.command = Command::Turtle::Command::Set;
	command.data.turtle.target = Command::Turtle::Target::Target;
	command.data.turtle.absolute = false;
	command.data.turtle.quantized = false;
	command.data.turtle.setPosition = true;

	command.data.turtle.position = {forward, sideways};

	sendCommand(command);
}

void ThreadedBrain::rotate(double angle)
{
	Command command {};
	command.valid = true;
	command.destination = Command::Destination::Turtle;
	command.data.turtle.command = Command::Turtle::Command::Set;
	command.data.turtle.target = Command::Turtle::Target::Target;
	command.data.turtle.absolute = false;
	command.data.turtle.quantized = false;
	command.data.turtle.setHeading = true;

	command.data.turtle.angle = angle;

	sendCommand(command);
}

void ThreadedBrain::setTile(const QColor color, const Turtle::TilePosition2D offset, bool absolute)
{
	Command command {};
	command.valid = true;
	command.destination = Command::Destination::Turtle;
	command.data.turtle.command = Command::Turtle::Command::Set;
	command.data.turtle.target = Command::Turtle::Target::Tile;
	command.data.turtle.absolute = absolute;
	command.data.turtle.quantized = true;
	command.data.turtle.tile = offset;
	command.data.turtle.color = color;

	sendCommand(command);
}

QColor ThreadedBrain::getTile(const Turtle::TilePosition2D offset, bool absolute)
{
	Command command {};
	command.valid = true;
	command.destination = Command::Destination::Turtle;
	command.data.turtle.command = Command::Turtle::Command::Get;
	command.data.turtle.target = Command::Turtle::Target::Tile;
	command.data.turtle.absolute = absolute;
	command.data.turtle.quantized = true;
	command.data.turtle.tile = offset;

	command = sendCommand(command);
	return command.data.turtle.color;
}

Turtle::TileSensor ThreadedBrain::tileSensor()
{
	Command command {};
	command.valid = true;
	command.destination = Command::Destination::Turtle;
	command.data.turtle.command = Command::Turtle::Command::Get;
	command.data.turtle.target = Command::Turtle::Target::Current;

	command = sendCommand(command);
	return command.data.turtle.tileSensor;
}

void ThreadedBrain::start()
{
	setActive(true);
	emit started();

	//Invoke the run slot from the main event loop
	QMetaObject::invokeMethod(this, &ThreadedBrain::run, Qt::QueuedConnection);
}

void ThreadedBrain::stop()
{
	setActive(false);
	QMetaObject::invokeMethod(this, &ThreadedBrain::stopWaitingForActive, Qt::QueuedConnection);
}

Turtle::Command ThreadedBrain::sendCommand(const Turtle::Command & command)
{
	emit signalSendCommand(command);
	waitForActive();
	return commandData;
}

void ThreadedBrain::commandReply(const Turtle::Command & data)
{
	commandData = data;
	QMetaObject::invokeMethod(this, [this]{idleEventLoop.quit();}, Qt::QueuedConnection);
}

void ThreadedBrain::run()
{
	//Main execution function
	try
	{
		Main(*this);
	}
	catch(const std::exception & e)
	{
		log("<font color = \"red\">Exception: " + QString(e.what()) + "</font>");
	}
	catch(...)
	{
		log("<font color = \"red\">Exception</font>");
	}


	setActive(false);
	emit stopped();
}

void ThreadedBrain::waitForActive()
{
	if (*this)
		idleEventLoop.exec();
}

void ThreadedBrain::stopWaitingForActive()
{
	idleEventLoop.quit();
}
