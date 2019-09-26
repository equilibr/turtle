#include "Scene.h"

#include <osg/MatrixTransform>
#include <osg/Material>
#include <osg/PolygonMode>
#include <osg/Light>
#include <osg/LightSource>
#include <osg/Fog>
#include <osgFX/Cartoon>

using namespace Turtle;

void setupMaterial(const osg::ref_ptr<osg::Node> & node)
{
	osg::ref_ptr<osg::StateSet> stateSet = node->getOrCreateStateSet();
	osg::ref_ptr<osg::Material> material = new osg::Material;
	material->setColorMode( osg::Material::AMBIENT_AND_DIFFUSE );
	stateSet->setAttributeAndModes( material );
	stateSet->setMode( GL_DEPTH_TEST, osg::StateAttribute::ON );
//	stateSet->setMode( GL_LIGHTING, osg::StateAttribute::ON );
	stateSet->setMode( GL_LIGHT0 ,osg::StateAttribute::ON );
	stateSet->setMode( GL_LIGHT1 ,osg::StateAttribute::ON );
}

osg::ref_ptr<osgFX::Cartoon> setupCartoon(const osg::ref_ptr<osg::Node> node)
{
	osg::ref_ptr<osgFX::Cartoon> cartoon = new osgFX::Cartoon;

	cartoon->addChild(node);
	return cartoon;
}

void enableWireframe(const osg::ref_ptr<osg::Node> & node)
{
	osg::ref_ptr<osg::PolygonMode> pm = new osg::PolygonMode;
	pm->setMode(osg::PolygonMode::FRONT_AND_BACK,osg::PolygonMode::LINE);
	node->getOrCreateStateSet()->setAttribute(pm);
}

void setupFog(const osg::ref_ptr<osg::Node> & node)
{
	osg::ref_ptr<osg::Fog> fog = new osg::Fog;
	fog->setMode( osg::Fog::LINEAR );
	fog->setStart( 40.0f );
	fog->setEnd( 100.0f );
	fog->setColor( osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f) );

	node->getOrCreateStateSet()->setAttributeAndModes( fog );
}

osg::ref_ptr<osg::Node> createLightSource(
		int num,
		const osg::Vec3& trans,
		const osg::Vec4& color)
{
	osg::ref_ptr<osg::Light> light = new osg::Light;
	light->setLightNum( num );
	light->setDiffuse(color);
	light->setPosition( osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f) );

	osg::ref_ptr<osg::LightSource> lightSource = new osg::LightSource;
	lightSource->setLight( light );

	osg::ref_ptr<osg::MatrixTransform> sourceTrans = new osg::MatrixTransform;
	sourceTrans->setMatrix( osg::Matrix::translate(trans) );
	sourceTrans->addChild( lightSource );
	return sourceTrans;
}

osg::ref_ptr<osg::Group> setupScene()
{
	osg::ref_ptr<osg::Group> root = new osg::Group;

	root->addChild(createLightSource(0,{0,0,1000},{1,1,1,0}));
	root->addChild(createLightSource(1,{-20,-20,100},{1,1,1,0}));

	setupFog(root);
	setupMaterial(root);

	return root;
}


Scene::Scene()
{
	m_root = setupScene();
}
