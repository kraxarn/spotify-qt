#include "settingspage.hpp"

SettingsPage::SettingsPage(Settings &settings, QWidget *parent)
	: settings(settings), parent(parent)
{
	page = new QVBoxLayout();
	page->setAlignment(Qt::AlignTop);
}

void SettingsPage::applyFail(const QString &name)
{
	QMessageBox::warning(
		parent,
		"Failed to apply settings",
		QString("Failed to apply setting \"%1\". Check your settings and try again.").arg(name));
}

QWidget *SettingsPage::toWidget()
{
	Utils::layoutToWidget(page);
}