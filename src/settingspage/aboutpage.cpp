#include "aboutpage.hpp"
#include "mainwindow.hpp"

AboutPage::AboutPage(lib::settings &settings, lib::cache &cache,
	const lib::http_client &httpClient, QWidget *parent)
	: SettingsPage(settings, parent)
{
	addTab(about(), "General");
	addTab(systemInfo(), "System information");
	addTab(cacheInfo(), "Cache");
	addTab(configPreview(), "Config preview");
	addTab(contributors(httpClient), "Contributors");

	if (lib::crash_handler::is_init()
		|| !cache.get_all_crashes().empty())
	{
		addTab(crashLogs(cache), "Crashes");
	}
}

auto AboutPage::about() -> QWidget *
{
	auto *mainWindow = MainWindow::find(parentWidget());
	auto *layout = tabContent();
	layout->setAlignment(Qt::AlignCenter);

	// Logo
	constexpr int logoSize = 96;
	auto *titleLogo = new QLabel();
	titleLogo->setPixmap(Icon::get("logo:spotify-qt").pixmap(logoSize, logoSize));
	layout->addWidget(titleLogo, 0, Qt::AlignHCenter);

	// Title
	constexpr float appNameFontMulti = 1.5F;
	auto *titleAppName = new QLabel(QString("spotify-qt %1").arg(APP_VERSION));
	auto appNameFont = titleAppName->font();
	appNameFont.setPointSize((int) (appNameFont.pointSizeF() * appNameFontMulti));
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
				.arg(QString::fromStdString(mainWindow->getCurrentUser().display_name))),
			0, Qt::AlignHCenter);
	}

	return WidgetUtils::layoutToWidget(layout, this);
}

auto AboutPage::systemInfo() -> QWidget *
{
	return new SystemInfoView(this);
}

auto AboutPage::cacheInfo() -> QWidget *
{
	if (paths == nullptr)
	{
		paths = new QtPaths(this);
	}
	return new CacheView(*paths, this);
}

auto AboutPage::configPreview() -> QWidget *
{
	return new ConfigView(settings, this);
}

auto AboutPage::crashLogs(lib::cache &cache) -> QWidget *
{
	return new View::Crashes(cache, this);
}

auto AboutPage::contributors(const lib::http_client &httpClient) -> QWidget *
{
	return new List::Contributors(httpClient);
}

auto AboutPage::icon() -> QIcon
{
	return Icon::get("help-about");
}

auto AboutPage::title() -> QString
{
	return "About";
}

auto AboutPage::save() -> bool
{
	return true;
}
