#include "settingspage/application.hpp"
#include "mainwindow.hpp"
#include "util/font.hpp"
#include "util/widget.hpp"

#include <QGroupBox>

SettingsPage::Application::Application(lib::settings &settings, QWidget *parent)
	: SettingsPage::Base(settings, parent)
{
	addTab(app(), "General");
	addTab(windowTitle(), QStringLiteral("Title"));
}

void SettingsPage::Application::hideEvent(QHideEvent *event)
{
	Base::hideEvent(event);

	const auto *mainWindow = MainWindow::find(parent());
	if (mainWindow != nullptr)
	{
		disconnect(mainWindow, &MainWindow::playbackRefreshed,
			this, &Application::onPlaybackRefreshed);
	}
}

void SettingsPage::Application::showEvent(QShowEvent *event)
{
	Base::showEvent(event);

	const auto *mainWindow = MainWindow::find(parent());
	if (mainWindow != nullptr)
	{
		connect(mainWindow, &MainWindow::playbackRefreshed,
			this, &Application::onPlaybackRefreshed);
	}
}

auto SettingsPage::Application::app() -> QWidget *
{
	auto *layout = tabContent();
	auto *comboBoxLayout = new QGridLayout();

	auto *appRefreshGroup = new QGroupBox(QStringLiteral("Refresh interval"), this);
	appRefreshGroup->setToolTip(QStringLiteral("How often to refresh playback status from the Spotify servers"));
	layout->addWidget(appRefreshGroup);

	auto *appRefreshLayout = new QGridLayout();
	appRefreshGroup->setLayout(appRefreshLayout);

	// Refresh interval (active)
	auto *appRefreshLabel = new QLabel(QStringLiteral("While active"), this);
	appRefreshLabel->setToolTip(QStringLiteral("How often to refresh while the main window is active"));
	appRefreshLayout->addWidget(appRefreshLabel, 0, 0);

	appRefresh = new AppRefreshComboBox(this);
	appRefresh->addItems({
		QStringLiteral("1"),
		QStringLiteral("3"),
		QStringLiteral("5"),
		QStringLiteral("10"),
	});
	appRefresh->setCurrentIndex(-1);
	appRefresh->setEditText(QString::number(settings.general.refresh_interval));
	appRefreshLayout->addWidget(appRefresh, 0, 1);

	auto *appRefreshSuffix = new QLabel(QStringLiteral("seconds"), this);
	appRefresh->setSuffixLabel(appRefreshSuffix);
	appRefreshLayout->addWidget(appRefreshSuffix, 0, 2);

	// Refresh interval (inactive)
	auto *appRefreshInactiveLabel = new QLabel(QStringLiteral("While inactive"), this);
	appRefreshInactiveLabel->setToolTip(
		QStringLiteral("How often to refresh while the main window is inactive/minimized"));
	appRefreshLayout->addWidget(appRefreshInactiveLabel, 1, 0);

	appRefreshInactive = new AppRefreshComboBox(this);
	appRefreshInactive->addItems({
		QStringLiteral("Same as active"),
		QStringLiteral("3"),
		QStringLiteral("5"),
		QStringLiteral("10"),
	});
	appRefreshInactive->setCurrentIndex(0);
	appRefreshLayout->addWidget(appRefreshInactive, 1, 1);

	if (settings.general.refresh_interval_inactive > 0)
	{
		appRefreshInactive->setEditText(QString::number(settings.general.refresh_interval_inactive));
	}

	auto *appRefreshInactiveSuffix = new QLabel(QStringLiteral("seconds"), this);
	appRefreshInactive->setSuffixLabel(appRefreshInactiveSuffix);
	appRefreshLayout->addWidget(appRefreshInactiveSuffix, 1, 2);

	// Max queue
	auto *maxQueueLabel = new QLabel("Queue limit", this);
	maxQueueLabel->setToolTip("Maximum amount of items allowed to be queued at once");
	comboBoxLayout->addWidget(maxQueueLabel, 0, 0);

	appMaxQueue = new QComboBox(this);
	appMaxQueue->setEditable(true);
	appMaxQueue->setValidator(new QIntValidator(minMaxQueue,
		maxMaxQueue, this));
	appMaxQueue->addItems({
		"100", "250", "500"
	});
	appMaxQueue->setCurrentText(QString::number(settings.spotify.max_queue));
	comboBoxLayout->addWidget(appMaxQueue, 0, 1);
	comboBoxLayout->addWidget(new QLabel("tracks", this), 0, 2);

	layout->addLayout(comboBoxLayout);

	// MPRIS D-Bus
#ifdef USE_DBUS
	appMedia = new QCheckBox("Media controller", this);
	appMedia->setChecked(settings.general.media_controller);
	appMedia->setToolTip("Enable media controller through the MPRIS D-Bus interface");
	layout->addWidget(appMedia);
#endif

	// Register hotkeys (Windows)
#ifdef _WIN32
	appHotkeys = new QCheckBox(QStringLiteral("Register media keys"));
	appHotkeys->setToolTip(QStringLiteral("Register media keys as hotkeys"));
	appHotkeys->setChecked(settings.general.media_hotkeys);
	layout->addWidget(appHotkeys);
#endif

	// What's new dialog
	appWhatsNew = new QCheckBox("Show what's new on start", this);
	appWhatsNew->setToolTip("Show what's new in the latest version after the app has been updated");
	appWhatsNew->setChecked(settings.general.show_changelog);
	layout->addWidget(appWhatsNew);

	// Check for updates
	appUpdates = new QCheckBox(QStringLiteral("Check for updates on start"), this);
	appUpdates->setToolTip(QStringLiteral("Check for updates on start, and show notification if any"));
	appUpdates->setChecked(settings.general.check_for_updates);
	layout->addWidget(appUpdates);

	ignoreUnavailableIndex = new QCheckBox(QStringLiteral("Ignore index of unavailable songs"), this);
	ignoreUnavailableIndex->setToolTip(QStringLiteral("Ignore index of unavailable tracks, "
		"required by some clients to properly index playlists with unavailable songs"));
	ignoreUnavailableIndex->setChecked(settings.general.ignore_unavailable_index);
	layout->addWidget(ignoreUnavailableIndex);

	return Widget::layoutToWidget(layout, this);
}

