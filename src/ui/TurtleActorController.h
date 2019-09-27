#ifndef TURTLEACTORCONTROLLER_H
#define TURTLEACTORCONTROLLER_H

#include <QObject>

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

	void setLinearSpeed(double speed) const { actor.linearSpeed() = speed; }
	void setRotationSpeed(double speed) const { actor.rotationSpeed() = speed; }


signals:
	void newState(TurtleActor::State state, TurtleActor::CallbackType action);
	void newRunState(bool active);

public slots:
	void reset() { actor.reset(); }

	void setSingleStep(bool enable);
	void continueSingleStep();

	//Directly set the target position
	void setTargetPosition(Position2D target) const;
	void setTargetPositionX(Position2D::value_type target) const;
	void setTargetPositionY(Position2D::value_type target) const;

	//Directly set the target angle
	void setTargetAngle(double target) const;

	//Set the pen color
	void setPenColor(QColor color) const;

	//Set the pen state
	void setPenDown(bool down) const;

	//Move on the current heading(angle)
	void setMove(Position2D::value_type distance) const;

	//Rotate by some angle
	void setRotate(const double angle) const;

private:
	void callback(TurtleActor::CallbackType type);

	TurtleActor & actor;
	bool pause;
};

inline void TurtleActorController::setSingleStep(bool enable)
{
	pause = enable;
	actor.pause(pause);
}

inline void TurtleActorController::continueSingleStep()
{
	actor.unpause();
}

inline void TurtleActorController::setTargetPosition(Position2D target) const
{
	TurtleActor::Location location = actor.state().target;
	location.position = target;
	actor.setTarget(location);
}

void TurtleActorController::setTargetPositionX(Position2D::value_type target) const
{
	TurtleActor::Location location = actor.state().target;
	location.position.x() = target;
	actor.setTarget(location);
}

void TurtleActorController::setTargetPositionY(Position2D::value_type target) const
{
	TurtleActor::Location location = actor.state().target;
	location.position.y() = target;
	actor.setTarget(location);
}

inline void TurtleActorController::setTargetAngle(double target) const
{
	TurtleActor::Location location = actor.state().target;
	location.angle = target;
	actor.setTarget(location);
}

inline void TurtleActorController::setPenColor(QColor color) const
{
	TurtleActor::Pen pen = actor.state().pen;
	pen.color = color;
	actor.setPen(pen);
}

inline void TurtleActorController::setPenDown(bool down) const
{
	TurtleActor::Pen pen = actor.state().pen;
	pen.down = down;
	actor.setPen(pen);
}

inline void TurtleActorController::setMove(Position2D::value_type distance) const
{
	actor.move(distance);
}

inline void TurtleActorController::setRotate(const double angle) const
{
	actor.rotate(angle);
}

#endif // TURTLEACTORCONTROLLER_H
