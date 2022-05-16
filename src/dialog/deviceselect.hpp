#pragma once

#include "lib/spotify/device.hpp"
#include "lib/spotify/callback.hpp"

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

	signals:
		void deviceSelected(const lib::spt::device &device);

	private:
		static constexpr int roleDevice = 0x100;

		QListWidget *list;

		void onItemDoubleClicked(QListWidgetItem *item);
		void onOk(bool checked);
		void onCancel(bool checked);
	};
}
