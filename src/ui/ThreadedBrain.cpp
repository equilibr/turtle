#include "ThreadedBrain.h"

#include "main.h"

void ThreadedBrain::run()
{
	active = true;
	emit started();

	//Main execution function
	MainBrain(*this);

	stop();
	emit stopped();
}

void ThreadedBrain::stop()
{
	active = false;
}
