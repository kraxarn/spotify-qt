#include "settingspage.hpp"

SettingsPage::SettingsPage(Settings &settings, QWidget *parent)
	: settings(settings), QWidget(parent)
{
	layout = new QVBoxLayout();
	layout->setAlignment(Qt::AlignTop);
	setLayout(layout);
}

void SettingsPage::warning(const QString &title, const QString &message)
{
	QMessageBox::warning(this, title, message);
}

void SettingsPage::applyFail(const QString &setting)
{
	warning("Failed to apply settings",
		QString("Failed to apply setting \"%1\". Check your settings and try again.").arg(setting));
}
