#pragma once

#include <QObject>
#include <QLabel>
#include <QTimer>

class Loader: public QObject
{
Q_OBJECT

public:
	explicit Loader(QLabel *parent);

private:
	int limit = 3;
	QString labelText;
	QLabel *label = nullptr;
	QTimer *timer = nullptr;

	void timeout();
};
