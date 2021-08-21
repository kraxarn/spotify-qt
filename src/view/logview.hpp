#pragma once

#include "lib/log.hpp"

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
	static QString collectLogs();
	void saveToFile();
	static void copyToClipboard();
};
