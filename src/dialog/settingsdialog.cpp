#include "settingsdialog.hpp"

SettingsDialog::SettingsDialog(Settings &settings, QWidget *parent)
	: settings(settings), QDialog(parent)
{
	// Main layout
	auto mainLayout = new QHBoxLayout();

	// List of categories
	stack = new QStackedWidget(this);
	categories = new QListWidget(this);
	pages = QList<SettingsPage*>({
		new ApplicationPage(settings, this),
		new InterfacePage(settings, this),
		new SpotifyPage(settings, this),
		new PlaylistsPage(settings, this),
		new AboutPage(settings, this)
	});

//		{"window", "Application"},
//		{"draw-brush", "Interface"}, // Tray icon
//		{"headphones", "Spotify"},
//		{"view-media-playlist", "Playlists"},
//		{"help-about", "About"},

	for (auto &page : pages)
	{
		new QListWidgetItem(page->icon(), page->title(), categories);
		stack->addWidget(page);
	}
	categories->setCurrentRow(0);
	categories->setMaximumWidth(160);
	QListWidget::connect(categories, &QListWidget::currentRowChanged, this, &SettingsDialog::categoryChanged);
	mainLayout->addWidget(categories);

	// Side layout
	auto stackLayout = new QVBoxLayout();
	mainLayout->addLayout(stackLayout, 1);

	// Stack to show categories
	stackLayout->addWidget(stack, 1, Qt::AlignTop);

	// Buttons
	auto buttons = new QDialogButtonBox();
	auto okButton = buttons->addButton(QDialogButtonBox::Ok);
	QPushButton::connect(okButton, &QPushButton::clicked, [this](bool checked)
	{
		if (applySettings())
			accept();
	});
	auto applyButton = buttons->addButton(QDialogButtonBox::Apply);
	QPushButton::connect(applyButton, &QPushButton::clicked, [this](bool checked)
	{
		applySettings();
	});
	auto cancelButton = buttons->addButton(QDialogButtonBox::Cancel);
	QPushButton::connect(cancelButton, &QPushButton::clicked, [this](bool checked)
	{
		accept();
	});
	stackLayout->addWidget(buttons);

	// Set layout
	setWindowTitle("Settings");
	setLayout(mainLayout);
	resize(520, 360);
}

bool SettingsDialog::applySettings()
{

	// Desktop notifications and tray icon
	if (itfTrayNotify->isChecked() && !itfTrayIcon->isChecked())
	{
		itfTrayIcon->setChecked(true);
		QMessageBox::information(this, "Desktop Notifications",
			"Desktop notifications requires tray icon to be enabled, so it was enabled");
	}

	// Check if tray icon needs to be reloaded
	auto reloadTray = settings.general.trayIcon != itfTrayIcon->isChecked()
		|| settings.general.trayNotifications != itfTrayNotify->isChecked()
		|| settings.general.trayLightIcon != itfTrayInvert->isChecked();

	// Apply
	settings.general.trayIcon = itfTrayIcon->isChecked();
	settings.general.trayNotifications = itfTrayNotify->isChecked();
	settings.general.trayLightIcon = itfTrayInvert->isChecked();
	settings.general.trayAlbumArt = itfTrayAlbum->isChecked();

	// Reload if needed
	auto window = dynamic_cast<MainWindow *>(parent());
	if (reloadTray && window != nullptr)
		window->reloadTrayIcon();

	// Check all pages
	for (auto &page : pages)
	{
		if (!page->save())
		{
			Log::error("Failed to save: {}", page->title());
			return false;
		}
	}

	// Everything is fine
	settings.save();
	return true;
}

void SettingsDialog::categoryChanged(int row)
{
	stack->setCurrentIndex(row);
}
