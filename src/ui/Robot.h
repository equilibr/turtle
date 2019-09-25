#ifndef ROBOT_H
#define ROBOT_H

#include <osg/Group>
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/Switch>

class Robot : public osg::Group
{
public:
	Robot(float radius = 0.5, float height = 0.5);

	void setPenColor(osg::Vec4 color) { setColor(pen, color); }
	void setBottomColor(osg::Vec4 color) { setColor(bottom, color); }
	void setTopColor(osg::Vec4 color) { setColor(top, color); }
	void setHatColor(osg::Vec4 color) { setColor(hat, color); }

	void setPenState(bool enabled) { penSwitch->setValue(0, enabled); }

protected:
	void setColor(osg::ref_ptr<osg::Geometry> node, osg::Vec4 color);

private:
	osg::ref_ptr<osg::Geometry> createCylinder(float radius, float height);
	osg::ref_ptr<osg::Geometry> createHat(float radius, float width, float height);

	osg::ref_ptr<osg::Geode> bindDrawable(osg::ref_ptr<osg::Geometry> node);

	osg::ref_ptr<osg::MatrixTransform> bindGeode(
			osg::ref_ptr<osg::Geode> node,
			float offset);

	void makeTransparent(osg::ref_ptr<osg::Geode> node);

	osg::ref_ptr<osg::Geometry> pen, bottom, top, hat;
	osg::ref_ptr<osg::Switch> penSwitch;
};

#endif // ROBOT_H
