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
	qRegisterMetaType<Turtle::TilePosition2D>("Turtle::TilePosition2D");
	qRegisterMetaType<Turtle::TileSensor>("Turtle::TileSensor");
	qRegisterMetaType<Turtle::TurtleActor::State>("Turtle::TurtleActor::State");

	connect(brain, &ThreadedBrain::started, this, &ThreadedBrainController::started);
	connect(brain, &ThreadedBrain::stopped, this, &ThreadedBrainController::stopped);
	connect(brain, &ThreadedBrain::signalLog, this, &ThreadedBrainController::log);

	connect(brain, &ThreadedBrain::signalTargetPosition, controller, &TurtleActorController::setTargetPosition);
	connect(brain, &ThreadedBrain::signalTargetAngle, controller, &TurtleActorController::setTargetAngle);
	connect(brain, &ThreadedBrain::signalPenColor, controller, &TurtleActorController::setPenColor);
	connect(brain, &ThreadedBrain::signalPenDown, controller, &TurtleActorController::setPenDown);
	connect(brain, &ThreadedBrain::signalMove, controller, &TurtleActorController::setMove);
	connect(brain, &ThreadedBrain::signalRotate, controller, &TurtleActorController::setRotate);
	connect(brain, &ThreadedBrain::signalSetTile, controller, &TurtleActorController::setTile);


	connect(brain, &ThreadedBrain::signalGetCurrentState, controller, &TurtleActorController::getCurrentState);
	connect(brain, &ThreadedBrain::signalGetTile, controller, &TurtleActorController::getTile);
	connect(brain, &ThreadedBrain::signalGetTileSensor, controller, &TurtleActorController::getTileSensor);

	connect(controller, &TurtleActorController::newRunState, brain, &ThreadedBrain::newRunState);
	connect(controller, &TurtleActorController::newCurrentState, brain, &ThreadedBrain::newCurrentState);
	connect(controller, &TurtleActorController::newTile, brain, &ThreadedBrain::newTile);
	connect(controller, &TurtleActorController::newTileSensor, brain, &ThreadedBrain::newTileSensor);

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
