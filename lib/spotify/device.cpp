#include "device.hpp"

using namespace lib::spt;

Device::Device(const nlohmann::json &json)
{
	json.at("id").get_to(id);
	json.at("name").get_to(name);
	json.at("type").get_to(type);
	json.at("is_active").get_to(isActive);
	json.at("volume_percent").get_to(volumePercent);
}