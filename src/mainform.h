#ifndef MAINFORM_H
#define MAINFORM_H

#include <QtWidgets/QMainWindow>
#include <QTranslator>
#include <QSettings>
#include "ui_mainform.h"

class mainForm : public QMainWindow
{
	Q_OBJECT

private slots:
	void loadLanguage();
    void createMulticlient();
	void startClient();
    void removeMulticlient();
    void shortcutMulticlient();
    void createCustomThinMulticlient();
    void createCustomMulticlient();
	void checkIPPort();
	void portChange(int index);
    void folderSelection();

public:
	explicit mainForm(QWidget *parent = 0);
	~mainForm();

private:
    Ui::mainWindow ui;
    int port;
	QTranslator m_translator;
	QSettings *settings;
    QString nosPath;
	QString nosVersion;
	QString multiVersion;
    QString ipRange;
    QRegExp ipRegex;
	void loadLanguage(const QString& rLanguage);
	void loadVersions();

    struct Version
    {
        explicit Version(QString versionStr)
        {
            sscanf(versionStr.toStdString().c_str(), "%d.%d.%d.%d", &major, &minor, &revision, &build);
        }
        bool operator<(const Version &otherVersion)
        {
            if (major < otherVersion.major)
                return true;
            else if (major > otherVersion.major)
                return false;
            if (minor < otherVersion.minor)
                return true;
            else if (minor > otherVersion.minor)
                return false;
            if (revision < otherVersion.revision)
                return true;
            else if (revision > otherVersion.revision)
                return false;
            if (build < otherVersion.build)
                return true;
            return false;
        }
        int major, minor, revision, build;
    };
};

#endif // MAINFORM_H
