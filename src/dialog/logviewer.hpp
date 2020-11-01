#pragma once

#include "../log.hpp"

#include <QDialog>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QPushButton>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QStandardPaths>

class LogViewer: public QDialog
{
Q_OBJECT

public:
	explicit LogViewer(QWidget *parent);

private:
	void saveToFile();
};
