#pragma once

#include "lib/spotify/api.hpp"
#include "lib/spotify/queue.hpp"
#include "lib/result.hpp"
#include "lib/settings.hpp"
#include "util/icon.hpp"

#include <QWidget>
#include <QVBoxLayout>
#include <QListWidget>
#include <QLabel>
#include <QTimer>
#include <QStackedWidget>
#include <QLineEdit>
#include <QDockWidget>

namespace Queue
{
	class View: public QDockWidget
	{
	Q_OBJECT

	public:
		View(lib::spt::api &spotify, lib::settings &settings, QWidget *parent);
		~View() override;

		void refreshQueue();
		void refreshFormat();

	private:
		lib::spt::api &spotify;
		lib::settings &settings;
		QStackedWidget *stackedWidget = nullptr;
		QListWidget *trackList = nullptr;
		QLabel *statusLabel = nullptr;
		QTimer *refreshTimer = nullptr;
		lib::spt::queue currentQueue; // Store current queue for skip operations
		bool isQueueEmpty = true; // Track if queue is empty to adjust refresh rate

		void setupUi();
		void skipTracks(int skips);
		void skipToTrackRecursive(int totalSkips, int currentSkip);

		void onItemClicked(QListWidgetItem *item);
		void onItemDoubleClicked(QListWidgetItem *item);
		void onContextMenu(const QPoint &position);
		void onRemoveFromQueue();
		void startAutoRefresh();
		void stopAutoRefresh();
		void showTrackList();
		void showStatusMessage(const QString &message);
		void updateItemText(QListWidgetItem *item);
	};
}
