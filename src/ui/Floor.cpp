#include "Floor.h"

#include <osg/Geometry>
#include <osg/Geode>
#include <osg/MatrixTransform>
#include <osg/ShapeDrawable>

Floor::Floor(size_t xCount, size_t yCount, float size) :
	xBase(-static_cast<int>(xCount)),
	yBase(-static_cast<int>(yCount)),
	size(size)
{
	//Generate tiles
	osg::Vec4 color {1,1,1,1};

	Index::value_type xSize = xCount*2 + 1;
	Index::value_type ySize = yCount*2 + 1;

	geometries.resize(xSize);
	for (auto & v : geometries)
		v.resize(ySize);

	float height = size / 10;
	Index index;

	for (index[0] = 0; index[0] < xSize; ++index[0])
		for (index[1] = 0; index[1] < ySize; ++index[1])
		{
			osg::ref_ptr<osg::Geometry> geometry = createTile(size, height);
			setColor(geometry, color);
			getGeometry(getPosition(index)) = geometry;

			osg::ref_ptr<osg::Geode> geode = new osg::Geode;
			geode->addDrawable( geometry );

			osg::ref_ptr<osg::MatrixTransform> transform =
					new osg::MatrixTransform;

			transform->addChild(geode);
			Coordinate coordinate = getCoordinate(getPosition(index));
			coordinate[2] = -height/2;
			transform->setMatrix(osg::Matrix::translate(coordinate));

			addChild(transform);
		}
}

void Floor::setColor(osg::ref_ptr<osg::Geometry> node, osg::Vec4 color)
{
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
	colors->push_back(color );
	node->setColorArray(colors, osg::Array::BIND_OVERALL);
}

osg::ref_ptr<osg::Geometry> Floor::createQuad(float size)
{
	float halfSize = size / 2;

	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
	vertices->push_back( osg::Vec3(-halfSize, -halfSize, 0.0) );
	vertices->push_back( osg::Vec3(halfSize, -halfSize, 0.0) );
	vertices->push_back( osg::Vec3(halfSize, halfSize, 0.0) );
	vertices->push_back( osg::Vec3(-halfSize, halfSize, 0.0) );

	osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
	normals->push_back( osg::Vec3(0.0f,-1.0f, 0.0f) );

	osg::ref_ptr<osg::Geometry> quad = new osg::Geometry;
	quad->setVertexArray( vertices );
	quad->setNormalArray( normals );
	quad->setNormalBinding( osg::Geometry::BIND_OVERALL );

	quad->addPrimitiveSet( new osg::DrawArrays(osg::DrawArrays::QUADS, 0, 4) );

	return quad;
}

osg::ref_ptr<osg::ShapeDrawable> Floor::createTile(float size, float height)
{
	osg::ref_ptr<osg::ShapeDrawable> shape = new osg::ShapeDrawable;
	shape->setShape(new osg::Box( {}, size * 0.9f, size * 0.9f, height));
	return shape;
}
