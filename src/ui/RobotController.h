#ifndef ROBOTCONTROLLER_H
#define ROBOTCONTROLLER_H

#include <QObject>
#include <QImage>

#include "Robot.h"
#include "Floor.h"

#include <osg/Group>
#include <osg/MatrixTransform>

class RobotController : public QObject
{
	Q_OBJECT
public:
	RobotController(double frameRate, QObject *parent = nullptr);

	void step();

	osg::ref_ptr<osg::Group> getNode() const { return root; }
	osg::ref_ptr<Robot> getRobot() const { return robot; }
	const QImage * getImage() const {return &image; }

	double getLinearSpeed() const { return linearSpeed; }
	double getRotationSpeed() const { return rotationSpeed; }
public slots:
	void clear();
	void stop() { idle = true; }

	void setLinearSpeed(double speed) { linearSpeed = speed; }
	void setRotationSpeed(double speed) { rotationSpeed = speed; }
	void setSingleStep(bool enable) { singleStep = enable; }
	void continueStep();
	void emitStatus();
	void resetField(size_t size);

	//Requests from the robot
	//-----------------------

	void setPenDown(bool down);
	void setPenColor(double r, double g, double b);
	void setX(double value);
	void setY(double value);
	void setAngle(double value);

	void move(double distance);
	void rotate(double angle);
	void log(QString text);

	//Replies from the UI
	//-------------------
	void newInputInteger(int input, bool ok)
	{
		emit setInputInteger(input, ok);
		emit ideling();
	}

	void newInputDouble(double input, bool ok)
	{
		emit setInputDouble(input, ok);
		emit ideling();
	}

	void newInputString(QString input, bool ok)
	{
		emit setInputString(input, ok);
		emit ideling();
	}


signals:
	//Notifications to the UI
	//-----------------------
	void penStateChanged(bool down);
	void penColorChanged(double r, double g, double b);
	void currentStateChanged(double x, double y, double angle);
	void targetStateChanged(double x, double y, double angle);
	void relativeMoveChanged(double distance);
	void relativeRotateChanged(double angle);
	void logAdded(QString text);

	void fieldSizeChanged(size_t size);
	void imageChanged();

	void stoppedOnStep(bool stopped);

	//Requests from the robot
	//-----------------------
	void getInteger(int input, QString title, QString label);
	void getDouble(double input, QString title, QString label);
	void getString(QString input, QString title, QString label);

	//Notifications to the robot
	//--------------------------
	void ideling();

	void setInputInteger(int input, bool ok);
	void setInputDouble(double input, bool ok);
	void setInputString(QString input, bool ok);

private:
	static const double pi;
	static double normalizeAngle(double angle);

	//Speed, in units/second
	double linearSpeed = 25.0;
	double rotationSpeed = 2.0;


	double frameRate;
	size_t fieldSize;
	double colorCycle;

	bool idle;
	bool singleStep;


	QImage image;

	osg::ref_ptr<Robot> robot;
	osg::ref_ptr<osg::MatrixTransform> transform;
	osg::ref_ptr<Floor> floor;
	osg::ref_ptr<osg::Group> root;

	bool penDown = false;
	osg::Vec4 penColor;

	osg::Vec3d currentPosition;
	double currentAngle;

	osg::Vec3d targetPosition;
	double targetAngle;

	bool penDirty = true;
	Floor::Position lastPenPosition;
};

#endif // ROBOTCONTROLLER_H
