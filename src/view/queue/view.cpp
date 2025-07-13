#include "view/queue/view.hpp"

#include "util/icon.hpp"
#include "lib/log.hpp"
#include "lib/spotify/util.hpp"
#include "widget/statusmessage.hpp"

#include <QHeaderView>
#include <QTimer>
#include <QStackedWidget>
#include <QMenu>
#include <QContextMenuEvent>
#include <QApplication>
#include <QClipboard>
#include <QFont>

Queue::View::View(lib::spt::api &spotify, lib::settings &settings, QWidget *parent)
	: QDockWidget("Queue", parent),
	spotify(spotify),
	settings(settings),
	isQueueEmpty(true) // Initialize as empty until we know otherwise
{
	setObjectName("QueueDock");
	setAllowedAreas(Qt::AllDockWidgetAreas);
	setFeatures(QDockWidget::DockWidgetMovable); // Only allow moving, remove close and undock buttons
	setupUi();
	startAutoRefresh();
}

Queue::View::~View()
{
	stopAutoRefresh();
}

void Queue::View::setupUi()
{
	// Create central widget for the dock widget
	auto *centralWidget = new QWidget(this);
	setWidget(centralWidget);
	
	auto *layout = new QVBoxLayout(centralWidget);
	layout->setContentsMargins(0, 0, 0, 0);

	// Create stacked widget to maintain consistent size
	stackedWidget = new QStackedWidget(centralWidget);
	layout->addWidget(stackedWidget);

	// Status label
	statusLabel = new QLabel(QStringLiteral("Loading queue..."), centralWidget);
	statusLabel->setAlignment(Qt::AlignCenter);
	statusLabel->setStyleSheet(QStringLiteral("QLabel { color: gray; padding: 10px; }"));
	statusLabel->setWordWrap(true);

	// Track list
	trackList = new QListWidget(centralWidget);
	trackList->setAlternatingRowColors(true);
	trackList->setSelectionMode(QAbstractItemView::SingleSelection);
	trackList->setContextMenuPolicy(Qt::CustomContextMenu);

	// Add both widgets to the stacked widget
	stackedWidget->addWidget(statusLabel);
	stackedWidget->addWidget(trackList);

	// Set minimum size to prevent resizing
	centralWidget->setMinimumWidth(250);
	centralWidget->setMinimumHeight(300);

	// Set up refresh timer
	refreshTimer = new QTimer(this);
	refreshTimer->setSingleShot(false);
	refreshTimer->setInterval(2000); // Default interval - will be adjusted based on queue state

	// Connect signals
	QWidget::connect(trackList, &QListWidget::itemClicked,
		this, &Queue::View::onItemClicked);

	QWidget::connect(trackList, &QListWidget::itemDoubleClicked,
		this, &Queue::View::onItemDoubleClicked);

	QWidget::connect(trackList, &QListWidget::customContextMenuRequested,
		this, &Queue::View::onContextMenu);

	QWidget::connect(refreshTimer, &QTimer::timeout,
		this, &Queue::View::refreshQueue);

	// Start with status message showing
	showStatusMessage(QStringLiteral("Loading queue..."));
}

