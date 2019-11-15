#include "mainform.h"
#include <QtWidgets/QApplication>
#include <QFontDatabase>
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QFontDatabase::addApplicationFont(":/main/font/PoetsenOne-Regular.ttf");
	mainForm w;
	w.show();
	return a.exec();
}
