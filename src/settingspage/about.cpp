#include "settingspage/about.hpp"
#include "mainwindow.hpp"

SettingsPage::About::About(lib::settings &settings,
	const lib::http_client &httpClient,
	QWidget *parent)
	: SettingsPage::Base(settings, parent)
{
	addTab(about(), "General");
	addTab(contributors(httpClient), "Contributors");
}

auto SettingsPage::About::about() -> QWidget *
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
	layout->addWidget(new QLabel(QString("Commit %1").arg(GIT_COMMIT), this),
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

auto SettingsPage::About::contributors(const lib::http_client &httpClient) -> QWidget *
{
	return new List::Contributors(httpClient);
}

auto SettingsPage::About::icon() -> QIcon
{
	return Icon::get("help-about");
}

auto SettingsPage::About::title() -> QString
{
	return "About";
}

auto SettingsPage::About::save() -> bool
{
	return true;
}
