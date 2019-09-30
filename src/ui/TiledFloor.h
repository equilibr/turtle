#ifndef TILEDFLOOR_H
#define TILEDFLOOR_H

#include "Types.h"
#include "TileSensor.h"

#include <QImage>

#include <osg/Node>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Texture2D>

namespace Turtle
{
	//Represents and handles a floor divided to tiles
	//This includes both the simulation and graphical data
	class TiledFloor
	{
	public:

		TiledFloor(const Index2D & size = {},
				const QColor & clearColor = Qt::white,
				const Position2D & tileSize = {1,1});

		Position2D halfSize() const { return m_halfPositionSize; }

		//Set a new image
		void setImage(const QImage & image);

		//Access functors
		const QImage & image() const {return m_image;}
		osg::ref_ptr<osg::Node> root() {return m_root;}

		//Set the clear color to use
		void setClearColor(const QColor & color) {m_clearColor = color;}

		//(re)create a floor
		void reset(const Index2D & size, const Position2D & tileSize = {1,1});

		//Clear the floor to its initial state
		void clear();

		//Set a pixel color at a given position
		void setColor(const Position2D & position, const QColor & color) {setColor(toIndex(position),color);}
		void setColor(const TilePosition2D & position, const QColor & color) {setColor(toIndex(position),color);}

		//Get a pixel color at a given position
		QColor getColor(const Position2D & position) {return getColor(toIndex(position));}
		QColor getColor(const TilePosition2D & position) {return getColor(toIndex(position));}

		TileSensor getTiles(const TilePosition2D position, size_t size) const;

		TilePosition2D toTileIndex(const Position2D & position) const;
		Position2D toPosition(const TilePosition2D & index) const;
	private:
		Index2D toIndex(const Position2D & position) const;
		Index2D toIndex(const TilePosition2D & position) const;
		void setColor(const Index2D & position, const QColor & color);
		QColor getColor(const Index2D & position);
		void createQuad();


		//The size of each tile
		Position2D m_tileSize;

		//The one sided (distance from origin to edge) size of the floor
		TilePosition2D m_halfIndexSize;

		//The one sided (distance from origin to edge) size of the floor
		Position2D m_halfPositionSize;

		//The color to use for clear operations
		QColor m_clearColor;

		//The pixels
		QImage m_image;
		osg::ref_ptr<osg::Image> m_textureImage;
		osg::ref_ptr<osg::Texture2D> m_texture;

		//The textured floor geometry
		osg::ref_ptr<osg::Geode> m_floor;

		//The floor root
		osg::ref_ptr<osg::Group> m_root;

		//Base for converting from center-origin to corner-origin
		TilePosition2D m_Base;
	};

}
#endif // TILEDFLOOR_H
