#include "developermenu.hpp"
#include "mainwindow.hpp"
#include "lib/qtpaths.hpp"

#include "dialog/createplaylist.hpp"
#include "dialog/addtoplaylist.hpp"
#include "dialog/apirequest.hpp"
#include "dialog/passwordentry.hpp"
#include "dialog/memory.hpp"
#include "dialog/lyricssearch.hpp"
#include "dialog/disallows.hpp"
#include "dialog/editplaylist.hpp"
#include "dialog/jsondump.hpp"
#include "dialog/widgets.hpp"

DeveloperMenu::DeveloperMenu(lib::settings &settings, lib::spt::api &spotify,
	lib::cache &cache, const lib::http_client &httpClient, QWidget *parent)
	: QMenu("Developer", parent),
	settings(settings),
	spotify(spotify),
	cache(cache),
	httpClient(httpClient)
{
	setIcon(Icon::get("folder-txt"));

	addMenuItem(this, "Test API requests", [this]()
	{
		auto *mainWindow = MainWindow::find(parentWidget());
		auto *debugView = new Dialog::ApiRequest(this->settings, mainWindow);
		debugView->show();
	});

	addMenuItem(this, "Reset size", [this]()
	{
		MainWindow::find(parentWidget())->resize(MainWindow::defaultSize());
	});

	addMenuItem(this, "Refresh access token", [this]()
	{
		try
		{
			this->spotify.refresh(false);
			QMessageBox::information(this, "Success",
				QString::fromStdString(lib::fmt::format("Successfully refreshed access token:\n{}",
					this->settings.account.refresh_token)));
		}
		catch (const std::exception &e)
		{
			QMessageBox::critical(this, "Error",
				QString("Refresh failed: %1").arg(e.what()));
		}
	});

	addMenuItem(this, "Load huge playlist (very slow)", [this]()
	{
		// playlist id taken from https://github.com/librespot-org/librespot/issues/481
		this->spotify.playlist("0bQjBPRqy5zReJethJy3aP",
			[this](const lib::result<lib::spt::playlist> &result)
			{
				if (!result.success())
				{
					lib::log::error("Fetch failed: {}", result.message());
					return;
				}

				auto *mainWindow = MainWindow::find(this->parentWidget());
				mainWindow->getSongsTree()->load(result.value());
			});
	});

	addMenuItem(this, "Show track notification", [this]()
	{
		auto *mainWindow = MainWindow::find(parentWidget());

		auto *trayIcon = mainWindow->getTrayIcon();
		if (trayIcon == nullptr)
		{
			StatusMessage::warn(QStringLiteral("Tray icon not loaded"));
			return;
		}

		const auto &track = mainWindow->playback().item;

		Http::getAlbumImage(track.image_small(), this->httpClient, this->cache,
			[trayIcon, &track](const QPixmap &pixmap)
			{
				trayIcon->message(track, pixmap);
			});
	});

	addMenuItem(this, QStringLiteral("Reload context album"), [this]()
	{
		auto *mainWindow = MainWindow::find(parentWidget());
		auto *contextView = mainWindow->findChild<Context::View *>();
		contextView->reset();
	});

	addMenuItem(this, QStringLiteral("Docked widgets"), [this]()
	{
		auto *mainWindow = MainWindow::find(parentWidget());
		const auto json = nlohmann::json::parse(mainWindow->getDockedWidgets());
		auto *dialog = new Dialog::JsonDump(json, mainWindow);
		dialog->open();
	});

	addMenuItem(this, QStringLiteral("Widgets"), [this]()
	{
		auto *mainWindow = MainWindow::find(parentWidget());
		auto *debugView = new Dialog::Widgets(mainWindow);
		debugView->show();
	});

	addMenuItem(this, QStringLiteral("Update liked tracks"), [this]
	{
		const auto *mainWindow = MainWindow::find(parentWidget());
		auto *tracksList = mainWindow->findChild<List::Tracks *>();
		if (tracksList == nullptr)
		{
			StatusMessage::warn(QStringLiteral("Tracks list not found"));
			return;
		}
		tracksList->updateLikedTracks([](const std::vector<lib::spt::track> &tracks)
		{
			StatusMessage::info(QString("Updated %1 tracks").arg(tracks.size()));
		});
	});

	addMenu(infoMenu());
	addMenu(getDialogMenu());
	addMenu(crashMenu());
	addMenu(statusMenu());
}

void DeveloperMenu::addMenuItem(QMenu *menu, const QString &text,
	const std::function<void()> &triggered)
{
	auto *action = menu->addAction(text);
	QAction::connect(action, &QAction::triggered, [triggered](bool /*checked*/)
	{
		triggered();
	});
}

auto DeveloperMenu::getDialogMenu() -> QMenu *
{
	dialogMenu = new QMenu("Dialogs", this);

	QMenu::connect(dialogMenu, &QMenu::aboutToShow,
		this, &DeveloperMenu::onDialogMenuAboutToShow);

	return dialogMenu;
}

