#ifndef TURTLEACTORCONTROLLER_H
#define TURTLEACTORCONTROLLER_H

#include <QObject>

#include "Command.h"
#include "TurtleActor.h"

using namespace Turtle;

//Handles interaction between the TurtleActor and the UI
class TurtleActorController : public QObject
{
	Q_OBJECT
public:
	explicit TurtleActorController(
			TurtleActor & actor,
			QObject *parent = nullptr);

	double linearSpeed() const { return actor.linearSpeed(); }
	double rotationSpeed() const { return actor.rotationSpeed(); }

signals:
	void log(QString text, QString title, int level);

	//Emitted when the current state is changed
	void newCurrentState(const Command & data);

	//Emitted when the run state is changed
	void newRunState(bool active);

	//Emitted when a new command is requested
	void signalCommand(const Command & data);

	//Emitted when a new command reply is available
	void commandReply(Turtle::Command data);

public slots:
	void setLinearSpeed(double speed) const { actor.linearSpeed() = speed; }
	void setRotationSpeed(double speed) const { actor.rotationSpeed() = speed; }

	void reset() { actor.reset(); }

	void setSingleStep(bool enable);
	void continueSingleStep();

	void command(const Command & data);

private:
	void commandUI(Command & data);
	void callback(TurtleActor::CallbackType type);

	TurtleActor & actor;
	bool pause;

	Command commandData;
};

#endif // TURTLEACTORCONTROLLER_H
