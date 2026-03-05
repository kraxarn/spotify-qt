#pragma once

#include <QJsonDocument>

class Void final
{
public:
	static auto fromJson(const QJsonDocument &json) -> Void;

private:
	Void() = default;
};
