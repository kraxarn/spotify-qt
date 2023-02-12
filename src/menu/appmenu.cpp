#include "menu/appmenu.hpp"
#include "mainwindow.hpp"
#include <QPushButton>

AppMenu::AppMenu(lib::settings &settings, const lib::http_client &httpClient,
	lib::cache &cache, QWidget *parent)
	: settings(settings),
	httpClient(httpClient),
	cache(cache),
	parent(parent)
{
}

void AppMenu::logOut()
{
	QMessageBox box(QMessageBox::Question,
		QStringLiteral("Are you sure?"),
		QStringLiteral("Do you also want to clear your application credentials or only log out?"));

	auto *clearAll = box.addButton(QStringLiteral("Clear everything"), QMessageBox::AcceptRole);
	auto *logOut = box.addButton(QStringLiteral("Only log out"), QMessageBox::AcceptRole);
	auto *cancel = box.addButton(QStringLiteral("Cancel"), QMessageBox::RejectRole);

	box.exec();
	auto *result = box.clickedButton();

	// Return if we pressed cancel
	if (result == nullptr || result == cancel)
	{
		return;
	}

	// Clear client secret/id if clearAll
	if (result == clearAll)
	{
		settings.remove_client();
	}

	// Clear login if cleatAll/logOut
	if (result == clearAll || result == logOut)
	{
		settings.remove_tokens();
	}

	settings.save();

	QMessageBox::information(MainWindow::find(parent),
		QStringLiteral("Logged out"),
		QStringLiteral("You are now logged out, the application will now close"));

	QCoreApplication::quit();
}

void AppMenu::openSettings()
{
	if (settingsDialog == nullptr)
	{
		auto *mainWindow = MainWindow::find(parent);
		settingsDialog = new Dialog::Settings(settings, cache, httpClient, mainWindow);
	}

	settingsDialog->open();
}
