#ifndef CHECKER_H
#define CHECKER_H

#include "ThreadedBrain.h"

#include <array>

class Checker
{
public:
	using Colors = std::array<std::array<QColor,2>,2>;

	explicit Checker(Colors colors);
	Checker();
	Checker(QColor on, double offFactor = 0.95, double onFactor = 0.8);

	QColor operator()(bool value);

	void reset();

private:
	QColor stateColor() const;
	Colors colors;

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
