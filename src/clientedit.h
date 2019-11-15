#pragma once
#include <string>
#include <vector>
#include <QString>

class clientEditor
{
public:
	static bool editFile(QString path, QString name, QString IP, int portByte);
private:
    static void changeIP(FILE* file, QString IP);
	static void changePort(FILE* file, int Port);
	static void changeLength(FILE* file, int length, long offset);
    static long findBytes(FILE* inFile, std::vector<unsigned char> searched, long offset);
	static void writeBytes(FILE* file, const char* toWrite, long offset);
	static void writeBytes(FILE* file, char* toWrite, long offset);
    static void fillBytes(FILE* file, long offset, long length);
};
