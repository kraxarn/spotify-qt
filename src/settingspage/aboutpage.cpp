#include "aboutpage.hpp"

AboutPage::AboutPage(lib::settings &settings, QWidget *parent)
	: SettingsPage(settings, parent)
{
	addTab(about(), "General");
	addTab(systemInfo(), "System information");
	addTab(cacheInfo(), "Cache");
	addTab(configPreview(), "Config preview");
}

QWidget *AboutPage::about()
{
	auto mainWindow = MainWindow::find(parentWidget());
	auto layout = tabContent();
	layout->setAlignment(Qt::AlignCenter);

	// Version
#ifdef GIT_COMMIT
	auto version = QString("%1-%2, built using Qt %3\nDevelopment build")
		.arg(APP_VERSION).arg(GIT_COMMIT).arg(QT_VERSION_STR);
#else
	auto version = QString("%1, built using Qt %2")
		.arg(APP_VERSION).arg(QT_VERSION_STR);
#endif

	// Logo
	auto titleLogo = new QLabel();
	titleLogo->setPixmap(Icon::get("logo:spotify-qt").pixmap(96, 96));
	layout->addWidget(titleLogo, 0, Qt::AlignHCenter);

	// Title
	auto titleAppName = new QLabel(QString("spotify-qt %1").arg(APP_VERSION));
	auto appNameFont = titleAppName->font();
	appNameFont.setPointSize(appNameFont.pointSizeF() * 1.5);
	titleAppName->setFont(appNameFont);
	layout->addWidget(titleAppName, 0, Qt::AlignHCenter);

	// Qt version
	layout->addWidget(new QLabel(QString("Built using Qt %1").arg(QT_VERSION_STR), this),
		0, Qt::AlignHCenter);

	// Build info
#ifdef GIT_COMMIT
	layout->addWidget(new QLabel(QString("Commit %1, development build").arg(GIT_COMMIT), this),
		0, Qt::AlignHCenter);
#endif

	// User info
	if (mainWindow != nullptr)
	{
		layout->addSpacing(16);
		layout->addWidget(new QLabel(QString("Hello %1!")
			.arg(mainWindow->getCurrentUser().displayName)), 0, Qt::AlignHCenter);
	}

	return Utils::layoutToWidget(layout);
}

QWidget *AboutPage::systemInfo()
{
	return new SystemInfoView(MainWindow::find(parentWidget()), this);
}

QWidget *AboutPage::cacheInfo()
{
	return new CacheView(MainWindow::find(parentWidget())->getCacheLocation(), this);
}

QWidget *AboutPage::configPreview()
{
	return new ConfigView(settings, this);
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