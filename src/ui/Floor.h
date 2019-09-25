#ifndef FLOOR_H
#define FLOOR_H

#include <array>
#include <vector>

#include <osg/Group>
#include <osg/Geometry>
#include <osg/ShapeDrawable>

class Floor : public osg::Group
{
public:
	using Coordinate = osg::Vec3;
	using Position = std::array<int, 2>;
	using Index = std::array<size_t, 2>;

	Floor(size_t xCount = 0, size_t yCount = 0, float size = 1);

	void setColor(Coordinate position = {}, osg::Vec4 color = {1,1,1,0})
	{ setColor(getPosition(position), color); }

	void setColor(Position position, osg::Vec4 color)
	{ setColor(getGeometry(position), color); }

	Position getPosition(Coordinate position) const
	{
		auto toIndex = [this](Coordinate::value_type x) -> int
		{
			if (fabsf(x) < size/2)
				return 0;

			float shifted = floorf(fabsf(x) + size / 2);
			float unwrapped = shifted * (x / fabsf(x));
			return static_cast<int>(unwrapped);
		};

		return
		{
			static_cast<int>(toIndex(position[0])),
			static_cast<int>(toIndex(position[1]))
		};
	}

	Index getIndex(Position position) const
	{
		return
		{
			static_cast<Index::size_type>(position[0] - xBase),
			static_cast<Index::size_type>(position[1] - yBase)
		};
	}

protected:
	using GeometryVector = std::vector<std::vector<osg::ref_ptr<osg::Geometry>>>;

	void setColor(osg::ref_ptr<osg::Geometry> node, osg::Vec4 color);

	osg::ref_ptr<osg::Geometry> createQuad(float size);
	osg::ref_ptr<osg::ShapeDrawable> createTile(float size, float height);

	Position getPosition(Index index) const
	{
		return
		{
			static_cast<Position::value_type>(index[0]) + xBase,
			static_cast<Position::value_type>(index[1]) + yBase
		};
	}

	Coordinate getCoordinate(Position position) const
	{ return {position[0] * size, position[1] * size, 0}; }

	osg::ref_ptr<osg::Geometry> & getGeometry(Position position)
	{
		auto index = getIndex(position);
		return geometries[index[0]][index[1]];
	}

	GeometryVector geometries;
	Position::value_type xBase, yBase;
	float size;
};

#endif // FLOOR_H
