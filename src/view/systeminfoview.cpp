#include "systeminfoview.hpp"
#include "mainwindow.hpp"

#include <QClipboard>

SystemInfoView::SystemInfoView(const lib::settings &settings, QWidget *parent)
	: QWidget(parent),
	appStatus(new SpotifyAppStatus(settings, this)),
	appVersion(SpotifyAppVersion::Unknown)
{
	auto *layout = new QVBoxLayout();
	setLayout(layout);

	textInfo = new QTextEdit(this);
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

void SystemInfoView::showEvent(QShowEvent *event)
{
	QWidget::showEvent(event);

	if (appVersion == SpotifyAppVersion::Unknown)
	{
		appStatus->version();
		connect(appStatus, &SpotifyAppStatus::versionFinished,
			this, &SystemInfoView::onAppStatusVersionFinished);
	}
}

auto SystemInfoView::systemInfo() const -> lib::qt::system_info
{
	SystemInfo info;

	auto *mainWindow = MainWindow::find(parentWidget());
	if (mainWindow != nullptr)
	{
		auto device = mainWindow->playback().device;
		if (!device.name.empty() && !device.type.empty())
		{
			info.add(QStringLiteral("Device"),
				QString::fromStdString(lib::fmt::format("{} ({})",
					device.name, device.type)));
		}
	}

	if (appVersion != SpotifyAppVersion::Unknown)
	{
		QString versionString;
		switch (appVersion)
		{
			case SpotifyAppVersion::Version1:
				versionString = QStringLiteral("Granted quota extension");
				break;

			case SpotifyAppVersion::Version2:
				versionString = QStringLiteral("Development mode (old)");
				break;

			case SpotifyAppVersion::Version3:
				versionString = QStringLiteral("Development mode (new)");
				break;

			default:
				break;
		}

		info.add(QStringLiteral("Spotify app"), versionString);
	}

	return info;
}

void SystemInfoView::copyToClipboard(bool /*checked*/)
{
	QApplication::clipboard()->setText(systemInfo().to_text());
}

void SystemInfoView::onAppStatusVersionFinished(const SpotifyAppVersion version)
{
	appVersion = version;
	textInfo->setHtml(systemInfo().to_html());
}