void Queue::View::refreshQueue()
{
	// Only show refreshing message if we don't already have content and it's not already showing an empty state
	// Also avoid showing it if we know the queue is empty to prevent flickering
	if (trackList->count() == 0 && statusLabel->text() != QStringLiteral("No tracks in queue") && !isQueueEmpty)
	{
		showStatusMessage(QStringLiteral("Refreshing queue..."));
	}

	spotify.queue([this](const lib::result<lib::spt::queue> &result)
	{
		if (!result.success())
		{
			showStatusMessage(QString::fromStdString(result.message()));
			return;
		}

		const auto &queue = result.value();
		
		// Store the queue for skip operations
		currentQueue = queue;
		
		// Check if queue is effectively empty (no currently playing and no upcoming tracks)
		const bool queueEmpty = !queue.currently_playing.is_valid() && queue.tracks.empty();
		
		// Adjust refresh interval based on queue state
		if (queueEmpty != isQueueEmpty)
		{
			isQueueEmpty = queueEmpty;
			if (isQueueEmpty)
			{
				// Queue is empty - refresh less frequently to avoid distraction
				refreshTimer->setInterval(5000); // 5 seconds
			}
			else
			{
				// Queue has content - refresh more frequently for responsiveness
				refreshTimer->setInterval(2000); // 2 seconds
			}
		}
		
		// Always rebuild to ensure we have the latest queue content
		// The queue content can change even if the count stays the same
		trackList->clear();

		// Add currently playing track at the top if available
		if (queue.currently_playing.is_valid())
		{
			auto *currentItem = new QListWidgetItem();
			
			// Store data for currently playing track (0 skips needed - already playing)
			currentItem->setData(Qt::UserRole, 0);
			
			// Store track name and artist name for format updates
			currentItem->setData(Qt::UserRole + 1, QString::fromStdString(queue.currently_playing.name));
			if (!queue.currently_playing.artists.empty())
			{
				currentItem->setData(Qt::UserRole + 2, QString::fromStdString(queue.currently_playing.artists.front().name));
			}
			
			// Store track URI
			currentItem->setData(Qt::UserRole + 3, QString::fromStdString(queue.currently_playing.id));
			
			// Mark as currently playing
			currentItem->setData(Qt::UserRole + 4, true);
			
			// Format display text
			updateItemText(currentItem);
			
			// Set play icon for currently playing track
			currentItem->setIcon(Icon::get(QStringLiteral("media-playback-start")));
			
			// Make the text bold to distinguish it
			QFont font = currentItem->font();
			font.setBold(true);
			currentItem->setFont(font);
			
			trackList->addItem(currentItem);
		}

		// Check if queue is empty (no upcoming tracks)
		if (queue.tracks.empty())
		{
			// If we have a currently playing track, show it, otherwise show empty message
			if (!queue.currently_playing.is_valid())
			{
				showStatusMessage(QStringLiteral("No tracks in queue"));
				return;
			}
		}

		// Populate track list with upcoming tracks
		for (size_t i = 0; i < queue.tracks.size(); i++)
		{
			const auto &track = queue.tracks.at(i);
			auto *item = new QListWidgetItem();
			
			// Store the number of skips needed (position + 1)
			item->setData(Qt::UserRole, static_cast<int>(i + 1));
			
			// Store track name and artist name for format updates
			item->setData(Qt::UserRole + 1, QString::fromStdString(track.name));
			if (!track.artists.empty())
			{
				item->setData(Qt::UserRole + 2, QString::fromStdString(track.artists.front().name));
			}
			
			// Store track URI for removal from queue
			item->setData(Qt::UserRole + 3, QString::fromStdString(track.id));
			
			// Mark as not currently playing
			item->setData(Qt::UserRole + 4, false);
			
			// Format display text
			updateItemText(item);

			// No icon for upcoming tracks - only the currently playing track has an icon
			
			trackList->addItem(item);
		}

		// Show the track list
		showTrackList();
	});
}

void Queue::View::skipTracks(int skips)
{
	if (skips <= 0)
	{
		return;
	}

	// Check if skip index is valid
	if (static_cast<size_t>(skips - 1) >= currentQueue.tracks.size())
	{
		lib::log::error("Skip index {} exceeds queue size {}", skips - 1, currentQueue.tracks.size());
		return;
	}

	// Limit maximum skips to prevent excessive API calls
	const int maxSkips = 15; // Reasonable limit
	if (skips > maxSkips)
	{
		lib::log::warn("Limiting skip operation from {} to {} tracks", skips, maxSkips);
		StatusMessage::warn(QString("Can only skip up to %1 tracks at once").arg(maxSkips));
		skips = maxSkips;
	}

	// Use the next API to skip, but with improved error handling
	// This preserves the queue context better than play_tracks
	skipToTrackRecursive(skips, 0);
}

void Queue::View::skipToTrackRecursive(int totalSkips, int currentSkip)
{
	if (currentSkip >= totalSkips)
	{
		// We've skipped enough tracks, refresh the queue
		QTimer::singleShot(800, this, &Queue::View::refreshQueue);
		return;
	}

	spotify.next([this, totalSkips, currentSkip](const std::string &result)
	{
		if (!result.empty())
		{
			lib::log::error("Failed to skip track {}/{}: {}", currentSkip + 1, totalSkips, result);
			
			// Show error only on first failure to avoid spam
			if (currentSkip == 0)
			{
				StatusMessage::error(QString("Failed to skip to track: %1")
					.arg(QString::fromStdString(result)));
			}
			
			// Still try to refresh the queue to show current state
			QTimer::singleShot(800, this, &Queue::View::refreshQueue);
			return;
		}

		// Continue with next skip
		skipToTrackRecursive(totalSkips, currentSkip + 1);
	});
}

void Queue::View::onItemClicked(QListWidgetItem *item)
{
	// Single click just selects the item, no action taken
	Q_UNUSED(item)
}

