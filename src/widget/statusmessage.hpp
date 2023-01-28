#pragma once

#include "util/icon.hpp"
#include "enum/messagetype.hpp"

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QTimeLine>

class StatusMessage: public QWidget
{
Q_OBJECT

public:
	explicit StatusMessage(QWidget *parent);

	void showStatus(MessageType messageType, const QString &text);
	static void show(MessageType messageType, const QString &text);

	static void info(const QString &text);
	static void warn(const QString &text);
	static void error(const QString &text);

	static void info(const QString &text, const QString &buttonText,
		const std::function<void()> &buttonAction);

private:
	static constexpr int height = 36;
	static constexpr int iconSize = height * 0.75;
	static constexpr int animationDuration = 400;

	QHBoxLayout *layout = nullptr;

	QLabel *icon = nullptr;
	QLabel *message = nullptr;
	QPushButton *action = nullptr;
	QPushButton *close = nullptr;

	QTimer *timer = nullptr;
	QTimeLine *timeLine = nullptr;

	QString buttonText;
	std::function<void()> buttonAction;

	// This may or may not be a good idea
	static StatusMessage *instance;

	static auto getIcon(MessageType messageType) -> QIcon;
	static auto getColor(MessageType messageType) -> QColor;
	static auto getInterval(MessageType messageType) -> int;

	void showAnimated();
	void hideAnimated();
	void animate(int from, int to);

	void onClose(bool checked);
	void onTimerTimeout();
	void onTimeLineFrameChanged(int value);
};
