#include "settingsdialog.hpp"

SettingsDialog::SettingsDialog(Settings &settings, QWidget *parent)
	: settings(settings), QDialog(parent)
{
	// Main layout
	auto mainLayout = new QVBoxLayout();
	auto tabs = new QTabWidget();

	// Tabs
	pages.append(
		{
			new AppSettings(settings, this),
			new InterfaceSettings(settings, this),
			new TraySettings(settings, this),
			new SpotifySettings(settings, this),
			new AboutSettings(settings, this)
		}
	);

	for (auto page : pages)
	{
		tabs->addTab(page->toWidget(), page->title());
	}

	mainLayout->addWidget(tabs, 1);

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
	mainLayout->addWidget(buttons);

	// Set layout
	setWindowTitle("Settings");
	setLayout(mainLayout);
	resize(360, 260);
}

bool SettingsDialog::applySettings()
{
	auto window = dynamic_cast<MainWindow *>(parent());

	for (auto &page : pages)
	{
		if (!page->applySettings(window))
			return false;
	}

	settings.save();
	return true;
}