#include "menu/queue.hpp"

#include "util/icon.hpp"

Menu::Queue::Queue(lib::spt::api &spotify, QWidget *parent)
	: Base(parent),
	spotify(spotify)
{
	setTitle(QStringLiteral("Queue"));
	setIcon(Icon::get(QStringLiteral("media-track-show-active")));

	QMenu::connect(this, &QMenu::aboutToShow,
		this, &Menu::Queue::refreshQueue);

	QMenu::connect(this, &QMenu::triggered,
		this, &Menu::Queue::onTriggered);

	addMessage(QStringLiteral("Refreshing queue..."));
}

void Menu::Queue::refreshQueue()
{
	spotify.queue([this](const lib::result<lib::spt::queue> &result)
	{
		for (auto &action: actions())
		{
			removeAction(action);
		}

		if (!result.success())
		{
			addMessage(QString::fromStdString(result.message()));
			return;
		}

		const auto &queue = result.value();
		if (queue.tracks.empty())
		{
			addMessage(QStringLiteral("No tracks in queue"));
			return;
		}

		for (size_t i = 0; i < queue.tracks.size(); i++)
		{
			const auto &track = queue.tracks.at(i);
			auto *action = addAction(QString::fromStdString(track.title()));
			action->setData(static_cast<int>(i + 1));
		}
	});
}

void Menu::Queue::skipTracks(int skips)
{
	if (skips <= 0)
	{
		return;
	}

	spotify.next([this, skips](const std::string &callback)
	{
		if (!callback.empty())
		{
			lib::log::error("Failed to skip in queue: {}", callback);
			return;
		}

		skipTracks(skips - 1);
	});
}

void Menu::Queue::onTriggered(QAction *action)
{
	const auto skips = action->data().toInt();
	skipTracks(skips);
}
