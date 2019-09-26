#ifndef SCENE_H
#define SCENE_H

#include <osg/Group>

namespace Turtle
{
	//Creates and manages the 3D scene
	class Scene
	{
	public:
		Scene();

		void addChild(const osg::ref_ptr<osg::Node> node) { m_root->addChild(node);}

		//Access functors
		osg::ref_ptr<osg::Node> root() {return m_root;}

	private:
		osg::ref_ptr<osg::Group> m_root;
	};

}
#endif // SCENE_H
