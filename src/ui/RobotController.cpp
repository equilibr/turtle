#include "RobotController.h"

#include <cmath>

const double RobotController::pi = 	acos(-1);

RobotController::RobotController(double frameRate, QObject *parent) :
	QObject (parent),
	frameRate(frameRate),
	fieldSize(0),
	colorCycle(0),

	idle(true),
	singleStep(false),

	robot(new Robot),
	transform(new osg::MatrixTransform),
	floor(new Floor(fieldSize, fieldSize)),
	root(new osg::Group)
{
	penDown = false;
	penColor = {1,1,1,1};

	currentPosition = {};
	currentAngle = 0;

	targetPosition = {};
	targetAngle = 0;

	transform->addChild(robot);
	root->addChild(transform);
	root->addChild(floor);
}

void RobotController::step()
{
	//Hat color cycle
	//---------------

	float alpha = static_cast<float>(sin(pi * colorCycle));
	colorCycle = fmod(colorCycle + 1e-1 / frameRate, 1);
	robot->setHatColor({0,1-alpha,alpha,1});

	if (idle)
		return;
	idle = true;

	//Position
	//--------

	//Calculate distance to set point
	osg::Vec3 delta = targetPosition - currentPosition;
	double distance = static_cast<double>(delta.length());

	if (distance <= (linearSpeed / frameRate))
		//The step is small enough to directly set it
		currentPosition = targetPosition;
	else
	{
		osg::Vec3 unit = delta;
		unit.normalize();
		currentPosition += unit * linearSpeed / static_cast<float>(frameRate);
		idle = false;
	}

	//Angle
	//-----

	//Calculate the shortest path
	double rDistance = normalizeAngle(targetAngle - currentAngle);
	if (fabs(rDistance) >= 1.0)
		rDistance = 0;
	else if (fabs(rDistance) > 0.5)
		//Go the other way
		rDistance = -rDistance;

	if (fabs(rDistance) <= (rotationSpeed / frameRate))
		currentAngle = targetAngle;
	else
	{
		currentAngle =
				normalizeAngle(
					currentAngle +
					rDistance  / fabs(rDistance)
					* rotationSpeed
					/ static_cast<double>(frameRate));
		idle = false;
	}

	//Pen handling
	//------------

	robot->setTopColor(penColor);
	robot->setPenColor(penColor);

	if (penDown)
	{
		const Floor::Position position = floor->getPosition(currentPosition);

		if (penDirty || (position != lastPenPosition))
		{
			floor->setColor(position, penColor);

			const Floor::Index index = floor->getIndex(position);
			const QPoint point(static_cast<int>(index[0]), fieldSize*2 - static_cast<int>(index[1]));
			QColor color = QColor::fromRgbF(
					static_cast<double>(penColor[0]),
					static_cast<double>(penColor[1]),
					static_cast<double>(penColor[2]));

			image.setPixelColor(point, color);

			lastPenPosition = position;
			penDirty = false;
			emit imageChanged();
		}
	}

	//Notification and action
	//-----------------------

	transform->setMatrix(
				osg::Matrix::rotate(2*pi*currentAngle,0,0,1) *
				osg::Matrix::translate( currentPosition[0], currentPosition[1], 0)
			);

	emit currentStateChanged(
			currentPosition[0],
			currentPosition[1],
			currentAngle);

	if (idle)
	{
		emit stoppedOnStep(singleStep);

		if (!singleStep)
			emit ideling();
	}
}

void RobotController::clear()
{
	Floor::Coordinate coordinate;
	osg::Vec3d position;

	const int size = static_cast<int>(fieldSize);
	const osg::Vec4 penColor = {1,1,1,1};

	for (coordinate[0] = -size; coordinate[0] < size; ++coordinate[0])
		for (coordinate[1] = -size; coordinate[1] < size; ++coordinate[1])
		{
			const Floor::Position position = floor->getPosition(coordinate);
			floor->setColor(position, penColor);

			const Floor::Index index = floor->getIndex(position);
			const QPoint point(static_cast<int>(index[0]), fieldSize*2 - static_cast<int>(index[1]));
			QColor color = QColor::fromRgbF(
					static_cast<double>(penColor[0]),
					static_cast<double>(penColor[1]),
					static_cast<double>(penColor[2]));

			image.setPixelColor(point, color);
		}
}

