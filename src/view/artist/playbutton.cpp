#include "view/artist/playbutton.hpp"
#include "mainwindow.hpp"

View::Artist::PlayButton::PlayButton(lib::spt::api &spotify, const lib::spt::artist &artist,
	QWidget *parent)
	: spotify(spotify),
	artist(artist),
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

	popularity = menu->addAction(Icon::get("draw-donut"), "popularity");
	popularity->setEnabled(false);

	follow = menu->addAction(Icon::get("non-starred-symbolic"), "Follow");
	follow->setEnabled(false);
	QAction::connect(follow, &QAction::triggered,
		this, &View::Artist::PlayButton::onFollow);

	menu->addMenu(new View::Artist::SearchMenu(artist, menu));
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

void View::Artist::PlayButton::onClicked(bool /*checked*/)
{
	const auto uri = lib::spt::api::to_uri("artist", artist.id);
	spotify.play_tracks(uri, {});
}

void View::Artist::PlayButton::onFollow(bool /*checked*/)
{
	auto isFollowing = follow->text().contains("Unfollow");
	updateFollow(!isFollowing);

	auto callback = [this, isFollowing](const std::string &status)
	{
		if (status.empty())
		{
			return;
		}

		auto *mainWindow = MainWindow::find(this->parentWidget());
		if (mainWindow != nullptr)
		{
			mainWindow->status(lib::fmt::format("Failed to {}: {}",
				isFollowing ? "unfollow" : "follow", status));
		}
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
