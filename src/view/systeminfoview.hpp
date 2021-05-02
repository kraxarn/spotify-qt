#pragma once

#include "../mainwindow.hpp"
#include "../spotify/spotify.hpp"
#include "logview.hpp"
#include "lib/libversion.hpp"
#include "common/systeminfo.hpp"

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
	auto systemInfo() -> SystemInfo;
	void copyToClipboard(bool checked);
};
