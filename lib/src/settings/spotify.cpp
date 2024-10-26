#include "lib/settings/spotify.hpp"

void lib::setting::to_json(nlohmann::json &j, const spotify &s)
{
	j = nlohmann::json{
		{"always_start", s.always_start},
		{"backend", s.backend},
		{"bitrate", s.bitrate},
		{"device_type", s.device_type},
		{"disable_discovery", s.disable_discovery},
		{"global_config", s.global_config},
		{"max_queue", s.max_queue},
		{"path", s.path},
		{"start_client", s.start_client},
		{"volume", s.volume},
		{"additional_arguments", s.additional_arguments},
	};
}

void lib::setting::from_json(const nlohmann::json &j, spotify &s)
{
	if (!j.is_object())
	{
		return;
	}

	lib::json::get(j, "always_start", s.always_start);
	lib::json::get(j, "backend", s.backend);
	lib::json::get(j, "bitrate", s.bitrate);
	lib::json::get(j, "device_type", s.device_type);
	lib::json::get(j, "disable_discovery", s.disable_discovery);
	lib::json::get(j, "global_config", s.global_config);
	lib::json::get(j, "max_queue", s.max_queue);
	lib::json::get(j, "path", s.path);
	lib::json::get(j, "start_client", s.start_client);
	lib::json::get(j, "volume", s.volume);
	lib::json::get(j, "additional_arguments", s.additional_arguments);
}
