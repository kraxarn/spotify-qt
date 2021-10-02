#include "view/search/shows.hpp"
#include "mainwindow.hpp"

View::Search::Shows::Shows(lib::spt::api &spotify,
	QWidget *parent)
	: QListWidget(parent),
	spotify(spotify)
{
	QListWidget::connect(this, &QListWidget::itemClicked,
		this, &View::Search::Shows::onItemClicked);

	setContextMenuPolicy(Qt::CustomContextMenu);
	QWidget::connect(this, &QWidget::customContextMenuRequested,
		this, &View::Search::Shows::onContextMenu);
}

void View::Search::Shows::add(const lib::spt::show &show)
{
	auto id = QString::fromStdString(show.id);
	auto name = QString::fromStdString(show.name);

	auto *item = new QListWidgetItem(name, this);
	item->setData(static_cast<int>(DataRole::ShowId), id);
	item->setToolTip(name);
}

void View::Search::Shows::onItemClicked(QListWidgetItem *item)
{
	auto showId = item->data(static_cast<int>(DataRole::ShowId))
		.toString()
		.toStdString();

	auto *mainWindow = MainWindow::find(parentWidget());
	auto *tracksList = mainWindow->getSongsTree();
	tracksList->setEnabled(false);

	spotify.show(showId, [this, mainWindow, tracksList](const lib::spt::show &show)
	{
		spotify.show_episodes(show,
			[show, mainWindow, tracksList](const std::vector<lib::spt::episode> &episodes)
			{
				// Pretend episodes are tracks before showing in list
				std::vector<lib::spt::track> tracks;
				tracks.reserve(episodes.size());
				for (const auto &episode: episodes)
				{
					tracks.insert(tracks.begin(), episode.to_track(show));
				}

				mainWindow->setSptContext(show);
				tracksList->load(tracks);
				tracksList->setEnabled(true);
			});
	});
}

void View::Search::Shows::onContextMenu(const QPoint &pos)
{
	auto *item = itemAt(pos);
	auto showId = item->data(static_cast<int>(DataRole::ShowId))
		.toString().toStdString();

	spotify.show(showId, [this, pos](const lib::spt::show &show)
	{
		auto *menu = new Menu::Show(spotify, show, parentWidget());
		menu->popup(mapToGlobal(pos));
	});
}
