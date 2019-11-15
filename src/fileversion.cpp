#include "fileversion.h"

#include <ShlObj.h>
#pragma comment(lib, "version.lib")

QString FileVersion::GetVersion(QString path) {
    char ver[50];

    DWORD dwHandle, sz = GetFileVersionInfoSizeA(path.toStdString().c_str(), &dwHandle);
    char *buf = new char[sz];
    if (!GetFileVersionInfoA(path.toStdString().c_str(), dwHandle, sz, &buf[0]))
    {
        delete[] buf;
        return "1.0.0.0";
    }
    VS_FIXEDFILEINFO * pvi;
    sz = sizeof(VS_FIXEDFILEINFO);
    if (!VerQueryValueA(&buf[0], "\\", (LPVOID*)&pvi, (unsigned int*)&sz))
    {
        delete[] buf;
        return "1.0.0.0";
    }
    sprintf(ver, "%d.%d.%d.%d"
        , pvi->dwProductVersionMS >> 16
        , pvi->dwFileVersionMS & 0xFFFF
        , pvi->dwFileVersionLS >> 16
        , pvi->dwFileVersionLS & 0xFFFF
    );
    delete[] buf;

    return QString(ver);
}
