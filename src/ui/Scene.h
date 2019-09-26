#ifndef SCENE_H
#define SCENE_H

#include <osg/Group>

namespace Turtle
{
	//Manages the 3D scene

	class Scene
	{
	public:
		Scene(osg::ref_ptr<osg::Node> floor);

		//Access functors
		osg::ref_ptr<osg::Node> root() {return m_root;}

	private:
		osg::ref_ptr<osg::Group> m_root;
	};

}
#endif // SCENE_H
