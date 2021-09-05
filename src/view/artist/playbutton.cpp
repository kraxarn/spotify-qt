#include "view/artist/playbutton.hpp"
#include "mainwindow.hpp"

View::Artist::PlayButton::PlayButton(lib::spt::api &spotify,
	const lib::http_client &httpClient, QWidget *parent)
	: spotify(spotify),
	httpClient(httpClient),
	QToolButton(parent)
{
	setEnabled(false);
	setIcon(Icon::get("media-playback-start"));
	setMenu(contextMenu());
	setPopupMode(QToolButton::MenuButtonPopup);

	QAbstractButton::connect(this, &QAbstractButton::clicked,
		this, &View::Artist::PlayButton::onClicked);
}

auto View::Artist::PlayButton::contextMenu() -> QMenu *
{
	auto *menu = new QMenu(this);

	popularity = menu->addAction(Icon::get("draw-donut"), "... popularity");
	popularity->setEnabled(false);

	follow = menu->addAction(Icon::get("non-starred-symbolic"), "Follow");
	follow->setEnabled(false);
	QAction::connect(follow, &QAction::triggered,
		this, &View::Artist::PlayButton::onFollow);

	menu->addMenu(new Menu::ArtistLinks(artist, httpClient, menu));
	menu->addMenu(new View::Artist::ShareMenu(artist, menu));

	return menu;
}

void View::Artist::PlayButton::updateFollow(bool isFollowing)
{
	follow->setIcon(Icon::get(QString("%1starred-symbolic")
		.arg(isFollowing ? "" : "non-")));

	follow->setText(QString("%1%2")
		.arg(isFollowing ? "Unfollow" : "Follow", follow->text()
			.right(follow->text().length() - follow->text().indexOf(' '))));
}

void View::Artist::PlayButton::setArtist(const lib::spt::artist &loadedArtist)
{
	artist = loadedArtist;

	const auto iconImage = Icon::get("draw-donut").pixmap(64, 64);
	const auto masked = ImageUtils::mask(iconImage, MaskShape::Pie,
		QVariant(artist.popularity));

	popularity->setIcon(QIcon(masked));
	popularity->setText(QString("%1% popularity").arg(artist.popularity));

	auto followers = lib::fmt::format("Follow ({} follower{})",
		lib::fmt::count(artist.followers),
		artist.followers == 1 ? "" : "s");
	follow->setText(QString::fromStdString(followers));

	spotify.is_following(lib::follow_type::artist, {artist.id},
		[this](const std::vector<bool> &follows)
		{
			this->updateFollow(!follows.empty() && follows.at(0));
			this->follow->setEnabled(true);
		});

	setEnabled(true);
}

void View::Artist::PlayButton::onClicked(bool /*checked*/)
{
	const auto uri = lib::spt::api::to_uri("artist", artist.id);
	spotify.play_tracks(uri, {});
}

void View::Artist::PlayButton::onFollow(bool /*checked*/)
{
	auto isFollowing = follow->text().contains("Unfollow");
	updateFollow(!isFollowing);

	auto callback = [isFollowing](const std::string &status)
	{
		if (status.empty())
		{
			return;
		}

		StatusMessage::error(QString("Failed to %1: %2")
			.arg(isFollowing ? "unfollow" : "follow")
			.arg(QString::fromStdString(status)));
	};

	if (isFollowing)
	{
		spotify.unfollow(lib::follow_type::artist,
			{artist.id}, callback);
	}
	else
	{
		spotify.follow(lib::follow_type::artist,
			{artist.id}, callback);
	}
}
