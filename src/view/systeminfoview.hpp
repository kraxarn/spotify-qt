#pragma once

#include "util/systeminfo.hpp"

#include <QFile>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

class SystemInfoView: public QWidget
{
Q_OBJECT

public:
	explicit SystemInfoView(QWidget *parent = nullptr);

private:
	auto systemInfo() -> lib::qt::system_info;
	void copyToClipboard(bool checked);
};
