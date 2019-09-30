#ifndef COMMAND_H
#define COMMAND_H

#include "Types.h"
#include "TileSensor.h"

#include <QVariant>

namespace Turtle
{
	struct Command
	{
		struct UI
		{
			enum class Command
			{
				Log,
				GetInt,
				GetDouble,
				GetString,
			} command;

			//Used for log prefixes and request dialog titles.
			QString title;

			//User for log text and request dialog text
			QString text;

			//To set the log level
			int level;

			//Default initial values and returned values for the request dialogs

			QString string;
			int integer;
			double real;

			double min, max, step;
		};

		struct Turtle
		{
			enum class Command
			{
				//Retrieve all available data
				Get,

				//Set some data
				Set
			} command;

			//Selects the target for the command
			enum class Target
			{
				//The color is the pen color
				Current,
				Target,

				//The color is the tile color
				Tile,
			} target;

			//Select what to set
			//The pen is set for the current and target items
			bool setPosition;
			bool setHeading;
			bool setPenColor;
			bool setPenState;

			//For positions. set when the the tiled position should be used instead of the normal one
			//For heading, set when the Direction/Heading should be used instead of the angle
			bool quantized;

			//When set the absolute reference frame should be used
			//When clear the current-heading reference frame should be used
			bool absolute;


			//The following are used for both command data and replies

			//Continous position
			Position2D position;

			//Tiled/quantized position
			TilePosition2D tile;

			//Pen/tile colour
			QColor color;

			//Pen state
			bool penDown;


			//Angle, in units of "circle". The positive direction is counter-clockwise.
			double angle;

			//Relative direction
			enum class Direction
			{
				None,

				Forward,
				Backward,
				Left,
				Right,

				ForwardLeft,
				ForwardRight,
				BackwardLeft,
				BackwardRight,
			} direction;

			//Absolute heading
			enum class Heading
			{
				PositiveX,
				NegativeX,
				PositiveY,
				NegativeY
			} heading;

			TileSensor tileSensor;
		};

		//Top level command destination
		//Used to simplify dispatching
		enum class Destination
		{
			UI,
			Turtle
		} destination;

		//Set when this is a reply to a command
		bool reply;

		//Set when this reply has valid data
		bool valid;

		struct
		{
			UI ui;
			Turtle turtle;
		} data;
	};
}

Q_DECLARE_METATYPE(Turtle::Command)

#endif // COMMAND_H
