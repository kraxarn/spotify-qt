#pragma once

#include "lib/qtpaths.hpp"
#include "lib/cache.hpp"

#include <QDialog>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>

class TracksCacheDialog: public QDialog
{
Q_OBJECT

public:
	explicit TracksCacheDialog(lib::cache &cache, QWidget *parent);

private:
	QTreeWidget *tree = nullptr;
	lib::cache &cache;

	void okClicked(bool checked);
	void open() override;
};
