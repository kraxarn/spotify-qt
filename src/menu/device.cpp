#include "menu/device.hpp"

#include "util/icon.hpp"
#include "widget/statusmessage.hpp"

Menu::Device::Device(lib::spt::api &spotify, QWidget *parent)
	: QMenu(parent),
	spotify(spotify)
{
	setTitle(QStringLiteral("Device"));
	setIcon(Icon::get(QStringLiteral("speaker")));

	QMenu::connect(this, &QMenu::aboutToShow, this, &Menu::Device::onAboutToShow);
	QMenu::connect(this, &QMenu::triggered, this, &Menu::Device::onTriggered);

	// Some systems don't show menus with no items
	auto *action = addAction(QStringLiteral("Refreshing devices..."));
	action->setDisabled(true);
}

void Menu::Device::refreshDevices()
{
	spotify.devices([this](const std::vector<lib::spt::device> &devices)
	{
		for (auto &action: actions())
		{
			removeAction(action);
		}

		if (devices.empty())
		{
			auto *action = addAction(QStringLiteral("No devices found"));
			action->setDisabled(true);
			return;
		}

		for (const auto &device: devices)
		{
			auto *action = addAction(QString::fromStdString(device.name));
			action->setCheckable(true);
			action->setChecked(device.is_active);
			action->setDisabled(device.is_active);
			action->setData(QString::fromStdString(device.id));
		}
	});
}

void Menu::Device::onAboutToShow()
{
	refreshDevices();
}

void Menu::Device::onTriggered(QAction *action)
{
	const auto deviceId = action->data().toString().toStdString();
	spotify.set_device(deviceId, [action](const std::string &status)
	{
		if (!status.empty())
		{
			action->setChecked(false);
			StatusMessage::error(QString("Failed to set device: %1")
				.arg(QString::fromStdString(status)));
			return;
		}

		action->setDisabled(true);
	});
}
