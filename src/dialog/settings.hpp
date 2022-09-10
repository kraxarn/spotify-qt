#pragma once

#include "mainwindow.hpp"
#include "settingspage/base.hpp"
#include "spotifyclient/runner.hpp"
#include "view/systeminfoview.hpp"
#include "lib/settings.hpp"
#include "dialog/openlink.hpp"

#include <QAction>
#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QFileDialog>
#include <QFileInfo>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QProcess>
#include <QPushButton>
#include <QStackedWidget>
#include <QStyleFactory>
#include <QVBoxLayout>

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
