#include "dialog/settings.hpp"

#include "settingspage/about.hpp"
#include "settingspage/application.hpp"
#include "settingspage/interface.hpp"
#include "settingspage/playlists.hpp"
#include "settingspage/spotify.hpp"
#include "settingspage/troubleshoot.hpp"
#include "settingspage/logs.hpp"

Dialog::Settings::Settings(lib::settings &settings, lib::cache &cache,
	const lib::http_client &httpClient, QWidget *parent)
	: QDialog(parent),
	settings(settings)
{
	setStyleSheet(parent->styleSheet());

	// Main layout
	auto *mainLayout = new QHBoxLayout();

	// List of categories
	stack = new QStackedWidget(this);
	categories = new QListWidget(this);
	pages = QList<SettingsPage::Base *>({
		new SettingsPage::Application(settings, this),
		new SettingsPage::Interface(settings, this),
		new SettingsPage::Spotify(settings, this),
		new SettingsPage::Playlists(settings, this),
		new SettingsPage::Logs(settings, this),
		new SettingsPage::Troubleshoot(settings, cache, this),
		new SettingsPage::About(settings, httpClient, this),
	});

	for (auto &page: pages)
	{
		new QListWidgetItem(page->icon(), page->title(), categories);
		stack->addWidget(page);
	}
	categories->setCurrentRow(0);
	categories->setMaximumWidth(160);
	QListWidget::connect(categories, &QListWidget::currentRowChanged,
		this, &Dialog::Settings::categoryChanged);
	mainLayout->addWidget(categories);

	// Side layout
	auto *stackLayout = new QVBoxLayout();
	mainLayout->addLayout(stackLayout, 1);

	// Stack to show categories
	stackLayout->addWidget(stack, 1);

	// Buttons
	auto *buttons = new QDialogButtonBox();
	auto *okButton = buttons->addButton(QDialogButtonBox::Ok);
	QPushButton::connect(okButton, &QPushButton::clicked, [this](bool /*checked*/)
	{
		if (applySettings())
		{
			accept();
		}
	});
	auto *applyButton = buttons->addButton(QDialogButtonBox::Apply);
	QPushButton::connect(applyButton, &QPushButton::clicked, [this](bool /*checked*/)
	{
		applySettings();
	});
	auto *cancelButton = buttons->addButton(QDialogButtonBox::Cancel);
	QPushButton::connect(cancelButton, &QPushButton::clicked, [this](bool /*checked*/)
	{
		accept();
	});
	stackLayout->addWidget(buttons);

	// Set layout
	setWindowTitle("Settings");
	setLayout(mainLayout);
	resize(620, 400);
}

auto Dialog::Settings::applySettings() -> bool
{
	// Check all pages
	for (auto &page: pages)
	{
		if (!page->save())
		{
			lib::log::error("Failed to save: {}", page->title().toStdString());
			return false;
		}
	}

	// Everything is fine
	settings.save();
	return true;
}

void Dialog::Settings::categoryChanged(int row)
{
	stack->setCurrentIndex(row);
}
