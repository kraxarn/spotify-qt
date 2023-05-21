#include "settingspage/base.hpp"
#include "mainwindow.hpp"

SettingsPage::Base::Base(lib::settings &settings, QWidget *parent)
	: QTabWidget(parent),
	settings(settings)
{
}

void SettingsPage::Base::warning(const QString &title, const QString &message)
{
	QMessageBox::warning(parentWidget(), title, message);
}

void SettingsPage::Base::info(const QString &title, const QString &message)
{
	QMessageBox::information(parentWidget(), title, message);
}

void SettingsPage::Base::applyFail(const QString &setting)
{
	warning("Failed to apply settings",
		QString("Failed to apply setting \"%1\". Check your settings and try again.")
			.arg(setting));
}

auto SettingsPage::Base::applyWarning(const QString &title, const QString &message) -> bool
{
	const auto dialogTitle = title.isEmpty()
		? QStringLiteral("Invalid value")
		: title;

	const auto result = QMessageBox::warning(parentWidget(), dialogTitle,
		QString("%1.\nDo you want to save anyway?").arg(message),
		QMessageBox::Ok | QMessageBox::Cancel);

	return result == QMessageBox::Ok;
}

auto SettingsPage::Base::tabContent() -> QVBoxLayout *
{
	auto *layout = new QVBoxLayout(this);
	layout->setAlignment(Qt::AlignTop);
	return layout;
}
