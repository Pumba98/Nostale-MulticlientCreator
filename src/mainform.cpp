#include "mainform.h"
#include <QFileDialog>
#include <QStandardPaths>
#include <QMessageBox>
#include <QInputDialog>
#include "multiclient.h"
#include "clientedit.h"
#include "fileversion.h"


mainForm::mainForm(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

    ipRange = "(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])";
    ipRegex = QRegExp("^" + ipRange
		+ "\\." + ipRange
		+ "\\." + ipRange
		+ "\\." + ipRange + "$");
    QRegExpValidator *ipValidator = new QRegExpValidator(ipRegex, this);
	ui.in_ip->setValidator(ipValidator);

	settings = new QSettings("Config.ini", QSettings::IniFormat);
    nosPath = settings->value("Path", "").toString();

	if (nosPath == "" || !QFile::exists(nosPath + "/NostaleClientX.exe"))
		folderSelection();

	loadLanguage(settings->value("Language", "EN").toString());
	loadVersions();

	bool exists = QFileInfo(nosPath + "/Multiclient.exe").exists();
	ui.btn_Start->setEnabled(exists);
	ui.btn_MulticlientLink->setEnabled(exists);
    ui.btn_Delete->setEnabled(exists);

	port = 0;

	connect(this->ui.in_ip, SIGNAL(textChanged(QString)), SLOT(checkIPPort()));
	connect(this->ui.in_filename, SIGNAL(textChanged(QString)), SLOT(checkIPPort()));
	connect(this->ui.cB_port, SIGNAL(activated(int)), SLOT(portChange(int)));
	connect(this->ui.btn_folderSelection, SIGNAL(clicked()), SLOT(folderSelection()));
	connect(this->ui.btn_createAdvanced, SIGNAL(clicked()), SLOT(createCustomMulticlient()));
	connect(this->ui.btn_createAdvancedGf, SIGNAL(clicked()), SLOT(createCustomThinMulticlient()));
	connect(this->ui.btn_Start, SIGNAL(clicked()), SLOT(startClient()));
	connect(this->ui.btn_MulticlientLink, SIGNAL(clicked()), SLOT(shortcutMulticlient()));
	connect(this->ui.btn_EN, SIGNAL(clicked()), SLOT(loadLanguage()));
	connect(this->ui.btn_DE, SIGNAL(clicked()), SLOT(loadLanguage()));
	connect(this->ui.btn_IT, SIGNAL(clicked()), SLOT(loadLanguage()));
	connect(this->ui.btn_Delete, SIGNAL(clicked()), SLOT(removeMulticlient()));
	connect(this->ui.btn_Create, SIGNAL(clicked()), SLOT(createMulticlient()));
}

mainForm::~mainForm()
{
	settings->sync();
}

void mainForm::folderSelection()
{
	QString DeskDir = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
	if (!QFile(DeskDir + "/NosTale.lnk").exists())
		DeskDir = "C:\\Users\\Public\\Desktop";
	QString possiblePath = QFileInfo(QFile(DeskDir + "/NosTale.lnk").symLinkTarget()).absolutePath();
	QMessageBox::information(NULL, "Setup", tr("Please select the NostaleClientX.exe in your NosTale Folder"));
	QString temp = QFileDialog::getOpenFileName(this, tr("Select NostaleClientX.exe"), possiblePath, tr("NostaleClientX.exe"), &tr("EXE (*.exe)"));
	if (temp != "")
	{
		nosPath = QFileInfo(temp).absolutePath();
		settings->setValue("Path", nosPath);
		loadVersions();
	}
	else if (temp == "" && (nosPath == "" || !QFile::exists(nosPath + "/NostaleClientX.exe")))
		exit(EXIT_SUCCESS);
}

void mainForm::loadVersions()
{
    nosVersion = FileVersion::GetVersion(nosPath + "/NostaleClientX.exe");
    multiVersion = FileVersion::GetVersion(nosPath + "/Multiclient.exe");
    ui.lab_nosVersion->setText(nosVersion);
    if (multiVersion != "")
        ui.lab_MCVersion->setText(multiVersion);
    else
        ui.lab_MCVersion->setText("None");

    if (Version(multiVersion) < Version(nosVersion))
        ui.btn_Create->setText(tr("Update Multiclient"));
    else
        ui.btn_Create->setText(tr("Create Multiclient"));
}

