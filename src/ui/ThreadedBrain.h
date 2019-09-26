#ifndef THREADEDBRAIN_H
#define THREADEDBRAIN_H

#include <QObject>
#include <QString>
#include <QColor>

//This object always lives in a separate thread
class ThreadedBrain : public QObject
{
	Q_OBJECT

public:
	explicit ThreadedBrain(QObject * parent = nullptr) : QObject(parent) {}

	operator bool() const { return active; }

	void setPenDown(bool down) { emit signalSetPenDown(down); }
	void setPenColor(double red, double green, double blue)  { emit signalSetPenColor(red, green, blue); }
	void setPenColor(double hue, double saturation)
	{
		auto color = QColor::fromHsvF(hue, saturation, 1.0);
		setPenColor(color.redF(), color.greenF(), color.blueF());
	}

	void move(double distance)  { emit signalMove(distance); }
	void rotate(double angle) { emit signalRotate(angle); }
	void log(QString text) { emit signalLog(text); }

	int getInteger(QString title = {}, QString label = {}, int input = 0, bool * ok = nullptr)
	{
		inputInteger = input;
		emit signalGetInteger(input, title, label);
		if (ok != nullptr)
			*ok = inputOK;

		return inputInteger;
	}

	double getDouble(QString title = {}, QString label = {}, double input = 0, bool * ok = nullptr)
	{
		inputDouble = input;
		emit signalGetDouble(input, title, label);
		if (ok != nullptr)
			*ok = inputOK;

		return inputDouble;
	}

	QString getString(QString title = {}, QString label = {}, QString input = {}, bool * ok = nullptr)
	{
		inputString = input;
		emit signalGetString(input, title, label);
		if (ok != nullptr)
			*ok = inputOK;

		return inputString;
	}


signals:
	void signalSetPenDown(bool down);
	void signalSetPenColor(double r, double g, double b);
	void signalMove(double distance);
	void signalRotate(double angle);
	void signalLog(QString text);

	void signalGetInteger(int input, QString title, QString label);
	void signalGetDouble(double input, QString title, QString label);
	void signalGetString(QString input, QString title, QString label);

	void started();
	void stopped();

public slots:
	void run();
	void stop();

	void setInputInteger(int input, bool ok)
	{
		inputInteger = input;
		inputOK = ok;
	}

	void setInputDouble(double input, bool ok)
	{
		inputDouble = input;
		inputOK = ok;
	}

	void setInputString(QString input, bool ok)
	{
		inputString = input;
		inputOK = ok;
	}


private:
	bool active;

	bool inputOK;
	int inputInteger;
	double inputDouble;
	QString inputString;
};

#endif // THREADEDBRAIN_H
