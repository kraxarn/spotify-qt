#pragma once

#include "lib/spotify/device.hpp"

#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>

namespace Dialog
{
	class DeviceSelect: public QDialog
	{
	Q_OBJECT

	public:
		DeviceSelect(const std::vector<lib::spt::device> &devices, QWidget *parent);

		auto selectedDevice() -> lib::spt::device;

	private:
		std::vector<lib::spt::device> devices;
		QListWidget *list;
	};
}
