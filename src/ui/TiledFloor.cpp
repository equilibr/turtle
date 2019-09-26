#include "TiledFloor.h"

#include <osg/Geometry>

using namespace Turtle;

using OsgColor = osg::Vec4;

OsgColor fromQColor(const QColor & color)
{
	return OsgColor
	{
		static_cast<OsgColor::value_type>(color.redF()),
		static_cast<OsgColor::value_type>(color.greenF()),
		static_cast<OsgColor::value_type>(color.blueF()),
		static_cast<OsgColor::value_type>(color.alphaF())
	};
}


TiledFloor::TiledFloor(const TiledFloor::Index2D && size, const QColor clearColor, const Position2D && tileSize)
{
	setClearColor(clearColor);
	reset(size, tileSize);

	m_texture = new osg::Texture2D;

	//Create default members to avoid dereferencing null pointers
	m_textureImage = new osg::Image;
	m_floor = new osg::Geode;
}

void TiledFloor::setImage(const QImage & image)
{
	Coordinate<int,2> p {image.width(), image.height()};
	reset(p, m_tileSize);

	//Copy image data to texture
	for (int s = 0; s < image.width(); ++s)
		for (int t = 0; t < image.height(); ++t)
		{
			QColor color = image.pixelColor(s,t);
			m_textureImage->setColor(fromQColor(color), static_cast<unsigned int>(s), static_cast<unsigned int>(t));
		}
}

void TiledFloor::reset(const Index2D & size, const Position2D & tileSize)
{
	m_tileSize = tileSize;
	m_halfIndexSize = size;
	m_halfPositionSize = tileSize * size + tileSize / 2;
	m_Base = -size;

	const TilePosition2D dimentions = size * 2 + 1;

	//Create the image
	m_image = QImage(dimentions.x(), dimentions.y(), QImage::Format_ARGB32);

	//Create the texture image
	m_textureImage->allocateImage(dimentions.x(),dimentions.y(),1, GL_RGB, GL_UNSIGNED_BYTE);

	clear();
}

void TiledFloor::clear()
{
	m_image.fill(m_clearColor);

	OsgColor color = fromQColor(m_clearColor);

	for (int s = 0; s < m_textureImage->s(); ++s)
		for (int t = 0; t < m_textureImage->t(); ++t)
			m_textureImage->setColor(color, static_cast<unsigned int>(s), static_cast<unsigned int>(t));
}

void TiledFloor::setColor(TiledFloor::Index2D position, QColor color)
{
	m_image.setPixelColor(
				static_cast<int>(position.x()),
				static_cast<int>(position.y()),
				color);

	m_textureImage->setColor(
				fromQColor(color),
				static_cast<unsigned int>(position.x()),
				static_cast<unsigned int>(position.y()));
}

TiledFloor::Index2D TiledFloor::toIndex(Position2D position) const
{
	using T = Position2D::value_type;
	auto toTile = [](T coord, T size) -> TilePosition2D::value_type
	{
		if (fabs(coord) < size/2)
			return 0;

		T shifted = std::floor(fabs(coord) + size / 2);
		T unwrapped = shifted * (coord / fabs(coord));
		return static_cast<TilePosition2D::value_type>(unwrapped);
	};

	TilePosition2D tile
	{
		toTile(position.x(), m_tileSize.x()),
		toTile(position.y(), m_tileSize.y())
	};

	return tile - m_Base;
}

void TiledFloor::createQuad()
{
	auto toVec3 = [](Position2D pos) -> osg::Vec3
	{
		return osg::Vec3
		{
			static_cast<osg::Vec3::value_type>(pos.x()),
			static_cast<osg::Vec3::value_type>(pos.y()),
			0
		};
	};

	Position2D a;
	a = a * Position2D{1,-1};

	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
	vertices->push_back( toVec3(m_halfPositionSize * Position2D{-1,-1}) );
	vertices->push_back( toVec3(m_halfPositionSize * Position2D{1,-1}) );
	vertices->push_back( toVec3(m_halfPositionSize * Position2D{1,1}) );
	vertices->push_back( toVec3(m_halfPositionSize * Position2D{-1,1}) );

	osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
	normals->push_back( osg::Vec3(0.0f,-1.0f, 0.0f) );

	osg::ref_ptr<osg::Vec2Array> texcoords = new osg::Vec2Array;
	texcoords->push_back( osg::Vec2(0.0f, 0.0f) );
	texcoords->push_back( osg::Vec2(0.0f, 1.0f) );
	texcoords->push_back( osg::Vec2(1.0f, 1.0f) );
	texcoords->push_back( osg::Vec2(1.0f, 0.0f) );

	osg::ref_ptr<osg::Geometry> quad = new osg::Geometry;
	quad->setVertexArray( vertices );
	quad->setNormalArray( normals );
	quad->setNormalBinding( osg::Geometry::BIND_OVERALL );
	quad->setTexCoordArray( 0, texcoords );

	quad->addPrimitiveSet( new osg::DrawArrays(osg::DrawArrays::QUADS, 0, 4) );

	m_floor = new osg::Geode;
	m_floor->addDrawable( quad );
	m_floor->getOrCreateStateSet()->setTextureAttributeAndModes(0, m_texture );
}