auto SettingsPage::Application::windowTitle() -> QWidget *
{
	const auto &qtSettings = settings.qt();

	auto *layout = new QVBoxLayout();

	auto *formatLayout = new QGridLayout();
	formatLayout->setVerticalSpacing(0);

	auto *titleFormatTitle = new QLabel(QStringLiteral("Format"), this);
	formatLayout->addWidget(titleFormatTitle, 0, 0);

	titleFormat = new QLineEdit(this);
	titleFormat->setFont(Font::monospace());
	titleFormat->setText(QString::fromStdString(qtSettings.track_title));
	titleFormat->setPlaceholderText(QStringLiteral("{artist} - {track}"));
	formatLayout->addWidget(titleFormat, 0, 1);

	connect(titleFormat, &QLineEdit::textEdited,
		this, &Application::onTitleFormatEdited);

	titlePreview = new QLabel(this);
	titlePreview->setFont(Font::italic());
	titlePreview->setWordWrap(true);
	formatLayout->addWidget(titlePreview, 1, 1);

	layout->addLayout(formatLayout);

	const QMap<QString, QString> options{
		{QStringLiteral("{artist}"), QStringLiteral("Name of primary artist")},
		{QStringLiteral("{artists}"), QStringLiteral("All artists, seperated by commas")},
		{QStringLiteral("{track}"), QStringLiteral("Name of track")},
	};

	auto *infoLayout = new QGridLayout();
	infoLayout->setRowStretch(static_cast<int>(options.size()), 1);
	QMapIterator<QString, QString> iter(options);
	auto row = 0;

	while (iter.hasNext())
	{
		iter.next();

		auto *key = new QLabel(iter.key(), this);
		key->setFont(Font::monospace());
		infoLayout->addWidget(key, row, 0);

		auto *value = new QLabel(iter.value(), this);
		infoLayout->addWidget(value, row++, 1);
	}

	auto *infoBox = new QGroupBox(QStringLiteral("Possible values"), this);
	infoBox->setLayout(infoLayout);
	layout->addWidget(infoBox, 1);

	return Widget::layoutToWidget(layout, this);
}

