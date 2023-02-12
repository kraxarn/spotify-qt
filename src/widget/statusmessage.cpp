#include "statusmessage.hpp"

StatusMessage *StatusMessage::instance = nullptr;

StatusMessage::StatusMessage(QWidget *parent)
	: QWidget(parent)
{
	setFixedHeight(0);

	layout = new QHBoxLayout(this);
	layout->setContentsMargins(12, 0, 12, 0);

	icon = new QLabel(this);
	layout->addWidget(icon);

	message = new QLabel(this);
	layout->addWidget(message, 1, Qt::AlignVCenter);

	action = new QPushButton(this);
	action->setFlat(true);
	layout->addWidget(action);

	QAbstractButton::connect(action, &QAbstractButton::clicked,
		this, &StatusMessage::onAction);

	close = new QPushButton(this);
	close->setFlat(true);
	close->setIcon(Icon::get("window-close"));
	layout->addWidget(close);

	QAbstractButton::connect(close, &QAbstractButton::clicked,
		this, &StatusMessage::onClose);

	timer = new QTimer(this);
	timer->setSingleShot(true);
	QTimer::connect(timer, &QTimer::timeout,
		this, &StatusMessage::onTimerTimeout);

	timeLine = new QTimeLine(animationDuration, this);
	QTimeLine::connect(timeLine, &QTimeLine::frameChanged,
		this, &StatusMessage::onTimeLineFrameChanged);

	if (instance == nullptr)
	{
		instance = this;
	}
}

void StatusMessage::showStatus(MessageType messageType, const QString &text)
{
	if (text.isNull() || text.isEmpty())
	{
		return;
	}

	action->setText(buttonText);
	action->setVisible(!buttonText.isEmpty());

	timer->stop();

	const auto pixmap = getIcon(messageType).pixmap(iconSize, iconSize);
	icon->setPixmap(pixmap);

	auto colors = palette();
	colors.setColor(QPalette::Window, getColor(messageType));
	setPalette(colors);

	message->setText(text);
	showAnimated();

	const auto interval = getInterval(messageType);
	if (interval >= 0)
	{
		timer->start(interval);
	}
}

void StatusMessage::showAnimated()
{
	animate(0, height);
}

void StatusMessage::hideAnimated()
{
	animate(height, 0);
}

void StatusMessage::animate(int from, int to)
{
	timeLine->stop();

	timeLine->setFrameRange(from, to);
	timeLine->start();
}

void StatusMessage::show(MessageType messageType, const QString &text)
{
	if (instance == nullptr)
	{
		lib::log::error("Failed to show status message, no instance found");
		return;
	}

	if (messageType != MessageType::InformationAction)
	{
		instance->buttonText = QString();
		instance->buttonAction = {};
	}

	instance->showStatus(messageType, text);
}

void StatusMessage::info(const QString &text)
{
	StatusMessage::show(MessageType::Information, text);
}

void StatusMessage::warn(const QString &text)
{
	StatusMessage::show(MessageType::Warning, text);
}

void StatusMessage::error(const QString &text)
{
	StatusMessage::show(MessageType::Error, text);
}

void StatusMessage::info(const QString &text, const QString &buttonText,
	const std::function<void()> &buttonAction)
{
	if (instance != nullptr)
	{
		instance->buttonText = buttonText;
		instance->buttonAction = buttonAction;
	}

	StatusMessage::show(MessageType::InformationAction, text);
}

auto StatusMessage::getIcon(MessageType messageType) -> QIcon
{
	switch (messageType)
	{
		case MessageType::Information:
		case MessageType::InformationAction:
			return Icon::get(QStringLiteral("data-information"));

		case MessageType::Warning:
			return Icon::get(QStringLiteral("data-warning"));

		case MessageType::Error:
			return Icon::get(QStringLiteral("data-error"));

		default:
			return {};
	}
}

auto StatusMessage::getColor(MessageType messageType) -> QColor
{
	// Colors borrowed from KColorScheme
	switch (messageType)
	{
		case MessageType::Information:
		case MessageType::InformationAction:
			return {61, 174, 233};

		case MessageType::Warning:
			return {246, 116, 0};

		case MessageType::Error:
			return {218, 68, 83};

		default:
			return {39, 174, 96};
	}
}

auto StatusMessage::getInterval(MessageType messageType) -> int
{
	switch (messageType)
	{
		case MessageType::Information:
			return 5000;

		case MessageType::Warning:
			return 10000;

		case MessageType::InformationAction:
		case MessageType::Error:
			return -1;

		default:
			return 0;
	}
}

void StatusMessage::onAction(bool checked)
{
	if (buttonAction)
	{
		buttonAction();
	}
	onClose(checked);
}

void StatusMessage::onClose(bool /*checked*/)
{
	timer->stop();
	hideAnimated();
}

void StatusMessage::onTimerTimeout()
{
	hideAnimated();
}

void StatusMessage::onTimeLineFrameChanged(int value)
{
	setFixedHeight(value);
}
