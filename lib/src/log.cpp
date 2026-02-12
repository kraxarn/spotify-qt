#include "lib/log.hpp"
#include "lib/logging.hpp"

void lib::log::message(const log_type log_type, const std::string &message)
{
	QtMsgType msg_type;
	switch (log_type)
	{
		case log_type::information:
			msg_type = QtInfoMsg;
			break;

		case log_type::warning:
			msg_type = QtWarningMsg;
			break;

		case log_type::error:
			msg_type = QtCriticalMsg;
			break;

		default:
			msg_type = QtDebugMsg;
			break;
	}

	Logging::message(msg_type, {}, QString::fromStdString(message));
}
