#pragma once

#include <QIcon>

class Icon
{
public:
	static QIcon get(const QString &name);

private:
	Icon();
};