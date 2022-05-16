#include "dialog/deviceselect.hpp"

Q_DECLARE_METATYPE(lib::spt::device)

Dialog::DeviceSelect::DeviceSelect(const std::vector<lib::spt::device> &devices, QWidget *parent)
	: QDialog(parent)
{
	auto *layout = new QVBoxLayout();
	setLayout(layout);
	setWindowTitle(QStringLiteral("Select device"));

	list = new QListWidget(this);
	for (const auto &device: devices)
	{
		auto *item = new QListWidgetItem(QString::fromStdString(device.name), list);
		item->setData(roleDevice, QVariant::fromValue(device));
	}
	layout->addWidget(list);

	QListWidget::connect(list, &QListWidget::itemDoubleClicked,
		this, &Dialog::DeviceSelect::onItemDoubleClicked);

	auto *buttons = new QDialogButtonBox();
	auto *okButton = buttons->addButton(QDialogButtonBox::Ok);

	QPushButton::connect(okButton, &QPushButton::clicked,
		this, &Dialog::DeviceSelect::onOk);

	auto *cancelButton = buttons->addButton(QDialogButtonBox::Cancel);
	QPushButton::connect(cancelButton, &QPushButton::clicked,
		this, &Dialog::DeviceSelect::onCancel);

	layout->addWidget(buttons);
}

void Dialog::DeviceSelect::onItemDoubleClicked(QListWidgetItem *item)
{
	const auto &data = item->data(roleDevice);
	const auto &device = data.value<lib::spt::device>();
	emit deviceSelected(device);

	accept();
}

void Dialog::DeviceSelect::onOk(bool /*checked*/)
{
	const auto &items = list->selectedItems();
	if (items.empty())
	{
		return;
	}

	const auto &data = items.first()->data(roleDevice);
	const auto &device = data.value<lib::spt::device>();
	emit deviceSelected(device);

	accept();
}

void Dialog::DeviceSelect::onCancel(bool /*checked*/)
{
	reject();
}
