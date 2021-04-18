#pragma once

#include "lib/settings.hpp"
#include "lib/enum/clienttype.hpp"
#include "lib/logmessage.hpp"
#include "lib/vector.hpp"
#include "lib/paths/paths.hpp"
#include "lib/strings.hpp"
#include "lib/log.hpp"

#include "thirdparty/filesystem.hpp"

#include <string>
#include <vector>

namespace lib
{
	namespace cl
	{
		/**
		 * Manages Spotify client
		 */
		class handler
		{
		public:
			/**
			 * Create new instance
			 * @param settings Settings to read client path from
			 * @param paths Paths to get cache path from
			 * @note Does not start the client
			 */
			handler(const lib::settings &settings, const lib::paths &paths);

			/**
			 * Start client
			 * @return Any possible error message
			 */
			auto start() -> std::string;

			/**
			 * Available backends for audio playback
			 */
			auto available_backends() -> std::vector<std::string>;

			/**
			 * Client is running
			 * @note False doesn't guarantee that the client isn't running
			 */
			auto is_running() -> bool;

			/**
			 * Spotifyd version
			 * @param client_path Path to client
			 * @note librespot only returns "librespot" without version
			 */
			auto version(const ghc::filesystem::path &client_path) -> std::string;

			/**
			 * Logged messages from Spotify client
			 */
			auto get_log() const -> const std::vector<lib::log_message> &;

			/**
			 * Local volume (0-1)
			 * @note Always returns 1 if PulseAudio isn't supported
			 */
			auto get_volume() -> float;

			/**
			 * Set local volume
			 * @param value 0-1
			 * @note Does nothing if PulseAudio isn't supported
			 */
			void set_volume(float value);

		protected:
			/**
			 * Get a saved password from system keyring
			 */
			virtual auto get_keyring_password(const std::string &username) -> std::string = 0;

			/**
			 * Save a password to system keyring
			 */
			virtual void set_keyring_password(const std::string &username,
				const std::string &password) = 0;

			/**
			 * Get password from user
			 * @param username Username to get password for
			 * @return Password
			 */
			virtual auto get_password(const std::string &username) -> std::string = 0;

			/**
			 * Start an external process
			 * @param path Path to executable
			 * @param arguments Program arguments
			 */
			virtual void start_process(const ghc::filesystem::path &path,
				const std::vector<std::string> &arguments) = 0;

			/**
			 * Start an external process and wait for the output
			 * @param path Path to executable
			 * @param arguments Program arguments
			 * @return Application output
			 */
			virtual auto start_process_and_wait(const ghc::filesystem::path &path,
				const std::vector<std::string> &arguments) -> std::string = 0;

			/**
			 * Save message to log
			 */
			void log_output(const std::string &message);

		private:
			const lib::settings &settings;
			const lib::paths &paths;
			std::vector<lib::log_message> log;
			ghc::filesystem::path path;
			lib::client_type client_type;

			/**
			 * Spotify client was compiled with PulseAudio support
			 */
			auto supports_pulse() -> bool;

			auto client_exec(const ghc::filesystem::path &client_path,
				std::vector<std::string> &arguments) -> std::string;

			auto get_sink_info() -> std::string;

			static auto get_client_type(const ghc::filesystem::path &path) -> lib::client_type;
		};
	}
}
