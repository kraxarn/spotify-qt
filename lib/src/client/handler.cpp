#include "lib/client/handler.hpp"

lib::cl::handler::handler(const lib::settings &settings, const lib::paths &paths)
	: settings(settings),
	paths(paths)
{
	path = settings.spotify.path;
	client_type = get_client_type(path);
}

auto lib::cl::handler::start() -> std::string
{
	// Don't start if already running
	if (!settings.spotify.always_start && is_running())
	{
		return std::string();
	}

	// Check if empty
	if (client_type == lib::client_type::none)
	{
		return "path is empty or invalid";
	}

	// Check if path exists
	if (!ghc::filesystem::exists(path))
	{
		return "file in path does not exist";
	}

	// If using global config, just start
	if (settings.spotify.global_config
		&& client_type == lib::client_type::spotifyd)
	{
		start_process(path, {});
		return std::string();
	}

	// Check if username exists
	auto username = settings.spotify.username;
	if (username.empty())
	{
		return "no username provided";
	}

	// Get password from keyring if set
	std::string password;
	if (settings.spotify.keyring_password)
	{
		password = get_keyring_password(username);
	}

	// Ask for password
	if (password.empty())
	{
		password = get_password(username);

		if (password.empty())
		{
			return "no password provided";
		}

		if (settings.spotify.keyring_password)
		{
			set_keyring_password(username, password);
		}
	}

	// Common arguments
	std::vector<std::string> arguments({
		"--bitrate", std::to_string(settings.spotify.bitrate),
		"--username", username,
		"--password", password
	});

	// librespot specific
	if (client_type == lib::client_type::librespot)
	{
		lib::vector::append(arguments, {
			"--name", "spotify-qt (librespot)",
			"--initial-volume", "100",
			"--autoplay",
			"--cache", paths.cache() / "librespot",
		});
	}
	else if (client_type == lib::client_type::spotifyd)
	{
		lib::vector::append(arguments, {
			"--no-daemon",
			"--device-name", "spotify-qt (spotifyd)",
		});
	}

	auto backend = settings.spotify.backend;
	if (backend.empty() && supports_pulse())
	{
		backend = "pulseaudio";
	}
	if (!backend.empty())
	{
		lib::vector::append(arguments, {
			"--backend", backend
		});
	}

	if (client_type == lib::client_type::librespot && settings.spotify.disable_discovery)
	{
		arguments.emplace_back("--disable-discovery");
	}

	lib::log::dev("starting: {} {}", path, lib::strings::join(arguments, " "));

	start_process(path, arguments);
	return std::string();
}

auto lib::cl::handler::client_exec(const ghc::filesystem::path &client_path,
	std::vector<std::string> &arguments) -> std::string
{
	// Check if it exists
	auto absolute_path = ghc::filesystem::absolute(client_path);
	if (!ghc::filesystem::exists(absolute_path))
	{
		return std::string();
	}

	// Check if either client
	if (get_client_type(client_path) == lib::client_type::none)
	{
		return std::string();
	}

	// Get output
	return start_process_and_wait(absolute_path, arguments);
}

auto lib::cl::handler::available_backends() -> std::vector<std::string>
{
	std::vector<std::string> items;

	if (client_type == lib::client_type::librespot)
	{
		std::vector<std::string> arguments = {
			"--name", "",
			"--backend", "?"
		};
		auto result = client_exec(path, arguments);

		for (auto &line : lib::strings::split(result, "\n"))
		{
			if (!lib::strings::starts_with(line, "-"))
			{
				continue;
			}

			auto item = lib::strings::right(line, line.size() - 2);
			item = lib::strings::remove(item, "(default)");
			item = lib::strings::trim(item);
			items.push_back(item);
		}
	}
	else if (client_type == lib::client_type::spotifyd)
	{
		std::vector<std::string> arguments = {
			"--help",
		};
		auto result = client_exec(path, arguments);

		for (auto &line : lib::strings::split(result, "\n"))
		{
			if (!lib::strings::contains(line, "audio backend"))
			{
				continue;
			}

			auto item = lib::strings::right(line, line.size()
				- lib::strings::index_of(line, "[") - 1);
			item = lib::strings::remove(item, "possible values: ");
			item = lib::strings::remove(item, "]");
			item = lib::strings::trim(item);
			lib::vector::append(items, lib::strings::split(item, ", "));
			break;
		}
	}

	return items;
}

