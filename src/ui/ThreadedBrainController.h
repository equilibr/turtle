#ifndef THREADEDBRAINCONTROLLER_H
#define THREADEDBRAINCONTROLLER_H

#include <QObject>
#include <QThread>
#include <QWaitCondition>
#include <QMutex>


#include "RobotController.h"
#include "ThreadedBrain.h"

class ThreadedBrainController : public QObject
{
	Q_OBJECT
public:
	explicit ThreadedBrainController(
			RobotController * robot,
			QObject *parent = nullptr);

	~ThreadedBrainController();

signals:
	void run();
	void stop();

	//From the brain
	void started();
	void stopped();

	void setPenDown(bool down);
	void setPenColor(double r, double g, double b);
	void move(double distance);
	void rotate(double angle);
	void log(QString text);

	void getInteger(int input, QString title, QString label);
	void getDouble(double input, QString title, QString label);
	void getString(QString input, QString title, QString label);

private slots:
	void ideling();

private:
	void wait();

	QThread brainThread;
	ThreadedBrain * brain;

	QWaitCondition operating;
	QMutex mutex;
};

#endif // THREADEDBRAINCONTROLLER_H
