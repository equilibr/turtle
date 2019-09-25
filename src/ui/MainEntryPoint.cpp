#include <QApplication>
#include "MainWindow.h"

int main(int argc, char** argv)
{
	QApplication qapp(argc, argv);

#	ifdef _WIN32
	//This MUST be executed before the main window is created, otherwise the software OpenGL
	//	rendered might be chosen and OSG will fail with obscure errors.
	//For reference, see:
	//	https://doc.qt.io/qt-5/windows-requirements.html#dynamically-loading-graphics-drivers
	QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
#	endif

	MainWindow w;
	w.showMaximized();

	//Run the Qt application.
	//By default the rendered OSG view will only be updated when the
	// camera manipulator processes a mouse event or when the widget is resized
	return qapp.exec();
}
