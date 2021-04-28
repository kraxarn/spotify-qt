#pragma once

#include "lib/settings.hpp"

#include <QVBoxLayout>
#include <QMessageBox>
#include <QTabWidget>
#include <QComboBox>
#include <QLabel>
#include <QCheckBox>

class SettingsPage: public QTabWidget
{
Q_OBJECT

public:
	virtual auto icon() -> QIcon = 0;
	virtual auto title() -> QString = 0;

	virtual auto save() -> bool = 0;

protected:
	explicit SettingsPage(lib::settings &settings, QWidget *parent);

	void warning(const QString &title, const QString &message);
	void applyFail(const QString &setting);
	auto tabContent() -> QVBoxLayout *;

	lib::settings &settings;
};
