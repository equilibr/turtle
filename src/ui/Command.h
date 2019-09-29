#ifndef COMMAND_H
#define COMMAND_H

#include "Types.h"

#include <QVariant>

namespace Turtle
{
	struct Command
	{
		struct UI
		{
			enum class Command
			{
				None,
				Log,
				GetInt,
				GetDouble,
				GetString,
			} command = Command::None;

			struct
			{
				//Used for log prefixes and request dialog titles.
				QString title;

				//User for log text and request dialog text
				QString text;

				//To set the log level
				int level;
			} data;

			//The data returned from the request dialog
			struct
			{
				QString string;
				int integer;
				double real;
			} reply;
		};

		struct Turtle
		{
			enum class Command
			{
				CurrentPosition,
				CurrentHeading,
				TargetPosition,
				TargetHeading,

				PenColor,
				PenState,

				Tile,
				TileSensor, //Get only

			} command;

			//Set when this is a "set" command, cleared when this is a "get" command
			bool set;

			//For positions. set when the the tiled position should be used instead of the normal one
			//For heading, set when the Direction/Heading should be used instead of the angle
			bool quantized;

			//Set when the absolute reference frame should be used
			bool absolute;


			//The following are used for both command data and replies

			//A generic boolean state
			bool state;

			//Continous position
			Position2D position;

			//Tiled/quantized position
			TilePosition2D tile;

			//Pen/tile colour
			QColor color;

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
				None,

				pX,
				nX,
				pY,
				nY,

				pXpY,
				pXnY,
				nXpY,
				nXnY,
			} heading;

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
