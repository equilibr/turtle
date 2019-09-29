#include "Checker.h"

Checker::Checker(Colors colors) :
	colors{colors}
{
	reset();
}

Checker::Checker() : Checker{Qt::black, 0.95, 0.2}
{
}

Checker::Checker(QColor on, double offFactor, double onFactor)
{
	colors[0][0] = QColor::fromHsvF(0,0,1.0);
	colors[0][1] = QColor::fromHsvF(0,0,offFactor);

	colors[1][0] = on;
	colors[1][1] = QColor::fromHsvF(on.hueF(), on.saturationF(), onFactor);

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

	return colors[value ? 1 :0][checker ? 1 :0];
}
