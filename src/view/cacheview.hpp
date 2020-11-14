#pragma once

#include "../util/utils.hpp"

#include <QTreeWidget>
#include <QDir>
#include <QHeaderView>
#include <QMenu>

class CacheView: public QTreeWidget
{
public:
	CacheView(const QString &cachePath, QWidget *parent);

private:
	static QString fullName(const QString &folderName);
	static void folderSize(const QString &path, unsigned int *count, unsigned int *size);
	void menu(const QPoint &pos);
};
