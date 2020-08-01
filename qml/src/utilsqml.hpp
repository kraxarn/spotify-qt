#pragma once

#include "common.hpp"

class UtilsQml : public QObject
{
	Q_OBJECT
	QML_ELEMENT

public:
	explicit UtilsQml(QObject *parent = nullptr);

	Q_INVOKABLE QString formatTime(int ms);
};


