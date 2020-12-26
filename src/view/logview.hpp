#pragma once

#include "../log.hpp"

#include <QFileDialog>
#include <QPushButton>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QStandardPaths>
#include <QClipboard>

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
