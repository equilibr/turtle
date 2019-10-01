#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <vector>

#include <QColorDialog>
#include <QFileDialog>
#include <QStandardPaths>
#include <QMessageBox>
#include <QImageWriter>
#include <QInputDialog>

#include <osgGA/NodeTrackerManipulator>

#include "QtOSGWidget.h"
#include "QtOSGMouseHandler.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	stepTimer(new QTimer(this)),
	frameTimer(new QTimer(this)),
	world{},
	actor{new TurtleActorController(world.mainActor(), this)},
	brain{new ThreadedBrainController(actor, this)}
{
	ui->setupUi(this);
	QTimer::singleShot(0,[this]()
	{
		QList<int> sizes = ui->splitterMiddle->sizes();
		int heightQuanta = (sizes[0] + sizes[1]) / 3;
		ui->splitterMiddle->setSizes({2*heightQuanta,1*heightQuanta});

		sizes = ui->splitterBottom->sizes();
		heightQuanta = (sizes[0] + sizes[1]) / 3;
		ui->splitterBottom->setSizes({2*heightQuanta,1*heightQuanta});
	});

	connect(ui->selectColor, &QPushButton::clicked, [this]()
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
	});

	connect(ui->singleStep, &QCheckBox::toggled, actor, &TurtleActorController::setSingleStep);
	connect(ui->Continue, &QPushButton::clicked, actor, &TurtleActorController::continueSingleStep);



	//Robot to UI signals
	//-------------------
	connect(actor, &TurtleActorController::log, ui->log, &QTextBrowser::append);
	connect(actor, &TurtleActorController::newCurrentState, this, &MainWindow::newCurrentState);
	connect(actor, &TurtleActorController::newRunState,
			[this](bool active) {ui->Continue->setEnabled(!active);});


	//UI to brain signals
	//-------------------
	connect(ui->start, &QPushButton::clicked, this, &MainWindow::run);
	connect(ui->stop, &QPushButton::clicked, this, &MainWindow::stop);

	connect(this, &MainWindow::run, brain, &ThreadedBrainController::start);
	connect(this, &MainWindow::stop, brain, &ThreadedBrainController::stop);

	//Brain to UI signals
	//-------------------
	connect(brain, &ThreadedBrainController::started, this, &MainWindow::started);
	connect(brain, &ThreadedBrainController::stopped, this, &MainWindow::stopped);


	//Initialization
	//--------------
	resize();

	setupViews(world.scene().root());

	connect(frameTimer, &QTimer::timeout, this, &MainWindow::frame);
	frameTimer->start(static_cast<int>(1000.0 / frameRate));

	connect(stepTimer, &QTimer::timeout, [this]{world();});
	stepTimer->start(10);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::frame()
{
	ui->followView->update();
	ui->image->update();
	ui->tileSensor->update();
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
	{-20,0,10},
	{0,0,0},
	{0,0,1});

	double aspectRatio2 = ui->followView->width();
	aspectRatio2 /= ui->followView->height();
	ui->followView->getCamera()->setProjectionMatrixAsPerspective( 45, aspectRatio2, 0.1, 1000 );

	//This should be after all the camera setup
	ui->followView->setSceneData(node);
}

void MainWindow::resize()
{
	world.resize({fieldSize, fieldSize});
	world.reset();
	actor->reset();

	auto size = static_cast<int>(fieldSize);
	ui->setX->setRange(-size, size);
	ui->setY->setRange(-size, size);
	ui->currentX->setRange(-size, size);
	ui->currentY->setRange(-size, size);

	ui->relativeDistance->setRange(-2*size*sqrt(2), 2*size*sqrt(2));

	ui->image->setImage(&world.floor().image());
	ui->tileSensor->setImage(&world.mainActor().tileSensorImage());
}

void MainWindow::reset()
{
	world.reset();
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

void MainWindow::newCurrentState(const Command & data)
{
	if (!data.valid)
		return;
	if (data.destination != Command::Destination::Turtle)
		return;
	if (data.data.turtle.command != Command::Turtle::Command::Get)
		return;
	if (data.data.turtle.target != Command::Turtle::Target::Current)
		return

	ui->penDown->setChecked(data.data.turtle.penDown);
	ui->red->setValue(data.data.turtle.color.redF());
	ui->green->setValue(data.data.turtle.color.greenF());
	ui->blue->setValue(data.data.turtle.color.blueF());

	ui->currentX->setValue(data.data.turtle.position.x());
	ui->currentY->setValue(data.data.turtle.position.y());
	ui->currentAngle->setValue(data.data.turtle.angle);
}

void MainWindow::on_actionLoad_triggered()
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
				QFileDialog::getOpenFileName(
					this,
					tr("Select load file"),
					QStandardPaths::writableLocation(QStandardPaths::PicturesLocation),
					filetypes)
				);

	if (file.fileName().isEmpty())
		return;

	world.setImage(QImage{file.fileName()});
	ui->image->setImage(&world.floor().image());
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

	if (!world.floor().image().save(file.fileName()))
		QMessageBox::critical(this,tr("Error opening file"),tr("The file could not be opened for writing."));
}

void MainWindow::on_actionClear_log_triggered()
{
	ui->log->clear();
}

void MainWindow::on_actionExit_triggered()
{
	brain->stop();
	close();
}

void MainWindow::on_actionResize_triggered()
{
	bool ok;
	int newSize = static_cast<int>(fieldSize);

	newSize =
			QInputDialog::getInt(
					this,
					"Half size",
					"Half-size of the field, in tiles",
					newSize,
					0, 1000, 1, &ok);

	if (!ok)
		return;

	fieldSize = static_cast<size_t>(newSize);
	resize();
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

	reset();
}

void MainWindow::on_actionLinear_speed_triggered()
{
	actor->setLinearSpeed(
				QInputDialog::getDouble(
					this,
					"Linear speed",
					"The speed, in units per second",
					actor->linearSpeed(),
					0, 1e6, 3));
}

void MainWindow::on_actionRotation_speed_triggered()
{
	actor->setRotationSpeed(
				QInputDialog::getDouble(
					this,
					"Rotation speed",
					"The speed, in circles per second",
					actor->rotationSpeed(),
					0, 100, 3));
}

bool MainWindow::logrobot()
{
	return ui->actionLog_robot->isChecked();
}

void MainWindow::on_actionClear_field_triggered()
{
	world.reset();
}
