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
	timer(new QTimer(this)),
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

	//UI to robot signals
	//-------------------

	connect(ui->penDown, &QCheckBox::clicked, actor, &TurtleActorController::setPenDown);

	connect(ui->setX, &QDoubleSpinBox::editingFinished, [this]()
	{ if (ui->setX->hasFocus()) actor->setTargetPositionX(ui->setX->value()); });

	connect(ui->setY, &QDoubleSpinBox::editingFinished, [this]()
	{ if (ui->setY->hasFocus()) actor->setTargetPositionY(ui->setY->value()); });

	connect(ui->setAngle, &QDoubleSpinBox::editingFinished, [this]()
	{ if (ui->setAngle->hasFocus()) actor->setTargetAngle(ui->setAngle->value()); });


	connect(ui->relativeDistance, &QDoubleSpinBox::editingFinished, [this]()
	{ if (ui->relativeDistance->hasFocus()) actor->setMove(ui->relativeDistance->value()); });

	connect(ui->relativeAngle, &QDoubleSpinBox::editingFinished, [this]()
	{ if (ui->relativeAngle->hasFocus()) actor->setRotate(ui->relativeAngle->value()); });

	auto setColor = [this]()
	{
		actor->setPenColor(
					QColor::fromRgbF(
						ui->red->value(),
						ui->green->value(),
						ui->blue->value()));
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

	connect(ui->singleStep, &QCheckBox::toggled, actor, &TurtleActorController::setSingleStep);
	connect(ui->Continue, &QPushButton::clicked, actor, &TurtleActorController::continueSingleStep);



	//Robot to UI signals
	//-------------------
	connect(actor, &TurtleActorController::newState, this, &MainWindow::newState);
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
	connect(brain, &ThreadedBrainController::log, ui->log, &QTextBrowser::append);

	//Initialization
	//--------------
	resize();

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
	world();

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

void MainWindow::newState(TurtleActor::State state, TurtleActor::CallbackType action)
{
	ui->penDown->setChecked(state.pen.down);
	ui->red->setValue(state.pen.color.redF());
	ui->green->setValue(state.pen.color.greenF());
	ui->blue->setValue(state.pen.color.blueF());

	ui->currentX->setValue(state.current.position.x());
	ui->currentY->setValue(state.current.position.y());
	ui->currentAngle->setValue(state.current.angle);

	ui->setX->setValue(state.target.position.x());
	ui->setY->setValue(state.target.position.y());
	ui->setAngle->setValue(state.target.angle);

	ui->relativeDistance->setValue(state.relative.distance);
	ui->relativeAngle->setValue(state.relative.angle);

	if (!logrobot())
		return;

	switch (action)
	{
		case TurtleActor::CallbackType::Pen:
			ui->log->append(
						QString("<i>Pen %1, Red %2, Green %3, Blue %4</i>")
						.arg(state.pen.down ? "down" : "up")
						.arg(state.pen.color.redF())
						.arg(state.pen.color.greenF())
						.arg(state.pen.color.blueF()));
			break;

		case TurtleActor::CallbackType::Move:
			ui->log->append(QString("<i>Set distance: %1</i>").arg(state.relative.distance));
			break;

		case TurtleActor::CallbackType::Rotate:
			ui->log->append(QString("<i>Set rotation: %1</i>").arg(state.relative.angle));
			break;

		default: ;
	}
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
					0, 100, 3));
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
