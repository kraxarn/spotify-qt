#include "view/log/application.hpp"
#include "lib/logging.hpp"

Log::Application::Application(QWidget *parent)
	: Base(parent)
{
}

auto Log::Application::getMessages() -> const QList<LogMessage> &
{
	return Logging::messages();
}
