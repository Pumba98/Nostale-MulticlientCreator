#pragma once
#include <Windows.h>
#include <QString>

class Multiclient
{
public:
	static bool create(QString path, QString filename);
    static bool createCustom(QString path, QString filename);
	static void start(QString path);
	static void remove(QString path);
private:
    enum graficInterface {openGL, directX};
    static graficInterface getGraficInterface(QString path);
};
