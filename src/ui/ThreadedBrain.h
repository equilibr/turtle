#ifndef THREADEDBRAIN_H
#define THREADEDBRAIN_H

#include <QAtomicInt>
#include <QObject>
#include <QEventLoop>
#include <QString>
#include <QColor>

#include "Types.h"
#include "Command.h"
#include "TileSensor.h"
#include "TurtleActor.h"

//This object always lives in a separate thread
class ThreadedBrain : public QObject
{
	Q_OBJECT

public:
	explicit ThreadedBrain(QObject * parent = nullptr) : QObject(parent) {}

	explicit operator bool() const { return active.load() != 0; }
	void setActive(bool value) { active.store(value ? 1 : 0);}

	int getInteger(QString title = {}, QString label = {}, int input = 0, bool * ok = nullptr);
	double getDouble(QString title = {}, QString label = {}, double input = 0, bool * ok = nullptr);
	QString getString(QString title = {}, QString label = {}, QString input = {}, bool * ok = nullptr);

	//Send a log string to the UI
	void log(QString text);

	//Return the current position
	Turtle::Position2D getCurrentPosition();

	//Directly set the target position
	void setTargetPosition(Turtle::Position2D target, bool jump = false);

	//Directly set the target angle
	void setTargetAngle(double target, bool jump = false);

	//Set the pen color
	void setPenColor(QColor color = Qt::black);

	void setPenColor(double red, double green, double blue)
	{ setPenColor(QColor::fromRgbF(red, green, blue)); }

	void setPenColor(double hue, double saturation)
	{ setPenColor(QColor::fromHsvF(hue, saturation, 1.0)); }

	//Set the pen state
	void setPenDown(bool down = true);

	//Jump by a given distance on the current heading
	void jump(Turtle::Position2D distance = {1,0}, bool jump = true);

	//Move on the current heading(angle)
	void move(double forward = 1.0, double sideways = 0);

	//Rotate by some angle
	void rotate(double angle = 0.5);
	void turnLeft() {rotate(0.25);}
	void turnRight() {rotate(-0.25);}

	void setDirectionalTile(const QColor color, const Turtle::TilePosition2D offset)
	{ setTile(color, offset, false); }

	QColor getDirectionalTile(const Turtle::TilePosition2D offset)
	{ return getTile(offset, false); }

	void setAbsoluteTile(const QColor color, const Turtle::TilePosition2D offset)
	{ setTile(color, offset, true); }

	QColor getAbsoluteTile(const Turtle::TilePosition2D offset)
	{ return getTile(offset, true); }

	void setTile(const QColor color, const Turtle::TilePosition2D offset, bool absolute = false);
	QColor getTile(const Turtle::TilePosition2D offset, bool absolute = false);

	//Get the tile sensor
	Turtle::TileSensor tileSensor();

signals:
	void started();
	void stopped();

	//The signal mathing to sendCommand
	void signalSendCommand(Turtle::Command command);

public slots:
	void start();
	void stop();

	//Send a command to the controller and return the result
	//This function will block until the command is complete
	Turtle::Command sendCommand(const Turtle::Command & command);

	//Should be called when the current command is done and it's data is ready
	void commandReply(const Turtle::Command & data);

private slots:
	void run();

private:
	void waitForActive();
	void stopWaitingForActive();

	Turtle::Command commandData;

	QEventLoop idleEventLoop;

	//Set to 0 when the execution should stop
	QAtomicInt active;
};

#endif // THREADEDBRAIN_H
