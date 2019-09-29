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
	void newTileSensor(Turtle::TileSensor sensor);

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

	//Return the current tile sensor
	void getTileSensor();

private:
	void callback(TurtleActor::CallbackType type);

	TurtleActor & actor;
	bool pause;
};

#endif // TURTLEACTORCONTROLLER_H
