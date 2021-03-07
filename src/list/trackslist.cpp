#include "trackslist.hpp"

TracksList::TracksList(spt::Spotify &spotify, lib::settings &settings, QWidget *parent)
	: spotify(spotify),
	settings(settings),
	QTreeWidget(parent)
{
	setEditTriggers(QAbstractItemView::NoEditTriggers);
	setSelectionBehavior(QAbstractItemView::SelectRows);
	setSortingEnabled(true);
	setRootIsDecorated(false);
	setAllColumnsShowFocus(true);
	setColumnCount(5);
	setHeaderLabels({
		settings.general.track_numbers == lib::context_all ? "#" : "",
		"Title", "Artist", "Album", "Length", "Added"
	});
	header()->setSectionsMovable(false);
	header()->setSortIndicator(settings.general.song_header_sort_by + 1, Qt::AscendingOrder);

	updateResizeMode(settings.general.track_list_resize_mode);

	// Hide specified columns
	for (auto &value : settings.general.hidden_song_headers)
		header()->setSectionHidden(value + 1, true);

	// Play tracks on click or enter/special key
	QTreeWidget::connect(this, &QTreeWidget::itemActivated, this, &TracksList::clicked);

	// Song context menu
	setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	QWidget::connect(this, &QWidget::customContextMenuRequested, this, &TracksList::menu);

	// Songs header context menu
	header()->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	QLabel::connect(header(), &QWidget::customContextMenuRequested, this, &TracksList::headerMenu);
}

void TracksList::menu(const QPoint &pos)
{
	auto item = itemAt(pos);
	if (item == nullptr)
		return;

	auto trackId = item->data(0, RoleTrackId).toString();
	if (trackId.isEmpty())
		return;

	(new SongMenu(item, spotify, parentWidget()))->popup(mapToGlobal(pos));
}

void TracksList::clicked(QTreeWidgetItem *item, int)
{
	if (item->isDisabled())
		return;

	auto mainWindow = MainWindow::find(parentWidget());

	bool indexFound;
	auto trackIndex = item->data(0, RoleIndex).toInt(&indexFound);
	if (!indexFound)
	{
		mainWindow->setStatus("Failed to start playback: track not found", true);
		return;
	}

	auto callback = [mainWindow, item](const QString &status)
	{
		if (!status.isEmpty())
		{
			mainWindow->setStatus(QString("Failed to start playback: %1")
				.arg(status), true);
		}
		else
			mainWindow->setPlayingTrackItem(item);

		mainWindow->refresh();
	};

	// If we played from library, we don't have any context
	if (mainWindow->getCurrentLibraryItem() != nullptr)
	{
		auto allTracks = mainWindow->currentTracks();
		this->spotify.playTracks(currentIndex().row(), allTracks, callback);
	}
	else
		this->spotify.playTracks(trackIndex, mainWindow->getSptContext(), callback);
}

void TracksList::headerMenu(const QPoint &pos)
{
	auto menu = new QMenu(header());
	auto showHeaders = menu->addMenu(Icon::get("visibility"), "Columns to show");
	auto sortBy = menu->addMenu(Icon::get("view-sort-ascending"), "Default sorting");
	auto headerTitles = QStringList({
		"Title", "Artist", "Album", "Length", "Added"
	});
	auto headers = this->settings.general.hidden_song_headers;
	for (int i = 0; i < headerTitles.size(); i++)
	{
		auto showTitle = showHeaders->addAction(headerTitles.at(i));
		showTitle->setCheckable(true);
		showTitle->setChecked(std::find(headers.begin(), headers.end(), i) == headers.end());
		showTitle->setData(QVariant(i));

		auto sortTitle = sortBy->addAction(headerTitles.at(i));
		sortTitle->setCheckable(true);
		sortTitle->setChecked(i == this->settings.general.song_header_sort_by);
		sortTitle->setData(QVariant(100 + i));
	}

	QMenu::connect(menu, &QMenu::triggered, [this](QAction *action)
	{
		int i = action->data().toInt();

		// Columns to show
		if (i < 100)
		{
			header()->setSectionHidden(i + 1, !action->isChecked());
			if (action->isChecked())
			{
				this->settings.general.hidden_song_headers
					.erase(std::remove(this->settings.general.hidden_song_headers.begin(),
						this->settings.general.hidden_song_headers.end(), i));
			}
			else
				this->settings.general.hidden_song_headers.push_back(i);
			this->settings.save();
			return;
		}

		// Sort by
		i -= 100;
		if (this->settings.general.song_header_sort_by == i)
			i = -1;
		header()->setSortIndicator(i + 1, Qt::AscendingOrder);
		this->settings.general.song_header_sort_by = i;
		this->settings.save();
	});

	menu->popup(header()->mapToGlobal(pos));
}

void TracksList::resizeEvent(QResizeEvent *event)
{
	resizeHeaders(event->size());
}

void TracksList::resizeHeaders(const QSize &newSize)
{
	if (settings.general.track_list_resize_mode != lib::resize_auto)
		return;

	const int indexSize = 60;
	const int lengthSize = 70;
	const int addedSize = 140;
	auto size = (newSize.width() - indexSize - lengthSize - addedSize) / 7;

	if (size < 60)
		size = 60;

	// #
	header()->resizeSection(0, indexSize);
	// Title
	header()->resizeSection(1, size * 3);
	// Artist
	header()->resizeSection(2, size * 2);
	// Album
	header()->resizeSection(3, size * 2);
	// Length
	header()->resizeSection(4, lengthSize);
	// Added
	header()->resizeSection(5, addedSize);
}

void TracksList::updateResizeMode(lib::resize_mode mode)
{
	QHeaderView::ResizeMode resizeMode;

	switch (mode)
	{
		case lib::resize_auto:
			resizeMode = QHeaderView::Fixed;
			break;

		case lib::resize_fit_content:
			resizeMode = QHeaderView::ResizeToContents;
			break;

		case lib::resize_custom:
			resizeMode = QHeaderView::Interactive;
			break;

		default:
			resizeMode = header()->sectionResizeMode(0);
			break;
	}

	header()->setSectionResizeMode(resizeMode);
	resizeHeaders(size());
}