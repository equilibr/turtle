#include "ThreadedBrain.h"

#include <QInputDialog>

#include "main.h"

int ThreadedBrain::getInteger(QString title, QString label, int input, bool * ok)
{
	return QInputDialog::getInt(
				nullptr,title,label,input,
				-2147483647,2147483647,1,
				ok);
}

double ThreadedBrain::getDouble(QString title, QString label, double input, bool * ok)
{
	return QInputDialog::getDouble(
				nullptr,title,label,input,
				-2147483647,2147483647,1,
				ok);
}

QString ThreadedBrain::getString(QString title, QString label, QString input, bool * ok)
{
	return QInputDialog::getText(
				nullptr,title,label, QLineEdit::Normal,
				input,ok);
}

void ThreadedBrain::log(QString text)
{
	emit signalLog(text);
}

void ThreadedBrain::setTargetPosition(Turtle::Position2D target)
{
	emit signalTargetPosition(target);
	waitForActive();
}

void ThreadedBrain::setTargetAngle(double target)
{
	emit signalTargetAngle(target);
	waitForActive();
}

void ThreadedBrain::setPenColor(QColor color)
{
	emit signalPenColor(color);
	waitForActive();
}

void ThreadedBrain::setPenColor(double red, double green, double blue)
{
	setPenColor(QColor::fromRgbF(red, green, blue));
}

void ThreadedBrain::setPenColor(double hue, double saturation)
{
	setPenColor(QColor::fromHsvF(hue, saturation, 1.0));
}

void ThreadedBrain::setPenDown(bool down)
{
	emit signalPenDown(down);
	waitForActive();
}

void ThreadedBrain::move(double distance)
{
	emit signalMove(distance);
	waitForActive();
}

void ThreadedBrain::rotate(double angle)
{
	emit signalRotate(angle);
	waitForActive();
}

void ThreadedBrain::start()
{
	setActive(true);
	emit started();

	//Invoke the run slot from the main event loop
	QMetaObject::invokeMethod(this, &ThreadedBrain::run, Qt::QueuedConnection);
}

void ThreadedBrain::stop()
{
	setActive(false);
	idleEventLoop.exit();
}

void ThreadedBrain::newRunState(bool active)
{
	if (active)
		idleEventLoop.quit();
}

void ThreadedBrain::run()
{
	//Main execution function
	MainBrain(*this);

	setActive(false);
	emit stopped();
}

void ThreadedBrain::waitForActive()
{
	if (*this)
		idleEventLoop.exec();
}