void RobotController::continueStep()
{
	emit stoppedOnStep(false);
	emit ideling();
}

void RobotController::emitStatus()
{
	emit fieldSizeChanged(fieldSize);

	emit penStateChanged(penDown);
	emit penColorChanged(penColor[0], penColor[1], penColor[2]);

	emit currentStateChanged(currentPosition[0], currentPosition[1], currentAngle);
	emit targetStateChanged(targetPosition[0], targetPosition[1], targetAngle);

	emit stoppedOnStep(false);
}

void RobotController::resetField(size_t size)
{
	fieldSize = size;

	//Create a new floor and replace it in the root
	floor = new Floor(size, size);
	root->removeChild(1,1);
	root->addChild(floor);

	image = QImage(static_cast<int>(size*2 + 1), static_cast<int>(size*2 + 1), QImage::Format_RGB32);
	image.fill(Qt::white);
	penDirty = true;

	emit fieldSizeChanged(fieldSize);
}

void RobotController::setPenDown(bool down)
{
	robot->setPenState(down);
	penDown = down;
	idle = false;
	penDirty = true;
	emit penStateChanged(down);
}

void RobotController::setPenColor(double r, double g, double b)
{
	penColor[0] = static_cast<float>(r);
	penColor[1] = static_cast<float>(g);
	penColor[2] = static_cast<float>(b);
	penColor[3] = 1;
	idle = false;
	penDirty = true;
	emit penColorChanged(r,g,b);
}

void RobotController::setX(double value)
{
	targetPosition[0] =
			qMin(static_cast<double>(fieldSize),
				 qMax(-static_cast<double>(fieldSize),
					  value));
	idle = false;
}

void RobotController::setY(double value)
{
	targetPosition[1] =
			qMin(static_cast<double>(fieldSize),
				 qMax(-static_cast<double>(fieldSize),
					  value));
	idle = false;
}

void RobotController::setAngle(double value)
{
	targetAngle = normalizeAngle(qMin(0.5, qMax(-0.5, value)));
	idle = false;
}

void RobotController::move(double distance)
{
	//Calculate final coordinates
	targetPosition[0] = currentPosition[0] + distance * cos(2*pi*currentAngle);
	targetPosition[1] = currentPosition[1] + distance * sin(2*pi*currentAngle);

	//Wrap to first quadrant
	double x = fabs(targetPosition[0]);
	double y = fabs(targetPosition[1]);

	if ( (x > fieldSize) || (y > fieldSize))
	{
		const double oX = fabs(currentPosition[0]);
		const double oY = fabs(currentPosition[1]);

		//Calculate the line equation
		const double slope = (y - oY) / (x - oX);
		const double offset = y - slope * x;

		//Make sure we do not go out of bounds
		if (x > y)
		{
			//The position is bounded at the wall passing through the x axis
			//Thus we need to calculate the y value at that point
			x = fieldSize;
			y = offset + slope * fieldSize;
		}
		else
		{
			//The position is bounded at the wall passing through the y axis
			//Thus we need to calculate the x value at that point
			x = (fieldSize - offset) / slope;
			y = fieldSize;
		}

		targetPosition[0] = std::signbit(targetPosition[0]) ? -x : x;
		targetPosition[1] = std::signbit(targetPosition[1]) ? -y : y;
	}
	idle = false;

	emit targetStateChanged(
			targetPosition[0],
			targetPosition[1],
			targetAngle);

	emit relativeMoveChanged(distance);
}

void RobotController::rotate(double angle)
{
	targetAngle = normalizeAngle(targetAngle + angle);
	idle = false;

	emit targetStateChanged(
			targetPosition[0],
			targetPosition[1],
			targetAngle);

	emit relativeRotateChanged(angle);
}

void RobotController::log(QString text)
{
	emit logAdded(text);
	emit ideling();
}

double RobotController::normalizeAngle(double angle)
{
	return fmod(angle + 1.5, 1.0) - 0.5;
}
