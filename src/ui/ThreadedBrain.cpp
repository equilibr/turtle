#include "ThreadedBrain.h"

#include <QInputDialog>

#include "main.h"

int ThreadedBrain::getInteger(QString title, QString label, int input, bool * ok)
{
	return requestData(title, label, input, ok).toInt();
}

double ThreadedBrain::getDouble(QString title, QString label, double input, bool * ok)
{
	return requestData(title, label, input, ok).toDouble();
}

QString ThreadedBrain::getString(QString title, QString label, QString input, bool * ok)
{
	return requestData(title, label, input, ok).toString();
}

void ThreadedBrain::log(QString text)
{
	emit signalLog(text);
}

void ThreadedBrain::setTargetPosition(Turtle::Position2D target)
{
	emit signalTargetPosition(target);
	waitForActive();
}

void ThreadedBrain::setTargetAngle(double target)
{
	emit signalTargetAngle(target);
	waitForActive();
}

Turtle::TurtleActor::State ThreadedBrain::getCurrentState()
{
	emit signalGetCurrentState();
	waitForActive();
	return m_state;
}

void ThreadedBrain::setPenColor(QColor color)
{
	emit signalPenColor(color);
	waitForActive();
}

void ThreadedBrain::setPenColor(double red, double green, double blue)
{
	setPenColor(QColor::fromRgbF(red, green, blue));
}

void ThreadedBrain::setPenColor(double hue, double saturation)
{
	setPenColor(QColor::fromHsvF(hue, saturation, 1.0));
}

void ThreadedBrain::setPenDown(bool down)
{
	emit signalPenDown(down);
	waitForActive();
}

void ThreadedBrain::move(double distance)
{
	emit signalMove(distance);
	waitForActive();
}

void ThreadedBrain::rotate(double angle)
{
	emit signalRotate(angle);
	waitForActive();
}

void ThreadedBrain::setDirectionalTile(const QColor color, const Turtle::TilePosition2D offset)
{
	setTile(color, offset, false);
}

QColor ThreadedBrain::getDirectionalTile(const Turtle::TilePosition2D offset)
{
	return getTile(offset, false);
}

void ThreadedBrain::setAbsoluteTile(const QColor color, const Turtle::TilePosition2D offset)
{
	setTile(color, offset, true);
}

QColor ThreadedBrain::getAbsoluteTile(const Turtle::TilePosition2D offset)
{
	return getTile(offset, true);
}

void ThreadedBrain::setTile(const QColor color, const Turtle::TilePosition2D offset, bool absolute)
{
	emit signalSetTile(color, offset, absolute);
	waitForActive();
}

QColor ThreadedBrain::getTile(const Turtle::TilePosition2D offset, bool absolute)
{
	emit signalGetTile(offset, absolute);
	waitForActive();
	return m_tile;
}

Turtle::TileSensor ThreadedBrain::tileSensor()
{
	emit signalGetTileSensor();
	waitForActive();
	return m_tileSensor;
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

void ThreadedBrain::newRunState(bool active)
{
	if (active)
		QMetaObject::invokeMethod(this, &ThreadedBrain::stopWaitingForActive, Qt::QueuedConnection);
}

void ThreadedBrain::newCurrentState(Turtle::TurtleActor::State state)
{
	m_state = state;
	if (active)
		QMetaObject::invokeMethod(this, &ThreadedBrain::stopWaitingForActive, Qt::QueuedConnection);
}

void ThreadedBrain::newTile(QColor color)
{
	m_tile = color;
	if (active)
		QMetaObject::invokeMethod(this, &ThreadedBrain::stopWaitingForActive, Qt::QueuedConnection);
}

void ThreadedBrain::newTileSensor(Turtle::TileSensor sensor)
{
	m_tileSensor = sensor;
	if (active)
		QMetaObject::invokeMethod(this, &ThreadedBrain::stopWaitingForActive, Qt::QueuedConnection);
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

QVariant ThreadedBrain::requestData(QString title, QString label, QVariant input, bool * ok)
{
	auto f = [this, title, label, input, ok]() -> QVariant
	{ return requestDataWorker(title, label, input, ok); };

	QVariant reply;
	QMetaObject::invokeMethod(
				controller,
				f,
				Qt::BlockingQueuedConnection,
				&reply);

	return reply;
}

QVariant ThreadedBrain::requestDataWorker(QString title, QString label, QVariant input, bool * ok)
{
	if (ok)
		*ok = false;

	//Dispatch the correct dialog according to the requested type
	switch (input.type())
	{
		case QVariant::Int:
			return QInputDialog::getInt(
						nullptr,
						title,
						label,
						input.toInt(),
						-2147483647,
						2147483647,
						1,
						ok);

		case QVariant::Double:
			return QInputDialog::getDouble(
						nullptr,
						title,
						label,
						input.toDouble(),
						-2147483647,
						2147483647,
						1,
						ok);

		case QVariant::String:
			return QInputDialog::getText(
						nullptr,
						title,
						label,
						QLineEdit::Normal,
						input.toString(),
						ok);

		default:
			return {};
	}
}
