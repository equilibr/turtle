#ifndef THREADEDBRAINCONTROLLER_H
#define THREADEDBRAINCONTROLLER_H

#include <QPointer>
#include <QObject>
#include <QThread>
#include <QWaitCondition>
#include <QMutex>
#include <QVariant>

#include "TurtleActorController.h"
#include "ThreadedBrain.h"

//Interface between the ThreadedBrain and the rest of the system
//This class allows to implement blocking calls from within the brain
// while not blocking anyone else.
class ThreadedBrainController : public QObject
{
	Q_OBJECT
public:
	explicit ThreadedBrainController(
			TurtleActorController * controller,
			QObject *parent = nullptr);

	~ThreadedBrainController();

signals:
	void started();
	void stopped();
	void log(QString text);

public slots:
	void start();
	void stop();

private:
	QThread brainThread;
	QPointer<ThreadedBrain> brain;
};

#endif // THREADEDBRAINCONTROLLER_H
