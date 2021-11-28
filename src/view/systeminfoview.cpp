#include "systeminfoview.hpp"

SystemInfoView::SystemInfoView(QWidget *parent)
	: QWidget(parent)
{
	auto *layout = new QVBoxLayout();
	setLayout(layout);
	setWindowTitle("System info");

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

	auto *copy = new QPushButton("Copy to clipboard", this);
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
	info.add("App version", QString("%1 (%2) - lib %3")
		.arg(APP_VERSION, GIT_COMMIT, LIB_VERSION));
#else
	info.add("App version", APP_VERSION);
#endif

	// Device
	auto *mainWindow = MainWindow::find(parentWidget());
	if (mainWindow != nullptr)
	{
		auto device = mainWindow->getCurrentPlayback().device;
		if (!device.name.empty() && !device.type.empty())
		{
			info.add("Device",
				QString::fromStdString(lib::fmt::format("{} ({})",
					device.name, device.type)));
		}
	}

	// Qt D-Bus support
#ifdef USE_DBUS
	info.add("D-Bus support", "Yes");
#else
	info.add("D-Bus support", "No");
#endif

	// KCrash support
#ifdef USE_KCRASH
	info.add("KCrash support", "Yes");
#else
	info.add("KCrash support", "No");
#endif

	return info;
}

void SystemInfoView::copyToClipboard(bool /*checked*/)
{
	QApplication::clipboard()->setText(systemInfo().to_text());
}
