#include "artistview.hpp"

ArtistView::ArtistView(spt::Spotify &spotify, const QString &artistId, QWidget *parent) : QDockWidget(parent)
{
	auto window = (MainWindow*) parent;

	auto artist = spotify.artist(artistId);
	setFeatures(QDockWidget::DockWidgetClosable);
	auto layout = new QVBoxLayout();
	layout->setContentsMargins(-1, 0, -1, 0);
	// Get cover image (320x320 -> 320x160)
	QPixmap cover;
	cover.loadFromData(window->get(artist.image), "jpeg");
	auto coverLabel = new QLabel(this);
	coverLabel->setPixmap(cover.copy(0, 80, 320, 160));
	layout->addWidget(coverLabel);
	// Artist name title
	auto title = new QLabel(artist.name, this);
	title->setAlignment(Qt::AlignHCenter);
	title->setWordWrap(true);
	auto titleFont = title->font();
	titleFont.setPointSize(24);
	title->setFont(titleFont);
	layout->addWidget(title);
	// Genres
	auto genres = new QLabel(artist.genres.join(", "));
	genres->setWordWrap(true);
	layout->addWidget(genres);
	// Tabs
	auto tabs = new QTabWidget(this);
	layout->addWidget(tabs);
	// Top tracks
	auto topTracks = artist.topTracks(spotify);
	QStringList topTrackIds;
	auto topTracksList = new QListWidget(tabs);
	for (auto &track : topTracks)
	{
		auto item = new QListWidgetItem(track.name, topTracksList);
		item->setIcon(QIcon(window->getAlbum(track.image)));
		item->setData(MainWindow::RoleTrackId, track.id);
		topTrackIds.append(QString("spotify:track:%1").arg(track.id));
	}
	QListWidget::connect(topTracksList, &QListWidget::itemClicked, [this, topTrackIds, &spotify, window](QListWidgetItem *item) {
		auto result =  spotify.playTracks(
			QString("spotify:track:%1").arg(item->data(MainWindow::RoleTrackId).toString()), topTrackIds);
		if (!result.isEmpty())
			window->setStatus(QString("Failed to start playback: %1").arg(result));
	});
	tabs->addTab(topTracksList, "Popular");
	// Albums
	auto albums = artist.albums(spotify);
	auto albumList = new QListWidget(tabs);
	auto singleList = new QListWidget(tabs);
	for (auto &album : albums)
	{
		auto parent = album.albumGroup == "single" ? singleList : albumList;
		auto year = album.releaseDate.toString("yyyy");
		auto item = new QListWidgetItem(QString("%1%2")
			.arg(album.name)
			.arg(year.isEmpty() ? "" : QString(" (%1)").arg(year)), parent);
		item->setIcon(QIcon(window->getAlbum(album.image)));
		item->setData(MainWindow::RoleAlbumId, album.id);
	}
	auto loadAlbumId = [this, window](QListWidgetItem *item) {
		if (!window->loadAlbum(item->data(MainWindow::RoleAlbumId).toString(), false))
			window->setStatus(QString("Failed to load album"));
	};
	QListWidget::connect(albumList,  &QListWidget::itemClicked, loadAlbumId);
	QListWidget::connect(singleList, &QListWidget::itemClicked, loadAlbumId);
	tabs->addTab(albumList,  "Albums");
	tabs->addTab(singleList, "Singles");
	// Related artists
	auto relatedArtists = artist.relatedArtists(spotify);
	auto relatedList = new QListWidget(tabs);
	for (auto &related : relatedArtists)
	{
		auto item = new QListWidgetItem(related.name, relatedList);
		item->setData(MainWindow::RoleArtistId, related.id);
	}
	QListWidget::connect(relatedList, &QListWidget::itemClicked, [this, relatedList, window](QListWidgetItem *item) {
		relatedList->setEnabled(false);
		window->openArtist(item->data(MainWindow::RoleArtistId).toString());
		close();
	});
	tabs->addTab(relatedList, "Related");
	// Rest of dock
	setWidget(MainWindow::layoutToWidget(layout));
	setFixedWidth(320);
}