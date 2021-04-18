#pragma once

#include "lib/client/handler.hpp"
#include "lib/log.hpp"
#include "util/qtutils.hpp"

#include <QProcess>

namespace spt
{
	/**
	 * More lightweight version of ClientHandler with no Spotify playback handler
	 */
	class ClientHelper: public lib::cl::handler
	{
	public:
		ClientHelper();

	protected:
		/**
		 * @note Not implemented
		 */
		auto get_keyring_password(const std::string &username) -> std::string override;

		/**
		 * @note Not implemented
		 */
		void set_keyring_password(const std::string &username,
			const std::string &password) override;

		/**
		 * @note Not implemented
		 */
		auto get_password(const std::string &username) -> std::string override;

		/**
		 * @note Not implemented
		 */
		void start_process(const ghc::filesystem::path &path,
			const std::vector<std::string> &arguments) override;

		auto start_process_and_wait(const ghc::filesystem::path &path,
			const std::vector<std::string> &arguments) const -> std::string override;

	private:
		static void notImplemented(const std::string &methodName);
	};
}
