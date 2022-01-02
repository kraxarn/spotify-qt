#include "view/log/application.hpp"
#include "lib/log.hpp"

Log::Application::Application(QWidget *parent)
	: Base(parent)
{
}

auto Log::Application::getMessages() -> const std::vector<lib::log_message> &
{
	return lib::log::get_messages();
}
