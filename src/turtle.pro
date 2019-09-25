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
		ui/Floor.cpp \
		ui/ImageDisplay.cpp \
		ui/MainEntryPoint.cpp \
		ui/MainWindow.cpp \
		ui/QtOSGMouseHandler.cpp \
		ui/QtOSGWidget.cpp \
		ui/Robot.cpp \
		ui/RobotController.cpp \
		ui/ThreadedBrain.cpp \
		ui/ThreadedBrainController.cpp \
		main.cpp

HEADERS += \
	ui/Floor.h \
	ui/ImageDisplay.h \
	ui/MainWindow.h \
	ui/QtOSGMouseHandler.h \
	ui/QtOSGWidget.h \
	ui/Robot.h \
	ui/RobotController.h \
	ui/ThreadedBrain.h \
	ui/ThreadedBrainController.h \
	main.h

FORMS += \
	MainWindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
