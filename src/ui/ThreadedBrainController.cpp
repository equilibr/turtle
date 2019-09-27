#include "ThreadedBrainController.h"

ThreadedBrainController::ThreadedBrainController(
		TurtleActorController * controller,
		QObject *parent) :
	QObject{parent},
	brain{new ThreadedBrain{this}}
{
	brain->moveToThread(&brainThread);
	connect(&brainThread, &QThread::finished, brain, &QObject::deleteLater);

	qRegisterMetaType<Turtle::Position2D>("Turtle::Position2D");
	qRegisterMetaType<Turtle::Position2D::value_type>("Turtle::Position2D::value_type");

	connect(brain, &ThreadedBrain::started, this, &ThreadedBrainController::started);
	connect(brain, &ThreadedBrain::stopped, this, &ThreadedBrainController::stopped);
	connect(brain, &ThreadedBrain::signalLog, this, &ThreadedBrainController::log);

	connect(brain, &ThreadedBrain::signalTargetPosition, controller, &TurtleActorController::setTargetPosition);
	connect(brain, &ThreadedBrain::signalTargetAngle, controller, &TurtleActorController::setTargetAngle);
	connect(brain, &ThreadedBrain::signalPenColor, controller, &TurtleActorController::setPenColor);
	connect(brain, &ThreadedBrain::signalPenDown, controller, &TurtleActorController::setPenDown);
	connect(brain, &ThreadedBrain::signalMove, controller, &TurtleActorController::setMove);
	connect(brain, &ThreadedBrain::signalRotate, controller, &TurtleActorController::setRotate);

	connect(controller, &TurtleActorController::newRunState, brain, &ThreadedBrain::newRunState);

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
