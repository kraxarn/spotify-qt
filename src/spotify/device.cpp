#include "device.hpp"

using namespace spt;

Device::Device(const QJsonObject &json)
{
	id = json["id"].toString();
	name = json["name"].toString();
	type = json["type"].toString();
	isActive = json["is_active"].toBool();
}