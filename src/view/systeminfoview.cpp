#include "systeminfoview.hpp"

#include "mainwindow.hpp"
#include "util/appinstalltype.hpp"

SystemInfoView::SystemInfoView(QWidget *parent)
	: QWidget(parent)
{
	auto *layout = new QVBoxLayout();
	setLayout(layout);

	auto *textInfo = new QTextEdit(this);
	textInfo->setHtml(systemInfo().to_html());
	textInfo->setReadOnly(true);
	layout->addWidget(textInfo);

	auto *infoLayout = new QHBoxLayout();

	auto *infoAbout = new QLabel(
		"This information could be useful when reporting bugs. "
		"Additional information, depending on the type of issue, may be more helpful.", this);
	infoAbout->setWordWrap(true);
	infoLayout->addWidget(infoAbout, 1);

	auto *copy = new QPushButton(QStringLiteral("Copy to clipboard"), this);
	QPushButton::connect(copy, &QPushButton::clicked,
		this, &SystemInfoView::copyToClipboard);
	infoLayout->addWidget(copy);

	layout->addLayout(infoLayout);
}

auto SystemInfoView::systemInfo() -> lib::qt::system_info
{
	lib::qt::system_info info;

	// spotify-qt version
#ifdef GIT_COMMIT
	info.add(QStringLiteral("App version"), QString("%1 (%2) - lib %3")
		.arg(APP_VERSION, GIT_COMMIT, LIB_VERSION));
#else
	info.add(QStringLiteral("App version"), APP_VERSION);
#endif

	// Device
	auto *mainWindow = MainWindow::find(parentWidget());
	if (mainWindow != nullptr)
	{
		auto device = mainWindow->getCurrentPlayback().device;
		if (!device.name.empty() && !device.type.empty())
		{
			info.add(QStringLiteral("Device"),
				QString::fromStdString(lib::fmt::format("{} ({})",
					device.name, device.type)));
		}
	}

	// Qt D-Bus support
#ifdef USE_DBUS
	info.add(QStringLiteral("D-Bus support"), QStringLiteral("Yes"));
#else
	info.add(QStringLiteral("D-Bus support"), QStringLiteral("No"));
#endif

	// KCrash support
#ifdef USE_KCRASH
	info.add(QStringLiteral("KCrash support"), QStringLiteral("Yes"));
#else
	info.add(QStringLiteral("KCrash support"), QStringLiteral("No"));
#endif

	// App install type
	if (AppInstallType::get() != InstallType::Unknown)
	{
		info.add(QStringLiteral("Install type"), AppInstallType::getString());
	}

	// CMake build type
	info.add(QStringLiteral("Build type"), QStringLiteral(BUILD_TYPE));

	return info;
}

void SystemInfoView::copyToClipboard(bool /*checked*/)
{
	QApplication::clipboard()->setText(systemInfo().to_text());
}
