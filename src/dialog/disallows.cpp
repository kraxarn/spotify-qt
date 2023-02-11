#include "dialog/disallows.hpp"
#include "mainwindow.hpp"

Dialog::Disallows::Disallows(QWidget *parent)
	: Base(parent)
{
	auto *layout = Base::layout<QVBoxLayout>();

	list = new QListWidget(this);
	list->setSelectionMode(QAbstractItemView::NoSelection);
	layout->addWidget(list);

	Base::addAction(DialogAction::Ok);
}

void Dialog::Disallows::showEvent(QShowEvent *event)
{
	constexpr int min = static_cast<int>(lib::player_action::interrupting_playback);
	constexpr int max = static_cast<int>(lib::player_action::transferring_playback);

	list->clear();

	auto *mainWindow = MainWindow::find(parentWidget());
	auto actions = mainWindow != nullptr
		? mainWindow->currentPlayback().disallowed_actions
		: std::unordered_set<lib::player_action>();

	for (auto i = min; i <= max; i++)
	{
		const auto value = static_cast<lib::player_action>(i);
		const auto str = lib::enums<lib::player_action>::to_string(value);

		auto *item = new QListWidgetItem(list);
		item->setText(QString::fromStdString(str));
		item->setData(actionRole, static_cast<int>(value));
		item->setCheckState(actions.find(value) != actions.end()
			? Qt::Checked
			: Qt::Unchecked);
	}

	QDialog::showEvent(event);
}

void Dialog::Disallows::onOk(bool checked)
{
	auto *mainWindow = MainWindow::find(parentWidget());
	if (mainWindow != nullptr)
	{
		auto playback = mainWindow->currentPlayback();
		std::unordered_set<lib::player_action> actions;

		for (int i = 0; i < list->count(); i++)
		{
			auto *item = list->item(i);
			if (item->checkState() == Qt::Checked)
			{
				const auto data = item->data(actionRole);
				const auto action = static_cast<lib::player_action>(data.value<int>());
				actions.insert(action);
			}
		}

		playback.disallowed_actions = actions;
		mainWindow->refreshed(playback);
	}

	Base::onOk(checked);
}
