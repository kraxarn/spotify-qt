#pragma once

#include "../lib/qtpaths.hpp"
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
	explicit TracksCacheDialog(QWidget *parent);

private:
	QTreeWidget *tree = nullptr;

	void okClicked(bool checked);

	void open() override;
};
