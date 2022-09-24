#pragma once

#include "settingspage/base.hpp"

#include "lib/settings.hpp"
#include "lib/cache.hpp"
#include "lib/httpclient.hpp"

#include <QDialog>
#include <QListWidget>
#include <QStackedWidget>

namespace Dialog
{
	class Settings: public QDialog
	{
	Q_OBJECT

	public:
		Settings(lib::settings &settings, lib::cache &cache,
			const lib::http_client &httpClient, QWidget *parent);

	private:
		void categoryChanged(int row);
		auto applySettings() -> bool;

		lib::settings &settings;

		QListWidget *categories = nullptr;
		QStackedWidget *stack = nullptr;

		QList<SettingsPage::Base *> pages;
	};
}