void Queue::View::onItemDoubleClicked(QListWidgetItem *item)
{
	if (item == nullptr)
	{
		return;
	}

	// Check if this is the currently playing track
	const bool isCurrentlyPlaying = item->data(Qt::UserRole + 4).toBool();
	if (isCurrentlyPlaying)
	{
		// Don't try to skip to the currently playing track
		return;
	}

	const auto skips = item->data(Qt::UserRole).toInt();
	if (skips > 0)
	{
		skipTracks(skips);
	}
}

void Queue::View::startAutoRefresh()
{
	refreshQueue(); // Initial refresh
	refreshTimer->start();
}

void Queue::View::stopAutoRefresh()
{
	refreshTimer->stop();
}

void Queue::View::refreshFormat()
{
	// Update formatting of existing items without fetching new data
	for (int i = 0; i < trackList->count(); ++i)
	{
		auto *item = trackList->item(i);
		if (item != nullptr)
		{
			updateItemText(item);
		}
	}
}

void Queue::View::updateItemText(QListWidgetItem *item)
{
	if (item == nullptr)
	{
		return;
	}

	const QString songTitle = item->data(Qt::UserRole + 1).toString();
	const QString artistName = item->data(Qt::UserRole + 2).toString();
	
	QString displayText;
	if (!artistName.isEmpty())
	{
		if (settings.general.queue_swap_name_format)
		{
			// Artist - Song format
			displayText = QString("%1 - %2").arg(artistName).arg(songTitle);
		}
		else
		{
			// Song - Artist format (default)
			displayText = QString("%1 - %2").arg(songTitle).arg(artistName);
		}
	}
	else
	{
		displayText = songTitle;
	}
	
	item->setText(displayText);
	item->setToolTip(displayText);
}

void Queue::View::showTrackList()
{
	stackedWidget->setCurrentWidget(trackList);
}

void Queue::View::showStatusMessage(const QString &message)
{
	statusLabel->setText(message);
	stackedWidget->setCurrentWidget(statusLabel);
}

void Queue::View::onContextMenu(const QPoint &position)
{
	auto *item = trackList->itemAt(position);
	if (item == nullptr)
	{
		return;
	}

	auto *menu = new QMenu(this);

	// Check if this is the currently playing track
	const bool isCurrentlyPlaying = item->data(Qt::UserRole + 4).toBool();
	const auto skips = item->data(Qt::UserRole).toInt();
	const auto trackName = item->data(Qt::UserRole + 1).toString();
	
	if (!isCurrentlyPlaying && skips > 0)
	{
		// Skip to this track (only for upcoming tracks)
		auto *skipAction = menu->addAction(Icon::get(QStringLiteral("media-skip-forward")), 
			QString("Skip to song"));

		QAction::connect(skipAction, &QAction::triggered, [this, skips]()
		{
			skipTracks(skips);
		});

		menu->addSeparator();
	}
	else if (isCurrentlyPlaying)
	{
		// For currently playing track, show it's playing
		auto *playingAction = menu->addAction(Icon::get(QStringLiteral("media-playback-start")), 
			QStringLiteral("Currently Playing"));
		playingAction->setEnabled(false);
		
		menu->addSeparator();
	}

	// Remove from queue - show that it's not possible
	auto *removeAction = menu->addAction(Icon::get(QStringLiteral("list-remove")), 
		QStringLiteral("Remove from queue"));
	removeAction->setEnabled(false);
	removeAction->setToolTip(QStringLiteral("Spotify API does not support removing tracks from queue"));

	// Copy track info
	menu->addSeparator();
	const auto artistName = item->data(Qt::UserRole + 2).toString();
	const auto displayName = !artistName.isEmpty() 
		? QString("%1 - %2").arg(trackName).arg(artistName)
		: trackName;
	
	auto *copyAction = menu->addAction(Icon::get(QStringLiteral("edit-copy")), 
		QStringLiteral("Copy track name"));
	
	QAction::connect(copyAction, &QAction::triggered, [displayName]()
	{
		QApplication::clipboard()->setText(displayName);
		StatusMessage::info(QStringLiteral("Track name copied to clipboard"));
	});

	menu->popup(trackList->mapToGlobal(position));
}

void Queue::View::onRemoveFromQueue()
{
	// This method exists for interface compatibility but cannot be implemented
	// because the Spotify Web API does not provide a way to remove tracks from the queue
	StatusMessage::warn(QStringLiteral("Spotify API does not support removing tracks from queue"));
}
