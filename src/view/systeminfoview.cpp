#include "systeminfoview.hpp"

SystemInfoView::SystemInfoView(QWidget *mainWindow, QWidget *parent)
	: mainWindow(mainWindow),
	QWidget(parent)
{
	auto layout = new QVBoxLayout();
	setLayout(layout);
	setWindowTitle("System info");

	auto textInfo = new QTextEdit(this);
	textInfo->setHtml(systemInfo());
	textInfo->setReadOnly(true);
	layout->addWidget(textInfo);

	auto infoLayout = new QHBoxLayout();

	auto infoAbout = new QLabel(
		"This information could be useful when reporting bugs. "
		"Additional information, depending on the type of issue, may be more helpful.", this);
	infoAbout->setWordWrap(true);
	infoLayout->addWidget(infoAbout, 1);

	auto copy = new QPushButton("Copy to clipboard", this);
	QPushButton::connect(copy, &QPushButton::clicked, this, &SystemInfoView::copyToClipboard);
	infoLayout->addWidget(copy);

	layout->addLayout(infoLayout);
}

QString SystemInfoView::systemInfo(bool html)
{
	return systemInfo(((MainWindow *) mainWindow)->getCurrentPlayback(), html);
}

QString SystemInfoView::systemInfo(const spt::Playback &playback, bool html)
{
	QMap<QString, QString> info;

	// Qt version
	info["Qt version"] = QT_VERSION_STR;

	// spotify-qt version
#ifdef GIT_COMMIT
	info["App version"] = QString("%1-dev (%2)")
		.arg(APP_VERSION).arg(GIT_COMMIT);
#else
	info["App version"] = APP_VERSION;
#endif

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
	auto device = playback.device;
	if (!device.name.isEmpty() && !device.type.isEmpty())
		info["Device"] = QString("%1 (%2)").arg(device.name).arg(device.type);

	// Kernel
	info["Kernel"] = QString("%1 %2").arg(QSysInfo::kernelType()).arg(QSysInfo::kernelVersion());

	// Product
	info["Product"] = QSysInfo::prettyProductName();

	// Build ABI
	info["ABI"] = QSysInfo::buildAbi();

	QString systemInfo(html ? "<table>" : "");
	QMapIterator<QString, QString> i(info);
	while (i.hasNext())
	{
		i.next();
		systemInfo += QString(html
			? "<tr><td>%1:</td> <td>%2</td></tr>"
			: "%1: %2\n")
			.arg(i.key()).arg(i.value());
	}
	return html
		? systemInfo + "</table>"
		: systemInfo;
}

void SystemInfoView::copyToClipboard(bool)
{
	QApplication::clipboard()->setText(systemInfo(false));
}
