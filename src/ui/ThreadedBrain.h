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

	//Set the pen color
	void setPenColor(QColor color);
	void setPenColor(double red, double green, double blue);
	void setPenColor(double hue, double saturation);

	//Set the pen state
	void setPenDown(bool down);

	//Move on the current heading(angle)
	void move(double distance = 1.0);

	//Rotate by some angle
	void rotate(double angle = 0.5);
	void turnLeft() {rotate(0.25);}
	void turnRight() {rotate(-0.25);}


signals:
	void started();
	void stopped();
	void signalLog(QString text);

	void signalTargetPosition(Turtle::Position2D target);
	void signalTargetAngle(double target);
	void signalPenColor(QColor color);
	void signalPenDown(bool down);
	void signalMove(Turtle::Position2D::value_type distance);
	void signalRotate(const double angle) const;


public slots:
	void start();
	void stop();

	void newRunState(bool active);

private slots:
	void run();

private:
	void waitForActive();
	void stopWaitingForActive();
	QVariant requestData(QString title, QString label, QVariant input, bool * ok);
	QVariant requestDataWorker(QString title, QString label, QVariant input, bool * ok);


	QPointer<QObject> controller;

	QEventLoop idleEventLoop;

	//Set to 0 when the execution should stop
	QAtomicInt active;
};

#endif // THREADEDBRAIN_H
