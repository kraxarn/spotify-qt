#pragma once

#include "util/urlutils.hpp"
#include "util/icon.hpp"

#include <QTreeWidget>
#include <QDir>
#include <QHeaderView>
#include <QMenu>

class CacheView: public QTreeWidget
{
public:
	CacheView(const lib::paths &paths, QWidget *parent);

private:
	const lib::paths &paths;

	static auto fullName(const QString &folderName) -> QString;
	static void folderSize(const QString &path, unsigned int *count, unsigned int *size);
	void menu(const QPoint &pos);
	void reload();
	void showEvent(QShowEvent *event) override;
};
