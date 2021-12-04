#include "createplaylistdialog.hpp"

#include <QFormLayout>
#include <QDialogButtonBox>
#include <QPushButton>

CreatePlaylistDialog::CreatePlaylistDialog(QWidget *parent)
	: QDialog(parent)
{
	auto *layout = new QVBoxLayout();
	setLayout(layout);
	setWindowTitle("Create new playlist");

	auto *formsWidget = new QWidget();
	auto *formsLayout = new QFormLayout(this);
	formsWidget->setLayout(formsLayout);

	name_editfield = new QLineEdit;
	description_editfield = new QLineEdit;

	formsLayout->addRow("Name", name_editfield);
	formsLayout->addRow("Description", description_editfield);

	layout->addWidget(formsWidget);

	auto *buttons = new QDialogButtonBox();

	auto *okButton = buttons->addButton(QDialogButtonBox::Ok);
	QPushButton::connect(okButton, &QPushButton::clicked, [this](bool /*checked*/)
	{
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
