#include "systeminfoview.hpp"

SystemInfoView::SystemInfoView(QWidget *mainWindow, QWidget *parent)
	: mainWindow(mainWindow),
	QWidget(parent)
{
	auto *layout = new QVBoxLayout();
	setLayout(layout);
	setWindowTitle("System info");

	auto *textInfo = new QTextEdit(this);
	textInfo->setHtml(systemInfo());
	textInfo->setReadOnly(true);
	layout->addWidget(textInfo);

	auto *infoLayout = new QHBoxLayout();

	auto *infoAbout = new QLabel(
		"This information could be useful when reporting bugs. "
		"Additional information, depending on the type of issue, may be more helpful.", this);
	infoAbout->setWordWrap(true);
	infoLayout->addWidget(infoAbout, 1);

	auto *copy = new QPushButton("Copy to clipboard", this);
	QPushButton::connect(copy, &QPushButton::clicked,
		this, &SystemInfoView::copyToClipboard);
	infoLayout->addWidget(copy);

	layout->addLayout(infoLayout);
}

auto SystemInfoView::systemInfo(bool html) -> QString
{
	return systemInfo(((MainWindow *) mainWindow)->getCurrentPlayback(), html);
}

auto SystemInfoView::systemInfo(const lib::spt::playback &playback, bool html) -> QString
{
	QMap<QString, QString> info;

	// Qt version
	info["Qt version"] = QT_VERSION_STR;

	// spotify-qt version
#ifdef GIT_COMMIT
	info["App version"] = QString("%1-dev (%2) - lib %3")
		.arg(APP_VERSION, GIT_COMMIT, LIB_VERSION);
#else
	info["App version"] = APP_VERSION;
#endif

	// Desktop environment
	if (SystemUtils::hasEnv("XDG_CURRENT_DESKTOP"))
	{
		info["Current desktop"] = SystemUtils::env("XDG_CURRENT_DESKTOP");
	}

	// Device
	auto device = playback.device;
	if (!device.name.empty() && !device.type.empty())
	{
		info["Device"] = QString::fromStdString(lib::fmt::format("{} ({})",
			device.name, device.type));
	}

	// Kernel
	info["Kernel"] = QString("%1 %2").arg(QSysInfo::kernelType(), QSysInfo::kernelVersion());

	// Product
	info["Product"] = QSysInfo::prettyProductName();

	// Build ABI
	info["ABI"] = QSysInfo::buildAbi();

	// Qt D-Bus support
#ifdef USE_DBUS
	info["D-Bus support"] = "Yes";
#else
	info["D-Bus support"] = "No";
#endif

	QString systemInfo(html ? "<table>" : "");
	QMapIterator<QString, QString> i(info);
	while (i.hasNext())
	{
		i.next();
		systemInfo += QString(html
			? "<tr><td>%1:</td> <td>%2</td></tr>"
			: "%1: %2\n")
			.arg(i.key(), i.value());
	}
	return html
		? systemInfo + "</table>"
		: systemInfo;
}

void SystemInfoView::copyToClipboard(bool /*checked*/)
{
	QApplication::clipboard()->setText(systemInfo(false));
}
