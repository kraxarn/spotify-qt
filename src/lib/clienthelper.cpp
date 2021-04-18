#include "clienthelper.hpp"

spt::ClientHelper::ClientHelper()
	: lib::cl::handler()
{
}

auto spt::ClientHelper::get_keyring_password(const std::string &/*username*/) -> std::string
{
	notImplemented("get_keyring_password");
	return std::string();
}

void spt::ClientHelper::set_keyring_password(const std::string &/*username*/,
	const std::string &/*password*/)
{
	notImplemented("set_keyring_password");
}

auto spt::ClientHelper::get_password(const std::string &/*username*/) -> std::string
{
	notImplemented("get_password");
	return std::string();
}

void spt::ClientHelper::start_process(const ghc::filesystem::path &/*path*/,
	const std::vector<std::string> &/*arguments*/)
{
	notImplemented("start_process");
}

auto spt::ClientHelper::start_process_and_wait(const ghc::filesystem::path &path,
	const std::vector<std::string> &arguments) const -> std::string
{
	QProcess process;

	process.start(QString::fromStdString(path),
		QtUtils::toStringList(arguments));
	process.waitForFinished();

	return process.readAllStandardOutput()
		.trimmed().toStdString();
}

void spt::ClientHelper::notImplemented(const std::string &methodName)
{
	lib::log::warn("ClientHelper: {} is not implemented", methodName);
}
