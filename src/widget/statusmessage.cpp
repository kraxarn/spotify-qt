#include "statusmessage.hpp"

StatusMessage *StatusMessage::instance = nullptr;

StatusMessage::StatusMessage(QWidget *parent)
	: QWidget(parent)
{
	setVisible(false);

	layout = new QHBoxLayout(this);
	layout->setContentsMargins(12, 0, 12, 0);

	icon = new QLabel(this);
	layout->addWidget(icon);

	message = new QLabel(this);
	layout->addWidget(message, 1, Qt::AlignHCenter);

	close = new QPushButton(this);
	close->setFlat(true);
	close->setIcon(Icon::get("window-close-symbolic"));
	layout->addWidget(close);

	QAbstractButton::connect(close, &QAbstractButton::clicked,
		this, &StatusMessage::onClose);

	timer = new QTimer(this);
	QTimer::connect(timer, &QTimer::timeout,
		this, &StatusMessage::onTimerTimeout);

	if (instance == nullptr)
	{
		instance = this;
	}
}

void StatusMessage::showStatus(MessageType messageType, const QString &text)
{
	if (timer->isActive())
	{
		timer->stop();
	}

	const auto iconSize = static_cast<int>(static_cast<float>(size().height()) * 0.75F);
	const auto pixmap = getIcon(messageType).pixmap(iconSize, iconSize);
	icon->setPixmap(pixmap);

	auto colors = palette();
	colors.setColor(QPalette::Window, getColor(messageType));
	setPalette(colors);

	message->setText(text);
	setVisible(true);

	const auto interval = getInterval(messageType);
	if (interval >= 0)
	{
		timer->start(interval);
	}
}

void StatusMessage::show(MessageType messageType, const QString &text)
{
	if (instance == nullptr)
	{
		lib::log::error("Failed to show status message, no instance found");
		return;
	}

	instance->showStatus(messageType, text);
}

auto StatusMessage::getIcon(MessageType messageType) -> QIcon
{
	switch (messageType)
	{
		case MessageType::Information:
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

		case MessageType::Error:
			return -1;

		default:
			return 0;
	}
}

void StatusMessage::onClose(bool /*checked*/)
{
	timer->stop();
	setVisible(false);
}

void StatusMessage::onTimerTimeout()
{
	setVisible(false);
}
