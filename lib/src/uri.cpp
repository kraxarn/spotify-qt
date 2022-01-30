#include "lib/uri.hpp"

#include "lib/fmt.hpp"

#include <regex>

lib::uri::uri(const std::string &url)
{
	set_url(url);
}

auto lib::uri::get_url() const -> const std::string &
{
	return url;
}

void lib::uri::set_url(const std::string &value)
{
	// Try to find a protocol
	if (value.find("://") == std::string::npos)
	{
		throw std::runtime_error("parse failed: no protocol specified");
	}

	// Should contain at least one .
	if (value.find('.') == std::string::npos)
	{
		throw std::runtime_error("parse failed: no top-level domain specified");
	}

	url = value;
}

auto lib::uri::hostname() -> std::string
{
	const auto start = url.find("://") + 3;
	return url.substr(start, url.find('/', start) - start);
}

auto lib::uri::origin() -> std::string
{
	return lib::fmt::format("{}//{}",
		protocol(), hostname());
}

auto lib::uri::pathname() -> std::string
{
	const auto url_start = url.find("://") + 3;
	const auto path_start = url.find('/', url_start);

	if (path_start == std::string::npos)
	{
		return {};
	}
	return url.substr(path_start);
}

auto lib::uri::protocol() -> std::string
{
	return url.substr(0, url.find(':') + 1);
}

auto lib::uri::get_search_params() -> std::map<std::string, std::string>
{
	std::map<std::string, std::string> params;

	const auto param_start = url.find('?');
	if (param_start == std::string::npos)
	{
		return params;
	}

	const auto values = lib::strings::split(url.substr(param_start), '&');
	for (size_t i = 0; i < values.size(); i++)
	{
		const auto value = values.at(i);
		const auto splitter = value.find('=');
		if (splitter == std::string::npos)
		{
			continue;
		}

		const auto key_offset = i == 0 ? 1 : 0;
		params.insert({
			value.substr(key_offset, splitter - key_offset),
			value.substr(splitter + 1),
		});
	}

	return params;
}

void lib::uri::set_search_params(const std::map<std::string, std::string> &params)
{
	std::string new_url = get_url().substr(0, url.find('?'));

	std::stringstream stream;
	for (auto iter = params.cbegin(); iter != params.cend(); iter++)
	{
		stream
			<< (iter == params.cbegin() ? "?" : "&")
			<< iter->first << "=" << iter->second;
	}

	new_url.append(stream.str());
	set_url(new_url);
}

auto lib::uri::encode(const std::string &uri) -> std::string
{
	std::ostringstream stream;
	std::regex regex("[!'\\(\\)*-.0-9A-Za-z_~]");

	for (size_t i = 0; i < uri.size(); i++)
	{
		const auto &str = uri.substr(i, 1);

		if (std::regex_match(str, regex))
		{
			stream << str;
		}
		else
		{
			stream << "%" << std::uppercase << std::hex << (0xff & str.at(0));
		}
	}

	return stream.str();
}
