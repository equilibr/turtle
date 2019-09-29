QT += core gui
QT += widgets opengl

TARGET = turtle
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

INCLUDEPATH += ui

win32: {
#Change this to point to where osg is installed
INCLUDEPATH += ../../libraries/OpenSceneGraph/install/include
LIBS += -L../../libraries/OpenSceneGraph/install/lib
}

LIBS += -lOpenThreads
LIBS += -losg
LIBS += -losgViewer
LIBS += -losgGA
LIBS += -losgDB
LIBS += -losgUtil
LIBS += -losgFX


DESTDIR = ../bin

SOURCES += \
		algorithms/Checker.cpp \
		algorithms/adder.cpp \
		algorithms/draw.cpp \
		algorithms/follow.cpp \
		algorithms/utility.cpp \
		ui/Actor.cpp \
		ui/Command.cpp \
		ui/ImageDisplay.cpp \
		ui/MainEntryPoint.cpp \
		ui/MainWindow.cpp \
		ui/QtOSGMouseHandler.cpp \
		ui/QtOSGWidget.cpp \
		ui/Robot.cpp \
		ui/Scene.cpp \
		ui/ThreadedBrain.cpp \
		ui/ThreadedBrainController.cpp \
		main.cpp \
		ui/TileSensor.cpp \
		ui/TiledFloor.cpp \
		ui/TurtleActor.cpp \
		ui/TurtleActorController.cpp \
		ui/World.cpp

HEADERS += \
	algorithms/Checker.h \
	algorithms/utility.h \
	ui/Actor.h \
	ui/Command.h \
	ui/ImageDisplay.h \
	ui/MainWindow.h \
	ui/QtOSGMouseHandler.h \
	ui/QtOSGWidget.h \
	ui/Robot.h \
	ui/Scene.h \
	ui/ThreadedBrain.h \
	ui/ThreadedBrainController.h \
	main.h \
	ui/TileSensor.h \
	ui/TiledFloor.h \
	ui/TurtleActor.h \
	ui/TurtleActorController.h \
	ui/Types.h \
	ui/World.h

FORMS += \
	ui/MainWindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
