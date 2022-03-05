#include "dialog/duplicateplaylisttrack.hpp"

#include <QPushButton>

Dialog::DuplicatePlaylistTrack::DuplicatePlaylistTrack(QWidget *parent)
	: Base(parent),
	action(DuplicateTrackResponse::None)
{
	setTitle(QStringLiteral("Duplicate"));

	auto *layout = Base::layout<QVBoxLayout>();

	// TODO: This doesn't make sense with a single track (both options do the same thing)
	const auto text = QStringLiteral(
		"One or more tracks are already in the destination playlist. "
		"Do you want to add all missing tracks, or add all tracks anyway?");

	auto *label = new QLabel(text, this);
	layout->addWidget(label);

	addAction(DialogAction::Cancel);

	auto *addMissing = addButton(QStringLiteral("Add missing"), QDialogButtonBox::AcceptRole);

	QPushButton::connect(addMissing, &QPushButton::clicked,
		this, &Dialog::DuplicatePlaylistTrack::onAddMissingClicked);

	auto *addAll = addButton(QStringLiteral("Add all"), QDialogButtonBox::ActionRole);

	QPushButton::connect(addAll, &QPushButton::clicked,
		this, &Dialog::DuplicatePlaylistTrack::onAddAllClicked);
}

auto Dialog::DuplicatePlaylistTrack::response() const -> DuplicateTrackResponse
{
	return action;
}

void Dialog::DuplicatePlaylistTrack::onCancel(bool checked)
{
	action = DuplicateTrackResponse::None;
	Base::onCancel(checked);
}

void Dialog::DuplicatePlaylistTrack::onAddMissingClicked(bool /*checked*/)
{
	action = DuplicateTrackResponse::AddMissing;
	accept();
}

void Dialog::DuplicatePlaylistTrack::onAddAllClicked(bool /*checked*/)
{
	action = DuplicateTrackResponse::AddAll;
	accept();
}
