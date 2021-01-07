#pragma once

#include "lib/settings.hpp"

#include <QVBoxLayout>
#include <QMessageBox>
#include <QTabWidget>

class SettingsPage: public QTabWidget
{
Q_OBJECT

public:
	virtual QIcon icon() = 0;
	virtual QString title() = 0;

	virtual bool save() = 0;

protected:
	explicit SettingsPage(lib::settings &settings, QWidget *parent);

	void warning(const QString &title, const QString &message);
	void applyFail(const QString &setting);
	QVBoxLayout *tabContent();

	lib::settings &settings;
};