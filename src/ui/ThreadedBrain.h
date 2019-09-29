#ifndef THREADEDBRAIN_H
#define THREADEDBRAIN_H

#include <QAtomicInt>
#include <QObject>
#include <QEventLoop>
#include <QString>
#include <QColor>
#include <QVariant>
#include <QPointer>

#include "Types.h"
#include "TileSensor.h"
#include "TurtleActor.h"

//This object always lives in a separate thread
class ThreadedBrain : public QObject
{
	Q_OBJECT

public:
	explicit ThreadedBrain(QPointer<QObject> controller, QObject * parent = nullptr) :
		QObject(parent),
		controller{controller}
	{}

	operator bool() const { return active.load() != 0; }
	void setActive(bool value) { active.store(value ? 1 : 0);}

	int getInteger(QString title = {}, QString label = {}, int input = 0, bool * ok = nullptr);
	double getDouble(QString title = {}, QString label = {}, double input = 0, bool * ok = nullptr);
	QString getString(QString title = {}, QString label = {}, QString input = {}, bool * ok = nullptr);

	//Send a log string to the UI
	void log(QString text);

	//Directly set the target position
	void setTargetPosition(Turtle::Position2D target);

	//Directly set the target angle
	void setTargetAngle(double target);

	//Get the current state
	Turtle::TurtleActor::State getCurrentState();

	//Set the pen color
	void setPenColor(QColor color = Qt::black);
	void setPenColor(double red, double green, double blue);
	void setPenColor(double hue, double saturation);

	//Set the pen state
	void setPenDown(bool down = true);

	//Move on the current heading(angle)
	void move(double distance = 1.0);

	//Rotate by some angle
	void rotate(double angle = 0.5);
	void turnLeft() {rotate(0.25);}
	void turnRight() {rotate(-0.25);}

	void setDirectionalTile(const QColor color, const Turtle::TilePosition2D offset);
	QColor getDirectionalTile(const Turtle::TilePosition2D offset);

	void setAbsoluteTile(const QColor color, const Turtle::TilePosition2D offset);
	QColor getAbsoluteTile(const Turtle::TilePosition2D offset);

	void setTile(const QColor color, const Turtle::TilePosition2D offset, bool absolute = false);
	QColor getTile(const Turtle::TilePosition2D offset, bool absolute = false);

	//Get the tile sensor
	Turtle::TileSensor tileSensor();


signals:
	void started();
	void stopped();
	void signalLog(QString text);

	void signalGetCurrentState();
	void signalTargetPosition(Turtle::Position2D target);
	void signalTargetAngle(double target);
	void signalPenColor(QColor color);
	void signalPenDown(bool down);
	void signalMove(Turtle::Position2D::value_type distance);
	void signalRotate(const double angle) const;

	void signalSetTile(const QColor color, const Turtle::TilePosition2D offset, bool absolute = false);
	void signalGetTile(const Turtle::TilePosition2D offset, bool absolute = false);

	void signalGetTileSensor();


public slots:
	void start();
	void stop();

	void newRunState(bool active);
	void newCurrentState(Turtle::TurtleActor::State state);
	void newTile(QColor color);
	void newTileSensor(Turtle::TileSensor sensor);

private slots:
	void run();

private:
	void waitForActive();
	void stopWaitingForActive();
	QVariant requestData(QString title, QString label, QVariant input, bool * ok);
	QVariant requestDataWorker(QString title, QString label, QVariant input, bool * ok);

	QPointer<QObject> controller;
	Turtle::TurtleActor::State m_state;
	Turtle::TileSensor m_tileSensor;
	QColor m_tile;

	QEventLoop idleEventLoop;

	//Set to 0 when the execution should stop
	QAtomicInt active;
};

#endif // THREADEDBRAIN_H
