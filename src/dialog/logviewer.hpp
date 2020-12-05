#pragma once

#include "log.hpp"

#include <QApplication>
#include <QClipboard>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QPushButton>
#include <QStandardPaths>
#include <QTreeWidget>
#include <QVBoxLayout>

class LogViewer: public QDialog
{
Q_OBJECT

public:
	explicit LogViewer(QWidget *parent);

private:
	QString collectLogs();
	void saveToFile();
	void copyToClipboard();
};
