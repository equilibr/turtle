#include "Robot.h"

#include <array>
#include <cmath>

#include <osg/ShapeDrawable>
#include <osg/Geode>
#include <osg/MatrixTransform>
#include <osgUtil/SmoothingVisitor>
#include <osg/BlendFunc>

Robot::Robot(float radius, float height)
{
	const float penHeight = height / 2;
	const float bottomHeight = height / 2 + height / 4;
	const float topHeight = height / 2 - height / 4;
	const float hatHeight = height / 2 - height / 4;

	m_root = new osg::Group;

	//Create the robot parts
	pen = createCylinder(radius * 0.1f, penHeight);
	bottom = createCylinder(radius * 0.9f, bottomHeight);
	top = createCylinder(radius * 0.4f, topHeight);
	hat = createHat(radius * 0.7f, 0.1f, hatHeight);

	//Make the pen switchable
	osg::ref_ptr<osg::Geode> penGeode = bindDrawable(pen);
	makeTransparent(penGeode);
	penSwitch = new osg::Switch;
	penSwitch->addChild(bindGeode(penGeode, penHeight / 2), false);
	m_root->addChild(penSwitch);

	//Make the bottom transparent
	osg::ref_ptr<osg::Geode> bottomGeode = bindDrawable(bottom);
	makeTransparent(bottomGeode);
	m_root->addChild(bindGeode(bottomGeode, bottomHeight / 2));

	m_root->addChild(bindGeode(bindDrawable(top), bottomHeight - topHeight / 2  - topHeight / 16));

	m_root->addChild(bindGeode(bindDrawable(hat), bottomHeight));


	//Set initial values
	setPenColor({1,1,1,0.3f});
	setBottomColor({1,1,1,0.5f});
	setTopColor({1,1,1,1});
	setHatColor({0,0,1,1});
}

void Robot::setColor(osg::ref_ptr<osg::Geometry> node, osg::Vec4 color)
{
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
	colors->push_back(color );
	node->setColorArray(colors, osg::Array::BIND_OVERALL);
}

osg::ref_ptr<osg::Geometry> Robot::createCylinder(float radius, float height)
{
	osg::ref_ptr<osg::ShapeDrawable> shape = new osg::ShapeDrawable;
	shape->setShape(new osg::Cylinder({}, radius, height));

	return shape;
}

osg::ref_ptr<osg::Geometry> Robot::createHat(float radius, float width, float height)
{
	const float pi = acosf(-1);
	//Calculate the location of the base points
	auto xBase = -radius * cosf(pi*width);
	auto yBase = radius * sinf(pi*width);

	std::array<osg::Vec3, 5> vertices =
	{{
		{radius,0,0},
		{xBase,-yBase,0},
		{xBase,yBase,0},
		{xBase,yBase,height},
		{xBase,-yBase,height},
	}};

	std::array<unsigned int, 6*3> indices =
	{{
		 0,3,2,
		 0,2,1,
		 0,1,4,
		 0,3,4,
		 1,2,3,
		 3,4,1
	}};

	osg::ref_ptr<osg::Vec3Array> va =
			new osg::Vec3Array(vertices.size());
	for (size_t i = 0; i < vertices.size(); ++i)
		(*va)[i].set(vertices[i]);

	osg::ref_ptr<osg::DrawElementsUInt> ia =
			new osg::DrawElementsUInt(GL_TRIANGLES, indices.size());
	for (size_t i = 0; i < indices.size(); ++i)
		(*ia)[i] = indices[i];

	osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
	geometry->setVertexArray( va );
	geometry->addPrimitiveSet( ia );
	osgUtil::SmoothingVisitor::smooth( *geometry );

	return geometry;
}

osg::ref_ptr<osg::Geode> Robot::bindDrawable(osg::ref_ptr<osg::Geometry> node)
{
	setColor(node, {1,1,1,1});

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable( node );

	return geode;
}

osg::ref_ptr<osg::MatrixTransform> Robot::bindGeode(
		osg::ref_ptr<osg::Geode> node,
		float offset)
{
	osg::ref_ptr<osg::MatrixTransform> transform =
			new osg::MatrixTransform;

	transform->addChild(node);
	transform->setMatrix(osg::Matrix::translate( osg::Vec3{0,0,offset}));

	return transform;
}

void Robot::makeTransparent(osg::ref_ptr<osg::Geode> node)
{
	osg::ref_ptr<osg::BlendFunc> blendFunc = new osg::BlendFunc;
	blendFunc->setFunction( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	osg::StateSet* stateset = node->getOrCreateStateSet();
	stateset->setAttributeAndModes( blendFunc );
	stateset->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
}