auto lib::cl::handler::supports_pulse() -> bool
{
	std::vector<std::string> arguments;

	if (client_type == lib::client_type::librespot)
	{
		arguments = {
			"--name", "",
			"--backend", "?",
		};
	}
	else if (client_type == lib::client_type::spotifyd)
	{
		arguments = {
			"--help",
		};
	}

	return lib::strings::contains(start_process_and_wait(path, arguments),
		"pulseaudio");
}

auto lib::cl::handler::version(const ghc::filesystem::path &client_path) -> std::string
{
	auto temp_type = get_client_type(client_path);

	if (temp_type == lib::client_type::spotifyd)
	{
		return start_process_and_wait(client_path, {"--version"});
	}

	if (temp_type == lib::client_type::librespot)
	{
		return "librespot";
	}

	return std::string();
}

auto lib::cl::handler::is_running() -> bool
{
	if (path.empty() || !ghc::filesystem::exists("/usr/bin/ps"))
	{
		return false;
	}

	return lib::strings::contains(start_process_and_wait("/usr/bin/ps",
		{"aux"}), path);
}

auto lib::cl::handler::get_sink_info() -> std::string
{
	if (!ghc::filesystem::exists("/usr/bin/pactl"))
	{
		return std::string();
	}

	auto result = start_process_and_wait("/usr/bin/pactl", {
		"list", "sink-inputs",
	});
	auto sinks = lib::strings::split(result, "Sink Input #");

	for (auto &sink : sinks)
	{
		if (lib::strings::contains(sink, "Spotify"))
		{
			return sink;
		}
	}

	return std::string();
}

auto lib::cl::handler::get_volume() -> float
{
	auto sink = get_sink_info();
	if (sink.empty())
	{
		return 1.F;
	}

	auto i = lib::strings::index_of(sink, "Volume:");
	if (i < 0)
	{
		return 1.F;
	}

	int v = 0;
	for (auto &p : lib::strings::split(lib::strings::right(sink,
		sink.length() - i), " "))
	{
		if (!lib::strings::ends_with(p, "%"))
		{
			continue;
		}

		if (!lib::strings::try_to_int(lib::strings::left(p, p.length() - 1), v))
		{
			continue;
		}

		return (float) v / 100.F;
	}

	return 1.F;
}

void lib::cl::handler::set_volume(float value)
{
	auto sink = get_sink_info();
	if (sink.empty())
	{
		return;
	}

	// Sink was found, get id
	auto left = lib::strings::left(sink, lib::strings::index_of(sink, "\n"));
	auto sinkId = lib::strings::right(left, left.length() -
		lib::strings::last_index_of(left, "#") - 1);

	start_process_and_wait("/usr/bin/pactl", {
		"set-sink-input-volume", sinkId, lib::strings::to_string(value, 2),
	});
}

void lib::cl::handler::log_output(const std::string &message)
{
	for (const auto &line : lib::strings::split(message, "\n"))
	{
		if (line.empty())
		{
			return;
		}
		log.emplace_back(lib::log_type::information, message);
	}
}

auto lib::cl::handler::get_log() const -> const std::vector<lib::log_message> &
{
	return log;
}

auto lib::cl::handler::get_client_type(const ghc::filesystem::path &path) -> lib::client_type
{
	const auto base_name = lib::strings::to_lower(path.stem());

	return base_name == "spotifyd"
		? lib::client_type::spotifyd
		: base_name == "librespot"
			? lib::client_type::librespot
			: lib::client_type::none;
}
