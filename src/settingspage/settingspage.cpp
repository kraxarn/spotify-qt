#include "settingspage.hpp"
#include "../mainwindow.hpp"

SettingsPage::SettingsPage(lib::settings &settings, QWidget *parent)
	: settings(settings),
	QTabWidget(parent)
{
}

void SettingsPage::warning(const QString &title, const QString &message)
{
	QMessageBox::warning(this, title, message);
}

void SettingsPage::applyFail(const QString &setting)
{
	warning("Failed to apply settings",
		QString("Failed to apply setting \"%1\". Check your settings and try again.")
			.arg(setting));
}

QVBoxLayout *SettingsPage::tabContent()
{
	auto layout = new QVBoxLayout(this);
	layout->setAlignment(Qt::AlignTop);
	return layout;
}
