#include "TurtleActorController.h"

TurtleActorController::TurtleActorController(
		TurtleActor &actor,
		QObject *parent) :
	QObject(parent),
	actor{actor},
	pause{false}
{
	//Add our callback dispatcher to the actors' callbacks list
	actor.callbacks().push_back([this](TurtleActor::CallbackType type){callback(type);} );
}

void TurtleActorController::callback(TurtleActor::CallbackType type)
{
	switch (type)
	{
		case TurtleActor::CallbackType::Reset:
			//After a reset the actor is active
			emit newRunState(true);

			//Restore the remembered pause mode
			actor.pause(pause);

			emit newState(actor.state(), TurtleActor::CallbackType::Reset);
			break;

		case TurtleActor::CallbackType::Active:
			emit newRunState(true);
			break;

		case TurtleActor::CallbackType::Paused:
			emit newRunState(false);
			break;

		case TurtleActor::CallbackType::Current:
		case TurtleActor::CallbackType::Target:
		case TurtleActor::CallbackType::Pen:
		case TurtleActor::CallbackType::Move:
		case TurtleActor::CallbackType::Rotate:
			emit newState(actor.state(), type);
			break;
	}
}