void mainForm::startClient()
{
	if (multiVersion != nosVersion)
	{
		QMessageBox messageBox;
		messageBox.setWindowTitle("Multiclient Update");
		messageBox.setText(tr("The Multiclient is not updated, update now?"));
		messageBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		messageBox.setDefaultButton(QMessageBox::Yes);
		messageBox.setButtonText(QMessageBox::Yes, tr("Yes"));
		messageBox.setButtonText(QMessageBox::No, tr("No"));
		int msg_reply = messageBox.exec();
		if (msg_reply == QMessageBox::Yes) {
			createMulticlient();
		}
	}
	Multiclient::start(nosPath + "/Multiclient.exe");
}

void mainForm::createMulticlient()
{
	if (Multiclient::create(nosPath, "Multiclient.exe"))
		QMessageBox::information(NULL, "Multiclient", tr("Multiclient created"));
	else
		QMessageBox::information(NULL, "Error", tr("Multiclient Creation failed"));
	ui.btn_Start->setEnabled(true);
	ui.btn_MulticlientLink->setEnabled(true);
	ui.btn_Delete->setEnabled(true);
	loadVersions();
}

void mainForm::removeMulticlient()
{
	Multiclient::remove(nosPath);
	ui.btn_Start->setEnabled(false);
	ui.btn_MulticlientLink->setEnabled(false);
	ui.btn_Delete->setEnabled(false);
	loadVersions();
}

void mainForm::shortcutMulticlient()
{
	QFile file;
	file.setFileName(nosPath + "/Multiclient.exe");
	QString DeskDir = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
	file.link(DeskDir + "/NosTale Multiclient.lnk");
}

void mainForm::createCustomThinMulticlient()
{
	Multiclient::createCustom(nosPath, ui.in_filename->text());
	if (clientEditor::editFile(nosPath, ui.in_filename->text(), ui.in_ip->text(), (ui.cB_port->currentIndex() == 10 ? port : 4000 + ui.cB_port->currentIndex() - 1)))
		QMessageBox::information(NULL, "Multiclient", tr("Multiclient created"));
	else
		QMessageBox::information(NULL, "Error", tr("Offset Error"));
}

void mainForm::createCustomMulticlient()
{
	Multiclient::create(nosPath, ui.in_filename->text());
	if (clientEditor::editFile(nosPath, ui.in_filename->text(), ui.in_ip->text(), (ui.cB_port->currentIndex() == 10 ? port : 4000 + ui.cB_port->currentIndex() - 1)))
		QMessageBox::information(NULL, "Multiclient", tr("Multiclient created"));
	else
		QMessageBox::information(NULL, "Error", tr("Offset Error"));
}

void mainForm::checkIPPort()
{
    bool bip = ui.in_ip->text().contains(ipRegex);
    bool bport = ui.cB_port->currentIndex() != 0;
    if (bport && ui.cB_port->currentIndex() == 10)
        bport = ui.cB_port->currentIndex() == 10 && port > 0;
    if (!(bip&&bport&&ui.in_filename->text() != ""))
    {
        ui.btn_createAdvanced->setEnabled(false);
        ui.btn_createAdvancedGf->setEnabled(false);
    }
    else
    {
        ui.btn_createAdvanced->setEnabled(true);
        ui.btn_createAdvancedGf->setEnabled(true);
    }
}

void mainForm::portChange(int index)
{
	if (index == 10)
	{
        port = QInputDialog::getText(nullptr, tr("Port"),
			tr("Port:"), QLineEdit::Normal, "", Q_NULLPTR, Qt::WindowTitleHint | Qt::WindowCloseButtonHint).toInt();
	}
	checkIPPort();
}

void mainForm::loadLanguage(const QString& rLanguage)
{
    if (rLanguage == "EN")
    {
        qApp->removeTranslator(&m_translator);
        settings->setValue("Language", "EN");
    }
    else if (rLanguage == "DE")
    {
        m_translator.load("lang_de");
        qApp->installTranslator(&m_translator);
        settings->setValue("Language", "DE");
    }
    else if (rLanguage == "IT")
    {
        m_translator.load("lang_it");
        qApp->installTranslator(&m_translator);
        settings->setValue("Language", "IT");
    }
    ui.retranslateUi(this);

    if (Version(multiVersion) < Version(nosVersion))
        ui.btn_Create->setText(tr("Update Multiclient"));
}

void mainForm::loadLanguage()
{
	QObject *senderObj = sender();
	if (senderObj->objectName() == "btn_DE")
		loadLanguage("DE");
	else if (senderObj->objectName() == "btn_EN")
		loadLanguage("EN");
	else if (senderObj->objectName() == "btn_IT")
		loadLanguage("IT");
}
