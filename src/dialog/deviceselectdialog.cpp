#include "deviceselectdialog.hpp"

DeviceSelectDialog::DeviceSelectDialog(const std::vector<lib::spt::device> &devices,
	QWidget *parent)
	: devices(devices),
	QDialog(parent)
{
	auto layout = new QVBoxLayout();
	setLayout(layout);
	setWindowTitle("Select device");

	list = new QListWidget(this);
	for (auto &device : devices)
		list->addItem(QString::fromStdString(device.name));
	layout->addWidget(list);

	QListWidget::connect(list, &QListWidget::itemDoubleClicked, [this](QListWidgetItem *item)
	{
		accept();
	});

	auto buttons = new QDialogButtonBox();
	auto okButton = buttons->addButton(QDialogButtonBox::Ok);
	QPushButton::connect(okButton, &QPushButton::clicked, [this](bool checked)
	{
		accept();
	});
	auto cancelButton = buttons->addButton(QDialogButtonBox::Cancel);
	QPushButton::connect(cancelButton, &QPushButton::clicked, [this](bool checked)
	{
		reject();
	});
	layout->addWidget(buttons);
}

lib::spt::device DeviceSelectDialog::selectedDevice()
{
	if (list->selectedItems().isEmpty())
	{
		return {};
	}

	for (auto &device : devices)
	{
		if (device.name == list->selectedItems().first()->text().toStdString())
			return device;
	}

	return {};
}
