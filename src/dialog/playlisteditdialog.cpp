#include "playlisteditdialog.hpp"

PlaylistEditDialog::PlaylistEditDialog(lib::spt::api &spotify, const lib::spt::playlist &playlist,
	int selectedIndex, QWidget *parent)
	: spotify(spotify),
	playlist(playlist),
	QDialog(parent)
{
	const auto &playlistName = QString::fromStdString(playlist.name);
	const auto &playlistDescription = QString::fromStdString(playlist.description);

	setWindowTitle(playlistName);
	setModal(true);
	auto *layout = new QVBoxLayout(this);

	// Name
	layout->addWidget(new QLabel("Name:", this));
	name = new QLineEdit(playlistName, this);
	layout->addWidget(name);

	// Description
	layout->addWidget(new QLabel("Description:", this));
	description = new QTextEdit(playlistDescription, this);
	layout->addWidget(description);

	// Toggles
	auto *toggles = new QHBoxLayout();
	toggles->setAlignment(Qt::AlignLeft);
	isPublic = new QCheckBox("Public", this);
	isPublic->setChecked(playlist.is_public);
	toggles->addWidget(isPublic);
	isCollaborative = new QCheckBox("Collaborative", this);
	isCollaborative->setChecked(playlist.collaborative);
	toggles->addWidget(isCollaborative);
	layout->addLayout(toggles);

	// Focus depending on selected index
	if (selectedIndex < 0)
	{
		setFocus();
	}
	else if (selectedIndex == 0)
	{
		name->setFocus();
	}
	else if (selectedIndex == 1)
	{
		description->setFocus();
	}

	// Dialog buttons
	auto *buttons = new QDialogButtonBox(this);
	buttons->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	QDialogButtonBox::connect(buttons, &QDialogButtonBox::accepted,
		this, &PlaylistEditDialog::yes);

	QDialogButtonBox::connect(buttons, &QDialogButtonBox::rejected,
		this, &PlaylistEditDialog::no);

	layout->addWidget(buttons);
	setLayout(layout);
}

void PlaylistEditDialog::yes()
{
	lib::spt::playlist_details pl = (nlohmann::json) playlist;
	pl.name = name->text().toStdString();
	pl.description = description->toPlainText().toStdString();
	pl.is_public = isPublic->isChecked();
	pl.collaborative = isCollaborative->isChecked();

	spotify.edit_playlist(playlist.id, pl, [this](const std::string &result)
	{
		if (result.empty())
		{
			accept();
			return;
		}

		QMessageBox::warning(this, "Edit failed",
			QString("Failed to save changes:\n%1")
				.arg(QString::fromStdString(result)));
	});
}

void PlaylistEditDialog::no()
{
	reject();
}
