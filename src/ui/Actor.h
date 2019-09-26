#ifndef ACTOR_H
#define ACTOR_H

#include <QString>

namespace Turtle
{
	//A base class for world actors
	class Actor
	{
	public:
		Actor(QString name = {}) : m_name{name} {}
		virtual ~Actor();

		//Execute a simulated actor step(s)
		//Returns whether something changed
		virtual bool operator()(int steps = 1) { Q_UNUSED(steps) return false;}

		//The actor name
		virtual const QString name() const { return m_name;}
		explicit operator const QString() { return name();}

	protected:
		QString m_name;
	};

}
#endif // ACTOR_H
