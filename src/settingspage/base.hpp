#pragma once

#include "lib/settings.hpp"

#include <QVBoxLayout>
#include <QMessageBox>
#include <QTabWidget>
#include <QComboBox>
#include <QLabel>
#include <QCheckBox>

namespace SettingsPage
{
	class Base: public QTabWidget
	{
	Q_OBJECT

	public:
		virtual auto icon() -> QIcon = 0;
		virtual auto title() -> QString = 0;

		virtual auto save() -> bool = 0;

	protected:
		Base(lib::settings &settings, QWidget *parent);

		void warning(const QString &title, const QString &message);
		void info(const QString &title, const QString &message);

		void applyFail(const QString &setting);
		auto tabContent() -> QVBoxLayout *;

		lib::settings &settings;
	};
}
