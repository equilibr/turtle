#include "ThreadedBrainController.h"

ThreadedBrainController::ThreadedBrainController(
		TurtleActorController * controller,
		QObject *parent) :
	QObject{parent},
	brain{new ThreadedBrain{}}
{
	brain->moveToThread(&brainThread);
	connect(&brainThread, &QThread::finished, brain, &QObject::deleteLater);

	qRegisterMetaType<Turtle::Command>("Turtle::Command");

	connect(brain, &ThreadedBrain::started, this, &ThreadedBrainController::started);
	connect(brain, &ThreadedBrain::stopped, this, &ThreadedBrainController::stopped);

	connect(brain, &ThreadedBrain::signalSendCommand, controller, &TurtleActorController::command);
	connect(controller, &TurtleActorController::commandReply, brain, &ThreadedBrain::commandReply);

	brainThread.start();
}

ThreadedBrainController::~ThreadedBrainController()
{
	stop();
	brainThread.quit();
	brainThread.wait();
}

void ThreadedBrainController::start()
{
	QMetaObject::invokeMethod(brain, &ThreadedBrain::start);
}

void ThreadedBrainController::stop()
{
	QMetaObject::invokeMethod(brain, &ThreadedBrain::stop, Qt::DirectConnection);
}
