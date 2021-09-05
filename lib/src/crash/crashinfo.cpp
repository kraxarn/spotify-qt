#include "lib/crash/crashinfo.hpp"

lib::crash_info::crash_info()
{
	timestamp = lib::date_time::seconds_since_epoch();
}

void lib::to_json(nlohmann::json &j, const crash_info &i)
{
	j = nlohmann::json{
		{"timestamp", i.timestamp},
		{"signal", i.signal},
		{"info", i.info},
		{"stack_trace", i.stack_trace},
		{"handled", i.handled},
	};
}

void lib::from_json(const nlohmann::json &j, crash_info &i)
{
	if (!j.is_object())
	{
		return;
	}

	j.at("timestamp").get_to(i.timestamp);
	j.at("signal").get_to(i.signal);
	j.at("info").get_to(i.info);
	j.at("stack_trace").get_to(i.stack_trace);
	j.at("handled").get_to(i.handled);
}

auto lib::crash_info::to_string() const -> std::string
{
	std::stringstream stream;

	stream << "Signal: " << signal;
	if (!info.empty())
	{
		stream << ", " << info;
	}
	stream << std::endl;

	auto width = static_cast<int>(std::to_string(stack_trace.size()).size());
	for (size_t i = 0; i < stack_trace.size(); i++)
	{
		stream << "#" << std::setw(width) << std::left << i
			<< ' ' << stack_trace.at(i) << std::endl;
	}

	return stream.str();
}
