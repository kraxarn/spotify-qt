#include "menu/addtoplaylist.hpp"
#include "mainwindow.hpp"
#include "util/icon.hpp"
#include "dialog/createplaylist.hpp"
#include "dialog/addtoplaylist.hpp"

Menu::AddToPlaylist::AddToPlaylist(std::vector<std::string> trackIds, lib::spt::api &spotify,
	const lib::cache &cache, QWidget *parent)
	: QMenu(parent),
	trackIds(std::move(trackIds)),
	spotify(spotify),
	cache(cache)
{
	setIcon(Icon::get(QStringLiteral("list-add")));
	setTitle(QStringLiteral("Add to playlist"));

	auto *newPlaylist = addAction(QStringLiteral("New playlist"));
	newPlaylist->setData({});

	QMenu::connect(this, &QMenu::aboutToShow,
		this, &Menu::AddToPlaylist::onAboutToShow);

	QMenu::connect(this, &QMenu::triggered,
		this, &Menu::AddToPlaylist::onTriggered);
}

void Menu::AddToPlaylist::onAboutToShow()
{
	auto *mainWindow = MainWindow::find(parentWidget());
	const auto &currentUserId = mainWindow->getCurrentUser().id;

	addSeparator();
	for (auto &playlist: cache.get_playlists())
	{
		if (playlist.owner_id != currentUserId)
		{
			continue;
		}

		// Create main action
		auto *action = addAction(QString::fromStdString(playlist.name));
		action->setData(QVariant::fromValue(playlist));
	}
}

void Menu::AddToPlaylist::onTriggered(QAction *action)
{
	const auto &data = action->data();

	if (data.canConvert<lib::spt::playlist>())
	{
		addToPlaylist(data.value<lib::spt::playlist>());
	}
	else
	{
		addToNewPlaylist();
	}
}

void Menu::AddToPlaylist::addToNewPlaylist()
{
	auto *createPlaylist = new Dialog::CreatePlaylist(trackIds, spotify, window());
	createPlaylist->open();
}

void Menu::AddToPlaylist::addToPlaylist(const lib::spt::playlist &playlist)
{
	Dialog::AddToPlaylist::ask(spotify, playlist, trackIds, window());
}