auto SettingsPage::Application::icon() -> QIcon
{
	return Icon::get(QStringLiteral("document-properties"));
}

auto SettingsPage::Application::title() -> QString
{
	return "Application";
}

static auto parseRefreshInterval(const QComboBox *comboBox, int &value) -> bool
{
	auto is_int = false;
	const int interval = comboBox->currentText().toInt(&is_int);

	if (!is_int
		|| interval < AppRefreshComboBox::minRefreshInterval
		|| interval > AppRefreshComboBox::maxRefreshInterval)
	{
		return false;
	}

	value = interval;
	return true;
}

auto SettingsPage::Application::saveGeneral() -> bool
{
	auto success = true;

	// Media controller
	if (appMedia != nullptr)
	{
		if (appMedia->isChecked() != settings.general.media_controller)
		{
			QMessageBox::information(this, "Media Controller",
				"Please restart the application to apply changes");
		}
		settings.general.media_controller = appMedia->isChecked();
	}

#ifdef _WIN32
	if (appHotkeys != nullptr)
	{
		if (settings.general.media_hotkeys != appHotkeys->isChecked())
		{
			auto *mainWindow = MainWindow::find(parentWidget());
			if (mainWindow != nullptr)
			{
				mainWindow->registerMediaHotkeys(appHotkeys->isChecked());
			}
		}
		settings.general.media_hotkeys = appHotkeys->isChecked();
	}
#endif

	// Refresh interval
	if (appRefresh != nullptr)
	{
		if (!parseRefreshInterval(appRefresh,
			settings.general.refresh_interval))
		{
			applyFail(QStringLiteral("refresh interval"));
			success = false;
		}
	}
	if (appRefreshInactive != nullptr)
	{
		// Same as active
		if (appRefreshInactive->currentIndex() == 0)
		{
			settings.general.refresh_interval_inactive = 0;
		}
		else if (!parseRefreshInterval(appRefreshInactive,
			settings.general.refresh_interval_inactive))
		{
			applyFail(QStringLiteral("inactive refresh interval"));
			success = false;
		}
	}

	// Max queue
	if (appMaxQueue != nullptr)
	{
		auto ok = false;
		auto maxQueue = appMaxQueue->currentText().toInt(&ok);
		if (!ok || maxQueue < minMaxQueue || maxQueue > maxMaxQueue)
		{
			applyFail("queue limit");
			success = false;
		}
		settings.spotify.max_queue = maxQueue;
	}

	// Other application stuff

	if (appWhatsNew != nullptr)
	{
		settings.general.show_changelog = appWhatsNew->isChecked();
	}

	if (appUpdates != nullptr)
	{
		settings.general.check_for_updates = appUpdates->isChecked();
	}

	if (ignoreUnavailableIndex != nullptr)
	{
		settings.general.ignore_unavailable_index = ignoreUnavailableIndex->isChecked();
	}

	return success;
}

auto SettingsPage::Application::saveTitle() const -> bool
{
	auto &qtSettings = settings.qt();

	if (titleFormat != nullptr)
	{
		qtSettings.track_title = titleFormat->text().toStdString();
	}

	return true;
}

auto SettingsPage::Application::save() -> bool
{
	return saveGeneral() && saveTitle();
}

void SettingsPage::Application::updatePreview()
{
	if (titleFormat == nullptr
		|| titlePreview == nullptr
		|| !currentTrack.is_valid())
	{
		return;
	}

	const auto format = titleFormat->text().isEmpty()
		? titleFormat->placeholderText()
		: titleFormat->text();

	const QString title = Format::title(currentTrack, format);
	titlePreview->setText(title);
}

void SettingsPage::Application::onPlaybackRefreshed(const lib::spt::playback &current,
	const lib::spt::playback &/*previous*/)
{
	if (currentTrack.id == current.item.id)
	{
		return;
	}

	currentTrack = current.item;
	updatePreview();
}

void SettingsPage::Application::onTitleFormatEdited(const QString &/*text*/)
{
	updatePreview();
}