auto DeveloperMenu::infoMenu() -> QMenu *
{
	auto *menu = new QMenu("Current", this);
	auto *mainWindow = MainWindow::find(parentWidget());

	addMenuItem(menu, "Playback", [mainWindow]()
	{
		const nlohmann::json json = mainWindow->playback();
		QMessageBox::information(mainWindow, "Playback",
			QString::fromStdString(json.dump(4)));
	});

	addMenuItem(menu, "Playback (MPRIS)", [mainWindow]()
	{
		const nlohmann::json json = mainWindow->playback().metadata();
		QMessageBox::information(mainWindow, "Metadata",
			QString::fromStdString(json.dump(4)));
	});

	addMenuItem(menu, "Context", [mainWindow]()
	{
		QMessageBox::information(mainWindow, "Context",
			QString::fromStdString(mainWindow->history()->currentUri()));
	});

	return menu;
}

auto DeveloperMenu::crashMenu() -> QMenu *
{
	auto *menu = new QMenu("Crash", this);

	addMenuItem(menu, "exception", []()
	{
		throw std::runtime_error("debug crash");
	});

	addMenuItem(menu, "segfault", []()
	{
		// Do stuff with widget that hopefully doesn't exist
		// to avoid any "unexpected behaviour" warnings
		QWidget::find(-1)->update();
	});

	addMenuItem(menu, "qFatal", []()
	{
		qFatal("debug crash");
	});

	return menu;
}

auto DeveloperMenu::statusMenu() -> QMenu *
{
	auto *menu = new QMenu("Status message", this);

	addMenuItem(menu, "Information", []()
	{
		StatusMessage::show(MessageType::Information, "Information");
	});

	addMenuItem(menu, QStringLiteral("Information with action"), []()
	{
		StatusMessage::info(QStringLiteral("Information"), QStringLiteral("Action"), []()
		{
			StatusMessage::info(QStringLiteral("Information action"));
		});
	});

	addMenuItem(menu, QStringLiteral("Information with long message"), []()
	{
		const auto text = QStringLiteral(
			"This is a very long informational message just to make sure that "
			"messages can be very long without breaking the layout of the "
			"application or causing other elements to resize"
		);

		StatusMessage::show(MessageType::Information, text);
	});

	addMenuItem(menu, "Warning", []()
	{
		StatusMessage::show(MessageType::Warning, "Warning");
	});

	addMenuItem(menu, "Error", []()
	{
		StatusMessage::show(MessageType::Error, "Error");
	});

	return menu;
}

void DeveloperMenu::onDialogMenuAboutToShow()
{
	if (!dialogMenu->isEmpty())
	{
		return;
	}

	auto *mainWindow = MainWindow::find(parentWidget());

	const QMap<QString, std::function<QDialog *()>> dialogs{
		{
			QStringLiteral("Device select"), [mainWindow]
			{
				return new Dialog::DeviceSelect({}, mainWindow);
			}
		},
		{
			QStringLiteral("Open link"), [mainWindow]
			{
				return new Dialog::OpenLink(QStringLiteral("/"), LinkType::Path, mainWindow);
			}
		},
		{
			QStringLiteral("Setup"), [this, mainWindow]
			{
				return new Dialog::Setup(settings, mainWindow);
			}
		},
		{
			QStringLiteral("Tracks cache"), [this, mainWindow]
			{
				auto *dialog = new Dialog::TracksCache(mainWindow);
				dialog->loadAllTracks(cache);
				return dialog;
			}
		},
		{
			QStringLiteral("What's new"), [this, mainWindow]
			{
				return new Dialog::WhatsNew(settings, httpClient, mainWindow);
			}
		},
		{
			QStringLiteral("Create playlist"), [this, mainWindow]
			{
				return new Dialog::CreatePlaylist({}, spotify, mainWindow);
			}
		},
		{
			QStringLiteral("Add to playlist"), [this, mainWindow]
			{
				return new Dialog::AddToPlaylist(spotify, lib::spt::playlist(), {}, {}, mainWindow);
			}
		},
		{
			QStringLiteral("Password entry"), [this, mainWindow]
			{
				auto *paths = new QtPaths(this);
				auto *runner = new SpotifyClient::Runner(settings, *paths, mainWindow);
				return new Dialog::PasswordEntry(runner, mainWindow);
			}
		},
		{
			QStringLiteral("Memory"), [mainWindow]
			{
				return new Dialog::Memory(mainWindow);
			}
		},
		{
			QStringLiteral("Lyrics search"), [this, mainWindow]
			{
				return new Dialog::LyricsSearch(httpClient, mainWindow);
			}
		},
		{
			QStringLiteral("Disallowed actions"), [mainWindow]
			{
				return new Dialog::Disallows(mainWindow);
			}
		},
		{
			QStringLiteral("Edit playlist"), [this, mainWindow]
			{
				return new Dialog::EditPlaylist(spotify, {}, -1, mainWindow);
			}
		},
	};

	QMapIterator<QString, std::function<QDialog *()>> iter(dialogs);
	while (iter.hasNext())
	{
		iter.next();
		addMenuItem(dialogMenu, iter.key(), [iter]()
		{
			iter.value()()->open();
		});
	}
}
