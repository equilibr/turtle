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

	void setLinearSpeed(double speed) const { actor.linearSpeed() = speed; }
	void setRotationSpeed(double speed) const { actor.rotationSpeed() = speed; }


signals:
	void newState(TurtleActor::State state, TurtleActor::CallbackType action);
	void newRunState(bool active);

	void newCurrentState(TurtleActor::State state);
	void newTile(QColor color);
	void newTileSensor(Turtle::TileSensor sensor);

	//Emmited when a new command reply is available
	void commandReply(Turtle::Command data);

public slots:
	void reset() { actor.reset(); }

	void setSingleStep(bool enable);
	void continueSingleStep();

	//Directly set the target position
	void setTargetPosition(Position2D target) const;
	void setTargetPositionX(Position2D::value_type target) const;
	void setTargetPositionY(Position2D::value_type target) const;
	void getCurrentState();


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

	//Set/Get a tile at a location relative to the current direction
	//The the non-absoulte position "y" axis is "sideways", and the "x" axis is "front"
	void setTile(const QColor color, const TilePosition2D offset, bool absolute);
	void getTile(const TilePosition2D offset, bool absolute);

	//Return the current tile sensor
	void getTileSensor();

	void command(const Command & data);

private:
	void callback(TurtleActor::CallbackType type);

	TurtleActor & actor;
	bool pause;
};

#endif // TURTLEACTORCONTROLLER_H
