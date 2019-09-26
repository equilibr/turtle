#include "ThreadedBrainController.h"

#include <QAbstractEventDispatcher>

ThreadedBrainController::ThreadedBrainController(
		Turtle::TurtleActor &actor,
		QObject *parent) :
	QObject{parent},
	brain{new ThreadedBrain(this)}
{
	brain->moveToThread(&brainThread);

//	QMetaObject::invokeMethod()

	//The following code is executed in the brain thread context
	//----------------------------------------------------------

	connect(brain, &ThreadedBrain::signalSetPenDown, brain, [this](bool down)
	{
		emit setPenDown(down);
		brainThread.eventDispatcher()->processEvents(QEventLoop::AllEvents);
		wait();
	},
	Qt::DirectConnection);

	connect(brain, &ThreadedBrain::signalSetPenColor, brain, [this](double r, double g, double b)
	{
		emit setPenColor(r,g,b);
		brainThread.eventDispatcher()->processEvents(QEventLoop::AllEvents);
		wait();
	},
	Qt::DirectConnection);

	connect(brain, &ThreadedBrain::signalMove, brain, [this](double distance)
	{
		emit move(distance);
		brainThread.eventDispatcher()->processEvents(QEventLoop::AllEvents);
		wait();
	},
	Qt::DirectConnection);

	connect(brain, &ThreadedBrain::signalRotate, brain, [this](double angle)
	{
		emit rotate(angle);
		brainThread.eventDispatcher()->processEvents(QEventLoop::AllEvents);
		wait();
	},
	Qt::DirectConnection);

	connect(brain, &ThreadedBrain::signalLog, brain, [this](QString text)
	{
		emit log(text);
		brainThread.eventDispatcher()->processEvents(QEventLoop::AllEvents);
		wait();
	},
	Qt::DirectConnection);


	connect(brain, &ThreadedBrain::signalGetInteger, brain, [this](int input, QString title, QString label)
	{
		emit getInteger(input, title, label);
		brainThread.eventDispatcher()->processEvents(QEventLoop::AllEvents);
		wait();
	},
	Qt::DirectConnection);

	connect(brain, &ThreadedBrain::signalGetDouble, brain, [this](double input, QString title, QString label)
	{
		emit getDouble(input, title, label);
		brainThread.eventDispatcher()->processEvents(QEventLoop::AllEvents);
		wait();
	},
	Qt::DirectConnection);

	connect(brain, &ThreadedBrain::signalGetString, brain, [this](QString input, QString title, QString label)
	{
		emit getString(input, title, label);
		brainThread.eventDispatcher()->processEvents(QEventLoop::AllEvents);
		wait();
	},
	Qt::DirectConnection);



	connect(brain, &ThreadedBrain::started, brain, [this]()
	{
		emit started();
		brainThread.eventDispatcher()->processEvents(QEventLoop::AllEvents);
	},
	Qt::DirectConnection);

	connect(brain, &ThreadedBrain::stopped, brain, [this]()
	{
		emit stopped();
		brainThread.eventDispatcher()->processEvents(QEventLoop::AllEvents);
	},
	Qt::DirectConnection);


	//The following code is executed in our thread context
	//----------------------------------------------------

	//Connect our signals to the robot
	connect(this, &ThreadedBrainController::setPenDown, actor, &RobotController::setPenDown);
	connect(this, &ThreadedBrainController::setPenColor, actor, &RobotController::setPenColor);
	connect(this, &ThreadedBrainController::move, actor, &RobotController::move);
	connect(this, &ThreadedBrainController::rotate, actor, &RobotController::rotate);
	connect(this, &ThreadedBrainController::log, actor, &RobotController::log);

	connect(this, &ThreadedBrainController::getInteger, actor, &RobotController::getInteger);
	connect(this, &ThreadedBrainController::getDouble, actor, &RobotController::getDouble);
	connect(this, &ThreadedBrainController::getString, actor, &RobotController::getString);

	connect(this, &ThreadedBrainController::run, brain, &ThreadedBrain::run, Qt::QueuedConnection);
	connect(this, &ThreadedBrainController::stop, brain, &ThreadedBrain::stop, Qt::DirectConnection);

	//Connect the robot input signal to the brain
	connect(actor, &RobotController::setInputInteger, brain, &ThreadedBrain::setInputInteger, Qt::DirectConnection);
	connect(actor, &RobotController::setInputDouble, brain, &ThreadedBrain::setInputDouble, Qt::DirectConnection);
	connect(actor, &RobotController::setInputString, brain, &ThreadedBrain::setInputString, Qt::DirectConnection);

	//Connect the robot ideling to release our blocked brain
	connect(actor, &RobotController::ideling, this, &ThreadedBrainController::ideling, Qt::DirectConnection);


	connect(&brainThread, &QThread::finished, brain, &QObject::deleteLater);
	brainThread.start();
}

ThreadedBrainController::~ThreadedBrainController()
{
	brainThread.quit();
	brainThread.wait();
}

void ThreadedBrainController::unlock()
{
	mutex.lock();
	operating.wakeOne();
	mutex.unlock();
}

void ThreadedBrainController::wait()
{
	process();
	mutex.lock();
	operating.wait(&mutex);
	mutex.unlock();
	process();
}

void ThreadedBrainController::process()
{
	brainThread.eventDispatcher()->processEvents(QEventLoop::AllEvents);
}
