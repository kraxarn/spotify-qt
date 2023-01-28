#include "spotify/deviceselect.hpp"
#include "dialog/deviceselect.hpp"

spt::DeviceSelect::DeviceSelect(QObject *parent)
	: QObject(parent)
{
}

void spt::DeviceSelect::select_device(const std::vector<lib::spt::device> &devices,
	lib::callback<lib::spt::device> &callback)
{
	auto *parentWidget = dynamic_cast<QWidget *>(parent());
	auto *dialog = new Dialog::DeviceSelect(devices, parentWidget);
	QDialog::connect(dialog, &Dialog::DeviceSelect::deviceSelected, callback);
	dialog->open();
}
