#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QPointer>
#include <QMainWindow>
#include <QTimer>
#include <QColor>
#include <QGraphicsScene>

#include "World.h"
#include "TurtleActorController.h"
#include "ThreadedBrainController.h"

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = nullptr);
	~MainWindow() override;

protected:
	size_t fieldSize = 50.0;
	constexpr static double frameRate = 25.0;
	void frame();

	void setupViews(osg::ref_ptr<osg::Node> node);
	void setupFollowView(osg::ref_ptr<osg::Node> node);

signals:
	void run();
	void stop();

protected slots:
	void resize();
	void reset();
	void started();
	void stopped();

private slots:
	void newState(TurtleActor::State state, TurtleActor::CallbackType action);

	void on_actionSave_as_triggered();
	void on_actionClear_log_triggered();
	void on_actionExit_triggered();
	void on_actionResize_triggered();
	void on_actionReset_triggered();
	void on_actionLinear_speed_triggered();
	void on_actionRotation_speed_triggered();



private:
	bool logrobot();

	Ui::MainWindow *ui;
	QTimer * timer;

	Turtle::World world;
	QPointer<TurtleActorController> actor;
	QPointer<ThreadedBrainController> brain;
};

#endif // MAINWINDOW_H
