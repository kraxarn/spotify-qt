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
	DeviceSelectDialog(const QVector<lib::spt::Device> &devices, QWidget *parent = nullptr);
	lib::spt::Device selectedDevice();

private:
	QVector<lib::spt::Device> devices;
	QListWidget *list;
};