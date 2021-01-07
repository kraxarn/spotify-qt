#include "settingsdialog.hpp"

SettingsDialog::SettingsDialog(lib::settings &settings, QWidget *parent)
	: settings(settings),
	QDialog(parent)
{
	setStyleSheet(parent->styleSheet());

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
	stackLayout->addWidget(stack, 1);

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
	resize(620, 400);
}

bool SettingsDialog::applySettings()
{
	// Check all pages
	for (auto &page : pages)
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

void SettingsDialog::categoryChanged(int row)
{
	stack->setCurrentIndex(row);
}
