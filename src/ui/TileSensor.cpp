#include "TileSensor.h"

using namespace Turtle;

TileSensor::TileSensor(const TileSensor::Data & data, int size) :
	length{size},
	center{(size - 1) / 2},
	data{data}
{
}

TileSensor::Data::size_type TileSensor::index(int front, int side) const
{
	const int onHeading = center + front;
	const int onSide = center + side;

	return static_cast<Data::size_type>(onHeading * length + onSide);
}