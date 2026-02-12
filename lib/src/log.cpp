#include "lib/log.hpp"
#include "lib/logging.hpp"

#include <QtLogging>

void lib::log::message(const QtMsgType log_type, const std::string &message)
{
	Logging::message(log_type, {}, QString::fromStdString(message));
}
