#pragma once

#include "lib/log/log.hpp"

#include <QApplication>
#include <QClipboard>
#include <QDateTime>
#include <QFileDialog>
#include <QPushButton>
#include <QStandardPaths>
#include <QTreeWidget>
#include <QVBoxLayout>

class LogView: public QWidget
{
Q_OBJECT

public:
	explicit LogView(QWidget *parent);

private:
	QString collectLogs();
	void saveToFile();
	void copyToClipboard();
};
