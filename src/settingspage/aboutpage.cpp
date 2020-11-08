#include "aboutpage.hpp"

AboutPage::AboutPage(Settings &settings, QWidget *parent)
	: SettingsPage(settings, parent)
{
	addTab(about(), "General");
	addTab(systemInfo(), "System information");
	addTab(appLogs(), "Application logs");
}

QWidget *AboutPage::about()
{
	auto cacheSize = 0u;
	auto mainWindow = dynamic_cast<MainWindow *>(findMainWindow());
	if (mainWindow != nullptr)
		for (auto const &file : QDir(mainWindow->getCacheLocation()).entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot))
			for (auto const &f : QDir(file.absoluteFilePath()).entryInfoList(QDir::Files))
				cacheSize += f.size();

	auto layout = tabContent();

	// Title
	auto title = new QHBoxLayout();
	title->setAlignment(Qt::AlignHCenter);
	auto titleLogo = new QLabel();
	titleLogo->setPixmap(Icon::get("logo:spotify-qt").pixmap(64, 64));
	title->addWidget(titleLogo);
	layout->addLayout(title);
	auto titleVersion = new QVBoxLayout();
	titleVersion->setSpacing(0);
	titleVersion->setAlignment(Qt::AlignVCenter);
	auto titleAppName = new QLabel("spotify-qt");
	auto appNameFont = titleAppName->font();
	appNameFont.setPointSize(appNameFont.pointSizeF() * 1.5);
	titleAppName->setFont(appNameFont);
	titleVersion->addWidget(titleAppName);
	titleVersion->addWidget(new QLabel(QString("%1, built using Qt %2").arg(APP_VERSION).arg(QT_VERSION_STR)));
	title->addLayout(titleVersion);

	// User info
	if (mainWindow != nullptr)
		layout->addWidget(new QLabel(QString("Hello %1!")
			.arg(mainWindow->getCurrentUser().displayName)), 0, Qt::AlignHCenter);

	// Grid with buttons
	layout->addSpacing(8);
	auto options = new QGridLayout();
	layout->addLayout(options, 1);
	layout->setAlignment(Qt::AlignBottom);

	// Open cache directory
	auto openCache = new QPushButton(Icon::get("folder-temp"),
		QString("Open cache directory (%1M)").arg(cacheSize / 1000 / 1000));
	openCache->setFlat(true);
	QAbstractButton::connect(openCache, &QPushButton::clicked, [this, mainWindow](bool checked)
	{
		if (mainWindow == nullptr)
			return;
		Utils::openUrl(mainWindow->getCacheLocation(), LinkType::Web, this);
	});
	options->addWidget(openCache, 0, 0);

	// Open config file
	auto openConfig = new QPushButton(Icon::get("folder-txt"), "Open config file");
	openConfig->setFlat(true);
	QAbstractButton::connect(openConfig, &QPushButton::clicked, [this](bool checked)
	{
		Utils::openUrl(Settings::fileName(), LinkType::Path, this);
	});
	options->addWidget(openConfig, 0, 1);

	return Utils::layoutToWidget(layout);
}

QWidget *AboutPage::systemInfo()
{
	return new SystemInfoDialog(findMainWindow(), this);
}

QWidget *AboutPage::appLogs()
{
	return new LogViewer(this);
}

QIcon AboutPage::icon()
{
	return Icon::get("help-about");
}

QString AboutPage::title()
{
	return "About";
}

bool AboutPage::save()
{
	return true;
}