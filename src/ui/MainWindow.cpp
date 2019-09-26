#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <vector>

#include <QColorDialog>
#include <QFileDialog>
#include <QStandardPaths>
#include <QMessageBox>
#include <QImageWriter>
#include <QInputDialog>

#include <osg/ShapeDrawable>
#include <osg/StateSet>
#include <osg/Material>
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/MatrixTransform>
#include <osg/PolygonMode>
#include <osg/Fog>
#include <osgFX/Cartoon>
#include <osg/LightSource>
#include <osg/Light>

#include <osgDB/ReadFile>
#include <osgUtil/SmoothingVisitor>
#include <osgGA/NodeTrackerManipulator>

#include "QtOSGWidget.h"
#include "QtOSGMouseHandler.h"
#include "RobotController.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	timer(new QTimer(this)),
	robotController(new RobotController(frameRate, this)),
	brainController(robotController, this)
{
	ui->setupUi(this);
	ui->actionLog_robot->setChecked(true);
	connect(ui->actionClear_field, &QAction::triggered, robotController, &RobotController::clear);


	//UI to robot signals
	//-------------------

	connect(ui->penDown, &QCheckBox::clicked, robotController, &RobotController::setPenDown);

	connect(ui->setX, &QDoubleSpinBox::editingFinished, [this]()
	{ if (ui->setX->hasFocus()) robotController->setX(ui->setX->value()); });

	connect(ui->setY, &QDoubleSpinBox::editingFinished, [this]()
	{ if (ui->setY->hasFocus()) robotController->setY(ui->setY->value()); });

	connect(ui->setAngle, &QDoubleSpinBox::editingFinished, [this]()
	{ if (ui->setAngle->hasFocus()) robotController->setAngle(ui->setAngle->value()); });


	connect(ui->relativeDistance, &QDoubleSpinBox::editingFinished, [this]()
	{ if (ui->relativeDistance->hasFocus()) robotController->move(ui->relativeDistance->value()); });

	connect(ui->relativeAngle, &QDoubleSpinBox::editingFinished, [this]()
	{ if (ui->relativeAngle->hasFocus()) robotController->rotate(ui->relativeAngle->value()); });

	auto setColor = [this]()
	{
		robotController->setPenColor(
					ui->red->value(),
					ui->green->value(),
					ui->blue->value());
	};

	connect(ui->red, &QDoubleSpinBox::editingFinished, setColor);
	connect(ui->green, &QDoubleSpinBox::editingFinished, setColor);
	connect(ui->blue, &QDoubleSpinBox::editingFinished, setColor);

	connect(ui->selectColor, &QPushButton::clicked, [this, setColor]()
	{
		QColor color(QColor::fromRgbF(
						 ui->red->value(),
						 ui->green->value(),
						 ui->blue->value()));

		color = QColorDialog::getColor(color, this, "Select Color");
		if (!color.isValid())
			return;

		ui->red->setValue(color.redF());
		ui->green->setValue(color.greenF());
		ui->blue->setValue(color.blueF());
		setColor();
	});

	connect(ui->singleStep, &QCheckBox::toggled, robotController, &RobotController::setSingleStep);
	connect(ui->Continue, &QPushButton::clicked, robotController, &RobotController::continueStep);



	//Robot to UI signals
	//-------------------

	connect(robotController, &RobotController::penStateChanged, [this](bool down)
	{
		ui->penDown->setChecked(down);

		if (!logrobot())
			return;

		if (down)
			ui->log->append("<i>Pen down</i>");
		else
			ui->log->append("<i>Pen up</i>");
	});

	connect(
				robotController,
				&RobotController::penColorChanged,
				[this](double r, double g, double b)
	{
		ui->red->setValue(r);
		ui->green->setValue(g);
		ui->blue->setValue(b);

		if (!logrobot())
			return;

		ui->log->append(QString("<i>Set color: %1 %2 %3</i>").arg(r).arg(g).arg(b));
	});

	connect(robotController, &RobotController::relativeMoveChanged, [this](double distance)
	{
		ui->relativeDistance->setValue(distance);

		if (!logrobot())
			return;

		ui->log->append(QString("<i>Set distance: %1</i>").arg(distance));
	});

	connect(robotController, &RobotController::relativeRotateChanged, [this](double angle)
	{
		ui->relativeAngle->setValue(angle);

		if (!logrobot())
			return;

		ui->log->append(QString("<i>Set rotation: %1</i>").arg(angle));
	});

	connect(robotController, &RobotController::logAdded, ui->log, &QTextBrowser::append);


	connect(
				robotController,
				&RobotController::currentStateChanged,
				[this](double x, double y, double angle)
	{
		ui->currentX->setValue(x);
		ui->currentY->setValue(y);
		ui->currentAngle->setValue(angle);
	});

	connect(
				robotController,
				&RobotController::targetStateChanged,
				[this](double x, double y, double angle)
	{
		ui->setX->setValue(x);
		ui->setY->setValue(y);
		ui->setAngle->setValue(angle);
	});

	connect(robotController, &RobotController::fieldSizeChanged, [this](size_t _size)
	{
		auto size = static_cast<int>(_size);
		ui->setX->setRange(-size, size);
		ui->setY->setRange(-size, size);
		ui->currentX->setRange(-size, size);
		ui->currentY->setRange(-size, size);

		ui->relativeDistance->setRange(-2*size*sqrt(2), 2*size*sqrt(2));

		ui->image->setImage(robotController->getImage());
	});

	connect(robotController, &RobotController::imageChanged, [this](){ui->image->update();});
	connect(robotController, &RobotController::stoppedOnStep, ui->Continue, &QPushButton::setEnabled);


	//UI to brain signals
	//-------------------
	connect(ui->start, &QPushButton::clicked, this, &MainWindow::run);
	connect(ui->stop, &QPushButton::clicked, this, &MainWindow::stop);

	connect(this, &MainWindow::run, &brainController, &ThreadedBrainController::run);
	connect(this, &MainWindow::stop, &brainController, &ThreadedBrainController::stop);
	connect(this, &MainWindow::stop, robotController, &RobotController::stop);

	//Brain to UI signals
	//-------------------
	connect(&brainController, &ThreadedBrainController::started, this, &MainWindow::started);
	connect(&brainController, &ThreadedBrainController::stopped, this, &MainWindow::stopped);


	//Data input to brain
	//-------------------

	connect(robotController, &RobotController::getInteger, [this](int input, QString title, QString label)
	{
		int reply;
		bool ok;

		reply = QInputDialog::getInt(
					this,title,label,input,
					-2147483647,2147483647,1,
					&ok);

		robotController->newInputInteger(reply, ok);
	});

	connect(robotController, &RobotController::getDouble, [this](double input, QString title, QString label)
	{
		double reply;
		bool ok;

		reply = QInputDialog::getDouble(
					this,title,label,input,
					-2147483647,2147483647,1,
					&ok);

		robotController->newInputDouble(reply, ok);
	});

	connect(robotController, &RobotController::getString, [this](QString input, QString title, QString label)
	{
		QString reply;
		bool ok;

		reply = QInputDialog::getText(
					this,title,label, QLineEdit::Normal,
					input,&ok);

		robotController->newInputString(reply, ok);
	});


	//Initialization
	//--------------

	robotController->resetField(fieldSize);
	robotController->emitStatus();

//	osg::ref_ptr<osg::Node> root = setupScene(robotController->getNode());
	setupViews(world.scene().root());

	connect(timer, &QTimer::timeout, this, &MainWindow::frame);
	timer->start(static_cast<int>(1000.0 / frameRate));
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::frame()
{
	robotController->step();
	ui->followView->update();
	ui->image->update();
}

void MainWindow::setupViews(osg::ref_ptr<osg::Node> node)
{
	setupFollowView(node);
}

void MainWindow::setupFollowView(osg::ref_ptr<osg::Node> node)
{
	osg::ref_ptr<osgGA::NodeTrackerManipulator> manipulator = new osgGA::NodeTrackerManipulator;
	manipulator->setRotationMode(osgGA::NodeTrackerManipulator::ELEVATION_AZIM);
	manipulator->setTrackNode(world.mainActor().robotRoot());

	ui->followView->getViewer()->setCameraManipulator(manipulator);
	new QtOSGMouseHandler(ui->followView);

	ui->followView->getViewer()->getCameraManipulator()->setHomePosition(
	{-6,0,4},
	{0,0,0},
	{0,0,1});

	double aspectRatio2 = ui->followView->width();
	aspectRatio2 /= ui->followView->height();
	ui->followView->getCamera()->setProjectionMatrixAsPerspective( 45, aspectRatio2, 0.1, 1000 );

	//This should be after all the camera setup
	ui->followView->setSceneData(node);
}

void MainWindow::started()
{
	ui->log->append(QString("<b>Started</b>"));

	ui->start->setEnabled(false);
	ui->stop->setEnabled(true);
}

void MainWindow::stopped()
{
	ui->log->append(QString("<b>Stopped</b>"));

	ui->start->setEnabled(true);
	ui->stop->setEnabled(false);
}

void MainWindow::on_actionSave_as_triggered()
{
	auto list = QImageWriter::supportedImageFormats();
	QString filetypes = "Images (";

	for (auto i : list)
	{
		filetypes.append("*.");
		filetypes.append(i);
		filetypes.append(" ");
	}
	filetypes.append(")");

	QFile file;
	file.setFileName(
				QFileDialog::getSaveFileName(
					this,
					tr("Select save file"),
					QStandardPaths::writableLocation(QStandardPaths::PicturesLocation),
					filetypes)
				);

	if (file.fileName().isEmpty())
		return;

	if (!robotController->getImage()->save(file.fileName()))
		QMessageBox::critical(this,tr("Error opening file"),tr("The file could not be opened for writing."));
}

void MainWindow::on_actionClear_log_triggered()
{
	ui->log->clear();
}

void MainWindow::on_actionExit_triggered()
{
	brainController.stop();
	close();
}

void MainWindow::on_actionReset_triggered()
{
	ui->penDown->setChecked(false);
	ui->red->setValue(1);
	ui->green->setValue(1);
	ui->blue->setValue(1);

	ui->setX->setValue(0);
	ui->setY->setValue(0);
	ui->setAngle->setValue(0);

	robotController->setPenDown(false);
	robotController->setPenColor(1,1,1);
	robotController->setX(0);
	robotController->setY(0);
	robotController->setAngle(0);
}

void MainWindow::on_actionLinear_speed_triggered()
{
	robotController->setLinearSpeed(
				QInputDialog::getDouble(
					this,
					"Linear speed",
					"The speed, in units per second",
					robotController->getLinearSpeed(),
					0, 100, 3));
}

void MainWindow::on_actionRotation_speed_triggered()
{
	robotController->setRotationSpeed(
				QInputDialog::getDouble(
					this,
					"Rotation speed",
					"The speed, in circles per second",
					robotController->getRotationSpeed(),
					0, 100, 3));
}

bool MainWindow::logrobot()
{
	return ui->actionLog_robot->isChecked();
}
