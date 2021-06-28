#include "contextcontent.hpp"

View::ContextContent::ContextContent(lib::spt::api &spotify, spt::Current &current,
	const lib::cache &cache, QWidget *parent)
	: spotify(spotify),
	current(current),
	cache(cache),
	QWidget(parent)
{
	auto *layout = new QHBoxLayout(this);
	layout->setSpacing(12);

	album = new QLabel(this);
	album->setFixedSize(64, 64);

	layout->addWidget(album);
	nowPlaying = new QLabel(this);
	nowPlaying->setWordWrap(true);
	layout->addWidget(nowPlaying);

	reset();

	// Show menu when clicking now playing
	if (nowPlaying != nullptr)
	{
		nowPlaying->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
		QLabel::connect(nowPlaying, &QWidget::customContextMenuRequested,
			this, &View::ContextContent::onSongMenu);
	}
}

void View::ContextContent::onSongMenu(const QPoint &pos)
{
	if (nowPlaying == nullptr)
	{
		return;
	}

	auto track = current.playback.item;
	if (track.name.empty()
		&& track.artists.empty())
	{
		return;
	}

	auto *menu = new SongMenu(track, spotify, cache, parentWidget());
	menu->popup(nowPlaying->mapToGlobal(pos));
}

void View::ContextContent::reset()
{
	if (album != nullptr)
	{
		album->setPixmap(Icon::get("media-optical-audio").pixmap(album->size()));
	}

	if (nowPlaying != nullptr)
	{
		nowPlaying->setText("No music playing");
	}
}

void View::ContextContent::setAlbum(const QPixmap &pixmap)
{
	if (album != nullptr)
	{
		album->setPixmap(ImageUtils::mask(pixmap));
	}
}

auto View::ContextContent::getCurrentlyPlaying() const -> const lib::spt::track &
{
	return currentlyPlaying;
}

void View::ContextContent::setCurrentlyPlaying(const lib::spt::track &track)
{
	if (nowPlaying != nullptr)
	{
		nowPlaying->setText(QString::fromStdString(track.details()));
	}
	currentlyPlaying = track;
}
