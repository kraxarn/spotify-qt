#pragma once

#include "util/icon.hpp"
#include "enum/messagetype.hpp"

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTimer>

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

private:
	QHBoxLayout *layout = nullptr;

	QLabel *icon = nullptr;
	QLabel *message = nullptr;
	QPushButton *close = nullptr;

	QTimer *timer = nullptr;

	// This may or may not be a good idea
	static StatusMessage *instance;

	static auto getIcon(MessageType messageType) -> QIcon;
	static auto getColor(MessageType messageType) -> QColor;
	static auto getInterval(MessageType messageType) -> int;

	void onClose(bool checked);
	void onTimerTimeout();
};
