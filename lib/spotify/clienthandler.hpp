#pragma once

#include "enum/clienttype.hpp"
#include "format.hpp"
#include "settings.hpp"
#include "util/date.hpp"
#include "util/file.hpp"
#include "util/strings.hpp"
#include "util/vector.hpp"

namespace lib
{
	namespace spt
	{
		/**
		 * Manages Spotify client
		 */
		class ClientHandler
		{
		public:
			/**
			 * Create new client handler instance
			 * (does not start the client)
			 * @param settings Settings for path etc.
			 */
			explicit ClientHandler(const lib::Settings &settings);

			/**
			 * Start the client
			 * @return Any possible error
			 */
			std::string start();

			/**
			 * Available backends for selected client
			 * @return Backends
			 */
			std::vector<std::string> available_backends();

			/**
			 * Check if the client is already running, accuracy not guaranteed
			 * @return If the client is running
			 */
			bool isRunning();

			/**
			 * Get version of spotifyd, just "librespot" or any empty string, depending on the client used
			 * @return Version
			 */
			std::string version();

			/**
			 * Get log from client
			 * @return Log as list with date and message
			 */
			static std::vector<std::pair<date, std::string>> log();

			/**
			 * Get current volume
			 * @remark Currently only works with the Pulseaudio backend
			 * @return Volume from 0.0-1.0
			 */
			static float get_volume();

			/**
			 * Set volume
			 * @remark Currently only works with the Pulseaudio backend
			 * @param value Volume from 0.0-1.0
			 */
			static void set_volume(float value);

		private:
			/**
			 * Instance of settings
			 */
			const lib::Settings &settings;

			/**
			 * Backend was compiled with Pulseaudio support
			 * @return Pulseaudio support
			 */
			bool supportsPulse();

			/**
			 * Start client and get output
			 * @param arguments Arguments to pass
			 * @return Command output, or empty if error
			 */
			std::string exec(const std::vector<std::string> &arguments);

			/**
			 * Get info about the sink used for audio playback
			 * @return Sink info
			 */
			static std::string sink_info();

			/**
			 * Get type of client in path
			 * @return Type of client
			 */
			static lib::ClientType client_type();

			/**
			 * Get path to client
			 * @return Path to client
			 */
			std::string path();

			/**
			 * Get path to file where output from Spotify client is saved
			 * @return Log path
			 */
			std::string log_path();

			/**
			 * Join arguments into a string
			 * @param args Arguments
			 * @return Joined arguments
			 */
			 std::string join_args(const std::vector<std::string> &args);
		};
	}
}