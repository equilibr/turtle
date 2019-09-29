#include "Checker.h"

Checker::Checker(
		ThreadedBrain & brain,
		QColor on,
		QColor off,
		QColor on2,
		QColor off2) :
	brain{brain},
	on{on},
	off{off},
	on2{on2},
	off2{off2}
{
	reset();
}

QColor Checker::operator()(bool value)
{
	switch (state)
	{
		case State::Initial:
			state = value ? State::On : State::Off;
			break;

		case State::On:
			if (value)
				state = State::OnChecker;
			else
				state = State::Off;
			break;

		case State::Off:
			if (value)
				state = State::On;
			else
				state = State::OffChecker;
			break;

		case State::OnChecker:
			if (value)
				state = State::On;
			else
				state = State::Off;
			break;

		case State::OffChecker:
			if (value)
				state = State::On;
			else
				state = State::Off;
			break;
	}

	return stateColor();
}

void Checker::reset()
{
	state = State::Initial;
}

QColor Checker::stateColor() const
{
	bool value = (state == State::On) || (state == State::OnChecker);
	bool checker = (state == State::OnChecker) || (state == State::OffChecker);

	return color(value, checker);
}

QColor Checker::color(bool value, bool checker) const
{
	if (!checker)
		return value ? on : off;
	else
		return value ? on2 : off2;
}
