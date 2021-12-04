#include "createplaylistdialog.hpp"

#include <QFormLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QMessageBox>

CreatePlaylistDialog::CreatePlaylistDialog(QWidget *parent)
	: QDialog(parent)
{
	auto *layout = new QVBoxLayout();
	setLayout(layout);
	setWindowTitle("Create new playlist");

	auto *formsWidget = new QWidget;
	auto *formsLayout = new QFormLayout;
	formsWidget->setLayout(formsLayout);

	name_editfield = new QLineEdit;
	description_editfield = new QLineEdit;
	public_checkbox = new QCheckBox;
	collaborative_checkbox = new QCheckBox;

	name_editfield->setPlaceholderText("Playlist name");
	description_editfield->setPlaceholderText("Playlist description");

	formsLayout->addRow("Name", name_editfield);
	formsLayout->addRow("Description", description_editfield);
	formsLayout->addRow("Public", public_checkbox);
	formsLayout->addRow("Collaborative", collaborative_checkbox);

	layout->addWidget(formsWidget);

	auto *buttons = new QDialogButtonBox();

	auto *okButton = buttons->addButton(QDialogButtonBox::Ok);
	QPushButton::connect(okButton, &QPushButton::clicked, [this](bool /*checked*/)
	{
		if (name_editfield->text().isEmpty()) {
			QMessageBox::information(
				this, "Information",
				"Please enter a playlist name. It may not be empty.");
			return;
		}

		accept();
	});

	auto *cancelButton = buttons->addButton(QDialogButtonBox::Cancel);
	QPushButton::connect(cancelButton, &QPushButton::clicked, [this](bool /*checked*/)
	{
		reject();
	});
	layout->addWidget(buttons);
}

auto CreatePlaylistDialog::playlistName() -> std::string
{
	return name_editfield->text().toStdString();
}

auto CreatePlaylistDialog::playlistDescription() -> std::string
{
	return description_editfield->text().toStdString();
}

auto CreatePlaylistDialog::playlistPublic() -> bool
{
	return public_checkbox->isChecked();
}

auto CreatePlaylistDialog::playlistCollaborative() -> bool
{
	return collaborative_checkbox->isChecked();
}
