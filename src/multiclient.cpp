#include "multiclient.h"
#include <QFileInfo>
#include <vector>
#include <QMessageBox>
#include <QDesktopServices>
#include <QTranslator>
#include <ShlObj.h>
#include "patternsearch.h"
#include <sstream>

bool Multiclient::create(QString path, QString filename)
{
	if (filename.toStdString().find(".exe") != filename.size() - 4)
		filename += ".exe";
	QFile::remove(path + "/" + filename);
    if (getGraficInterface(path) == openGL)
		QFile::copy(path + "/NostaleClient.exe", path + "/" + filename);
	else
		QFile::copy(path + "/NostaleClientX.exe", path + "/" + filename);
    QString mP = path + "/" + filename;
    FILE *file = fopen(mP.toStdString().c_str(), "rb+");
	if (file == NULL)
		return false;

	int offset_start = PTSearch::FindPattern(file, "\x0F\x8C\x00\x00\x00\x00\x8D\x55\xDC\xB8\x00\x00\x00\x00\xE8\x00\x00\x00\x00", "xx????xxxx????x????");
	int offset_end = PTSearch::FindPattern(file, "\xE8\x00\x00\x00\x00\x0F\x85\x00\x00\x00\x00\x33\xC0\x55\x68\x00\x00\x00\x00", "x????xx????xxxx????") + 6;
	if (offset_start != -1 && offset_end != -1)
	{
		fseek(file, offset_start, SEEK_SET);
		int dist = offset_end - offset_start;
		unsigned char data[1] = { 0xE9 };
		fwrite(data, sizeof(unsigned char), sizeof(data), file);
		fwrite(&dist, sizeof(unsigned char), sizeof(dist), file);
		fclose(file);
		return true;
	}
	else
	{
		fclose(file);
		return false;
	}
}

bool Multiclient::createCustom(QString path, QString filename)
{
	if (filename.toStdString().find(".exe") != filename.size() - 4)
		filename += ".exe";
	QFile::remove(path + "/" + filename);
    if (getGraficInterface(path) == openGL)
		QFile::copy(path + "/NostaleClient.exe", path + "/" + filename);
	else
		QFile::copy(path + "/NostaleClientX.exe", path + "/" + filename);
    QString mP = path + "/" + filename;
    FILE *file = fopen(mP.toStdString().c_str(), "rb+");
	if (file == NULL)
		return false;

	int offset_start = PTSearch::FindPattern(file, "\x0F\x8C\x00\x00\x00\x00\x8D\x55\xDC\xB8\x00\x00\x00\x00\xE8\x00\x00\x00\x00", "xx????xxxx????x????");
	int offset_end = PTSearch::FindPattern(file, "\xA1\x00\x00\x00\x00\xC6\x00\x00\xA1\x00\x00\x00\x00\xC6\x00\x01\xEB\x46", "x????xxxx????xxxxx") - 5;
	if (offset_start != -1 && offset_end != -1)
	{
		fseek(file, offset_start, SEEK_SET);
		int dist = offset_end - offset_start;
		unsigned char data[1] = { 0xE9 };
		fwrite(data, sizeof(unsigned char), sizeof(data), file);
		fwrite(&dist, sizeof(unsigned char), sizeof(dist), file);
		fclose(file);
		return true;
	}
	else
	{
		fclose(file);
		return false;
	}
}

void Multiclient::start(QString path)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	CreateProcess(path.toStdWString().c_str(), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}

void Multiclient::remove(QString path)
{
	QFile::remove(path + "/Multiclient.exe");
	QString DeskDir = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
	QFile::remove(DeskDir + "/NosTale Multiclient.lnk");
}

Multiclient::graficInterface Multiclient::getGraficInterface(QString path)
{
    FILE *file = fopen((path + "\\Config.dat").toStdString().c_str(), "rb+");
	fseek(file, 0x10, SEEK_SET);
	unsigned char currend;
	fread(&currend, 1, 1, file);
	if (currend == 0x01)
	{
		fclose(file);
		return graficInterface::openGL;

	}
	else
	{
		fclose(file);
		return graficInterface::directX;
	}
}
