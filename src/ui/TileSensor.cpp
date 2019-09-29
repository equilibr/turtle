#include "TileSensor.h"

using namespace Turtle;

TileSensor::TileSensor(const TileSensor &rhs) :
	length{rhs.length},
	center{rhs.center},
	data{rhs.data}
{
}

TileSensor::TileSensor(const TileSensor::Data & data, int size) :
	length{size*2 + 1},
	center{size},
	data{data}
{
}

TileSensor & TileSensor::operator=(const TileSensor & rhs)
{
	//Use placement new to perform the copy
	if(this == &rhs) return *this;
	this->~TileSensor();
	return *new(this) TileSensor(rhs);
}

TileSensor::Data::size_type TileSensor::index(int front, int side) const
{
	const int onHeading = center + front;
	const int onSide = center + side;

	return static_cast<Data::size_type>(onHeading * length + onSide);
}
