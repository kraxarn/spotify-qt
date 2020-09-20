#include "systeminfodialog.hpp"

SystemInfoDialog::SystemInfoDialog(QWidget *mainWindow, QWidget *parent)
	: mainWindow(mainWindow), QDialog(parent)
{
	auto layout = new QVBoxLayout();
	setLayout(layout);
	setWindowTitle("System info");

	auto textInfo = new QTextEdit(this);
	textInfo->setHtml(systemInfo());
	textInfo->setReadOnly(true);
	layout->addWidget(textInfo);

	auto infoAbout = new QLabel(
		"This information could be useful when reporting bugs. "
		"Additional information, depending on the type of issue, may be more helpful.", this);
	infoAbout->setWordWrap(true);
	layout->addWidget(infoAbout);

	auto buttons = new QDialogButtonBox();
	QPushButton::connect(buttons->addButton(QDialogButtonBox::Ok), &QPushButton::clicked,
		[this](bool checked)
		{
			accept();
		});
	layout->addWidget(buttons);
}

QString SystemInfoDialog::systemInfo()
{
	QMap<QString, QString> info;

	// Qt version
	info["Qt version"] = QT_VERSION_STR;

	// spotify-qt version
	info["App version"] = APP_VERSION;

	// Desktop environment
	if (qEnvironmentVariableIsSet("XDG_CURRENT_DESKTOP"))
	{
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
		info["Current desktop"] = qEnvironmentVariable("XDG_CURRENT_DESKTOP");
#else
		info["Current desktop"] = QString::fromLocal8Bit(qgetenv("XDG_CURRENT_DESKTOP"));
#endif
	}

	// Device
	auto device = ((MainWindow *) mainWindow)->getCurrentPlayback().device;
	if (!device.name.isEmpty() && !device.type.isEmpty())
		info["Device"] = QString("%1 (%2)").arg(device.name).arg(device.type);

	// Kernel
	info["Kernel"] = QString("%1 %2").arg(QSysInfo::kernelType()).arg(QSysInfo::kernelVersion());

	// Product
	info["Product"] = QSysInfo::prettyProductName();

	// Build ABI
	info["ABI"] = QSysInfo::buildAbi();

	QString systemInfo("<table>");
	QMapIterator<QString, QString> i(info);
	while (i.hasNext())
	{
		i.next();
		systemInfo += QString("<tr><td>%1:</td> <td>%2</td></tr>").arg(i.key()).arg(i.value());
	}
	return systemInfo + "</table>";
}