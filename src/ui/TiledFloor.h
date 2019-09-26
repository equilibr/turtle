#ifndef TILEDFLOOR_H
#define TILEDFLOOR_H

#include "Types.h"

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
		//A position in tile space with a corner as a reference
		using IndexPoint = size_t;
		using Index2D = Coordinate<IndexPoint,2>;
		using Index3D = Coordinate<IndexPoint,3>;

		//A position in tile space
		using TilePosition = int;
		using TilePosition2D = Coordinate<TilePosition,2>;
		using TilePosition3D = Coordinate<TilePosition,3>;


		TiledFloor(const Index2D & size = {},
				const QColor & clearColor = Qt::white,
				const Position2D & tileSize = {1,1});

		//Set a new image
		void setImage(const QImage & image);

		//Access functors
		const QImage & image() const {return m_image;}
		osg::ref_ptr<osg::Node> root() {return m_root;}

		//Set the clear color to use
		void setClearColor(QColor color) {m_clearColor = color;}

		//(re)create a floor
		void reset(const Index2D & size, const Position2D & tileSize);

		//Clear the floor to its initial state
		void clear();

		//Set a pixel color at a given position
		void setColor(Position2D position, QColor color) {setColor(toIndex(position),color);}

	private:
		void setColor(Index2D position, QColor color);
		Index2D toIndex(Position2D position) const;
		void createQuad();


		//The size of each tile
		Position2D m_tileSize;

		//The one sided (distance from origin to edge) size of the floor
		Index2D m_halfIndexSize;

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
