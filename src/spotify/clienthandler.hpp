#pragma once

#include "../keyring/kwallet.hpp"
#include "lib/spotify/clienthandler.hpp"
#include "util/qtutils.hpp"

#include <QDateTime>
#include <QFileInfo>
#include <QInputDialog>
#include <QList>
#include <QProcess>
#include <QStringList>

namespace spt
{
	class ClientHandler: public QObject, public lib::spt::client_handler
	{
	Q_OBJECT

	public:
		ClientHandler(const lib::settings &settings, const lib::paths &paths,
			QWidget *parent);
		~ClientHandler() override;

	protected:
		auto get_keyring_password(const std::string &username) -> std::string override;

		void set_keyring_password(const std::string &username,
			const std::string &password) override;

		auto get_password(const std::string &username) -> std::string override;

		void start_process(const ghc::filesystem::path &path,
			const std::vector<std::string> &arguments) override;

		auto start_process_and_wait(const ghc::filesystem::path &path,
			const std::vector<std::string> &arguments) -> std::string override;

	private:
		const lib::settings &settings;
		QProcess *process = nullptr;

		void readyRead();
		void readyError();
	};
}