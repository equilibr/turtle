#ifndef TILESENSOR_H
#define TILESENSOR_H

#include <vector>

#include <QMetaType>
#include <QColor>

namespace Turtle
{
	//A tile sensor matrix
	class TileSensor
	{
	public:
		//Data perpendicular to the heading has a stride of 1
		using Data = std::vector<QColor>;

		TileSensor() = default;
		TileSensor(const TileSensor & rhs);
		TileSensor(const Data & data, int size = 3);
		TileSensor & operator=(const TileSensor &rhs);

		//Return the color at a given position
		QColor get(int front, int side) const
		{ return data[index(front, side)]; }

		//The size of a dimension
		int size() const {return length;}

	private:
		Data::size_type index(int front, int side) const;

		const int length = 0;
		const int center = 0;

		Data data;
	};
}

Q_DECLARE_METATYPE(Turtle::TileSensor)

#endif // TILESENSOR_H
