#include "clientedit.h"
#include "multiclient.h"
#include <fstream>
#include <QMessageBox>

bool clientEditor::editFile(QString path, QString name, QString IP, int portByte)
{
	if(name.toStdString().find(".exe") != name.size() - 4)
		name += ".exe";

	FILE *file = fopen((path + "/" + name).toStdString().c_str(), "rb+");
	if (file == NULL)
	{
		QMessageBox::information(NULL, "error", "Edit failed");
		return false;
	}
    changeIP(file, IP);
	changePort(file, portByte);
	fclose(file);
	return true;
}


void clientEditor::changeIP(FILE* file, QString IP)
{
	std::vector<unsigned char> ipSig = { 0x37, 0x39, 0x2e, 0x31, 0x31, 0x30, 0x2e };
    long offs = findBytes(file, ipSig, 0x025000);
	if (offs == -1)
		return;
    writeBytes(file, IP.toStdString().c_str(), offs);
	if (sizeof(IP) < 12)
		fillBytes(file, offs + IP.size(), 12 - IP.size());
	changeLength(file, IP.size(), offs - 4);
}

long findPattern(std::vector<unsigned char> sig, QString mask, FILE* inFile, long start)
{
	long index = start;
	fseek(inFile, start, SEEK_SET);
	unsigned char currend;
	while (!feof(inFile) && !ferror(inFile))
	{
		for (int i = 0; i < sig.size(); i++)
		{
			fread(&currend, 1, 1, inFile);
			if (currend == sig[i] || mask[i] != 'x')
			{
				if (i == sig.size() - 1)
				{
					return index;
				}
			}
			else
			{
				index += i + 1;
				break;
			}
		}
	}
	return -1;
}

void clientEditor::changePort(FILE* file, int Port)
{
    std::vector<unsigned char> portSig = { 0xC7, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00 };
    QString mask = "xx?????xxx";

    unsigned int f_port = Port;
	unsigned int back = f_port & 0xff;
	unsigned int front = (f_port >> 8) & 0xff;

	char* data1 = (char*)(front);
	char* data2 = (char*)(back);
	
	long offs = findPattern(portSig, mask, file, 0xB0000);
	while (offs != -1)
	{
		writeBytes(file, data2, offs + 6);
		writeBytes(file, data1, offs + 7);
        offs = findPattern(portSig, mask, file, offs+6);
	}
}


void clientEditor::changeLength(FILE* file, int length, long offset)
{
	char *data;
	fseek(file, offset, SEEK_SET);
	if (length > 10)
		data = (char*)length;
	else
		data = (char*)10;
	fwrite(&data, 1, 1, file);
}


long clientEditor::findBytes(FILE* inFile, std::vector<unsigned char> searched, long offset)
{
    fseek(inFile, offset, SEEK_SET);
	unsigned char currend;
	while (!feof(inFile) && !ferror(inFile))
	{
		for (int i = 0; i < searched.size(); i++)
		{
			fread(&currend, 1, 1, inFile);
			if (currend == searched[i])
			{
				if (i == searched.size() - 1)
				{
                    return offset;
				}
			}
			else
			{
                offset += i + 1;
				break;
			}
		}
	}
	return -1;
}


void clientEditor::writeBytes(FILE* file, const char* toWrite, long offset)
{
	fseek(file, offset, SEEK_SET);
	fwrite(toWrite, sizeof(toWrite), sizeof(toWrite), file);
}

void clientEditor::writeBytes(FILE* file, char* toWrite, long offset)
{
	fseek(file, offset, SEEK_SET);
	fwrite(&toWrite, 1, 1, file);
}


void clientEditor::fillBytes(FILE* file, long offset, long length)
{
	unsigned char *data = new unsigned char[length];
	for (int i = 0; i < length; i++)
		data[i] = 0x00;
	fseek(file, offset, SEEK_SET);
	fwrite(data, length, length, file);
	delete[] data;
}
