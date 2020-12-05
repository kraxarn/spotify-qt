#pragma once

#include "spotify/device.hpp"

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
	DeviceSelectDialog(const QVector<spt::Device> &devices, QWidget *parent = nullptr);
	spt::Device selectedDevice();

private:
	QVector<spt::Device> devices;
	QListWidget *list;
};