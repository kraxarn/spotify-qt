#include "view/context/titleinfo.hpp"

#include "mainwindow.hpp"
#include "lib/developermode.hpp"
#include "util/icon.hpp"

#include <QMenu>

Context::TitleInfo::TitleInfo(lib::spt::api &spotify, QWidget *parent)
	: QLabel(parent),
	spotify(spotify)
{
	setToolTip(QStringLiteral("Currently playing from"));
	setVisible(false);
	setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);

	setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	QWidget::connect(this, &QWidget::customContextMenuRequested,
		this, &Context::TitleInfo::onContextMenu);

	auto *mainWindow = MainWindow::find(parent);
	MainWindow::connect(mainWindow, &MainWindow::playbackRefreshed,
		this, &Context::TitleInfo::onPlaybackRefreshed);
}

auto Context::TitleInfo::getIcon() const -> QIcon
{
	return Icon::getByType(playback.context.type);
}

void Context::TitleInfo::onContextMenu(const QPoint &pos)
{
	auto *menu = new QMenu(this);

	if (lib::developer_mode::enabled)
	{
		const auto uri = QString::fromStdString(playback.context.uri);
		auto *devContext = menu->addAction(uri);
		devContext->setEnabled(false);
	}

	auto *open = menu->addAction(getIcon(), QString("Open %1")
		.arg(QString::fromStdString(playback.context.type)));

	QAction::connect(open, &QAction::triggered,
		this, &Context::TitleInfo::onContextMenuTriggered);

	menu->popup(mapToGlobal(pos));
}

void Context::TitleInfo::onContextMenuTriggered(bool /*checked*/)
{
	auto *mainWindow = MainWindow::find(parentWidget());
	const auto &type = playback.context.type;
	const auto uri = lib::strings::split(playback.context.uri, ':').back();

	if (type == "album")
	{
		mainWindow->loadAlbum(uri);
	}
	else if (type == "artist")
	{
		mainWindow->openArtist(uri);
	}
	else if (type == "playlist")
	{
		spotify.playlist(uri, [mainWindow](const lib::result<lib::spt::playlist> &result)
		{
			if (!result.success())
			{
				StatusMessage::error(QString("Failed to load playlist: %1")
					.arg(QString::fromStdString(result.message())));

				return;
			}

			mainWindow->resetLibraryPlaylist();
			mainWindow->getSongsTree()->load(result.value());
		});
	}
}

void Context::TitleInfo::onPlaybackRefreshed(const lib::spt::playback &refreshed,
	const lib::spt::playback &/*previous*/)
{
	playback = refreshed;
}

void Context::TitleInfo::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::MouseButton::LeftButton)
	{
		onContextMenuTriggered(false);
	}

	QLabel::mouseReleaseEvent(event);
}
