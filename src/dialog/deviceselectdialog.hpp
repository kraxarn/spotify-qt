#pragma once

#include "lib/spotify/device.hpp"

#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>

class DeviceSelectDialog : public QDialog
{
	Q_OBJECT

public:
	DeviceSelectDialog(const std::vector<lib::spt::device> &devices, QWidget *parent = nullptr);
	lib::spt::device selectedDevice();

private:
	std::vector<lib::spt::device> devices;
	QListWidget *list;
};