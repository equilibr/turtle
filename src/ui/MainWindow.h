#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QColor>
#include <QGraphicsScene>

#include "RobotController.h"
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
	constexpr static size_t fieldSize = 50.0;
	constexpr static double frameRate = 25.0;
	void frame();

	void setupViews(osg::ref_ptr<osg::Node> node);
	void setupFollowView(osg::ref_ptr<osg::Node> node);

signals:
	void run();
	void stop();

protected slots:
	void started();
	void stopped();

private slots:
	void on_actionSave_as_triggered();
	void on_actionClear_log_triggered();
	void on_actionExit_triggered();
	void on_actionReset_triggered();
	void on_actionLinear_speed_triggered();
	void on_actionRotation_speed_triggered();

private:
	bool logrobot();

	Ui::MainWindow *ui;
	QTimer * timer;

	RobotController * robotController;
	ThreadedBrainController brainController;
};

#endif // MAINWINDOW_H
