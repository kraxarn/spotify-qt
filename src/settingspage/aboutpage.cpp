#include "aboutpage.hpp"

AboutPage::AboutPage(Settings &settings, QWidget *parent)
	: SettingsPage(settings, parent)
{
	auto cacheSize = 0u;
	auto mainWindow = dynamic_cast<MainWindow *>(parentWidget());
	if (mainWindow != nullptr)
		for (auto const &file : QDir(mainWindow->getCacheLocation()).entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot))
			for (auto const &f : QDir(file.absoluteFilePath()).entryInfoList(QDir::Files))
				cacheSize += f.size();

	auto layout = new QVBoxLayout();
	layout->setAlignment(Qt::AlignTop);

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
	titleVersion->addWidget(new QLabel(APP_VERSION));
	title->addLayout(titleVersion);

	// User info
	if (mainWindow != nullptr)
		layout->addWidget(new QLabel(QString("Hello %1!")
			.arg(mainWindow->getCurrentUser().displayName)), 0, Qt::AlignHCenter);

	// Grid with buttons
	layout->addSpacing(8);
	auto options = new QGridLayout();

	// About Qt
	auto aboutQt = new QPushButton(Icon::get("logo:qt"),
		QString("About Qt %1.%2")
			.arg(QT_VERSION_MAJOR).arg(QT_VERSION_MINOR));
	aboutQt->setFlat(true);
	QAbstractButton::connect(aboutQt, &QPushButton::clicked, [this](bool checked)
	{
		QMessageBox::aboutQt(this);
	});
	options->addWidget(aboutQt);

	// Generate report
	auto generateReport = new QPushButton(Icon::get("description"), "System info", this);
	generateReport->setFlat(true);
	QAbstractButton::connect(generateReport, &QPushButton::clicked, [this, mainWindow](bool checked)
	{
		(new SystemInfoDialog(mainWindow, this))->show();
	});
	options->addWidget(generateReport, 0, 1);

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
	options->addWidget(openCache);

	// Open config file
	auto openConfig = new QPushButton(Icon::get("folder-txt"), "Open config file");
	openConfig->setFlat(true);
	QAbstractButton::connect(openConfig, &QPushButton::clicked, [this](bool checked)
	{
		Utils::openUrl(Settings::fileName(), LinkType::Path, this);
	});
	options->addWidget(openConfig);
	layout->addLayout(options, 1);
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
	return false;
}