#ifndef THREADEDBRAINCONTROLLER_H
#define THREADEDBRAINCONTROLLER_H

#include <QObject>
#include <QThread>
#include <QWaitCondition>
#include <QMutex>

#include "TurtleActor.h"
#include "ThreadedBrain.h"

//Interface between the ThreadedBrain and the rest of the system
//This class allows to implement blocking calls from within the brain
// while not blocking anyone else.
class ThreadedBrainController : public QObject
{
	Q_OBJECT
public:
	explicit ThreadedBrainController(
			Turtle::TurtleActor & actor,
			QObject *parent = nullptr);

	~ThreadedBrainController();

	//This should be called from the thread context that holds this object
	//It will unlock the managed thread that is waiting inside wait()
	void unlock();

	//This should be called from the brainThread context
	//It will wait until unlock() is called from the managing thread
	void wait();

	//This should be called from the brainThread context
	//It will process all pending events in the brainThread context
	void process();

public slots:

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


private:
	QThread brainThread;
	ThreadedBrain * brain;

	QWaitCondition operating;
	QMutex mutex;
};

#endif // THREADEDBRAINCONTROLLER_H
