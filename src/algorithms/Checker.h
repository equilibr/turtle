#ifndef CHECKER_H
#define CHECKER_H

#include "ThreadedBrain.h"

class Checker
{
public:
	Checker(
			ThreadedBrain & brain,
			QColor on = Qt::black,
			QColor off = Qt::white,
			QColor on2 = Qt::red,
			QColor off2 = Qt::green);

	QColor operator()(bool value);

	void reset();

private:
	QColor color(bool value, bool checker) const;
	QColor stateColor() const;

	ThreadedBrain & brain;
	QColor on;
	QColor off;
	QColor on2;
	QColor off2;

	enum class State
	{
		Initial,
		On,
		Off,
		OnChecker,
		OffChecker
	} state;

};

#endif // CHECKER_H
