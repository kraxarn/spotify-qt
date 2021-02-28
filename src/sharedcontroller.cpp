#include "sharedcontroller.hpp"

#define SHARED_CONTROLLER_KEY "spotify-qt-shared-controller"

SharedController::SharedController(QObject *parent)
	: QSharedMemory(SHARED_CONTROLLER_KEY, parent)
{
	callback = new std::function<void(const std::string &data)>([this](const std::string &data)
	{
		lib::log::dev("shared controller data: {}", data);
	});

	write(callback, sizeof(callback));
}

SharedController::~SharedController()
{
	delete callback;
	deleteLater();
}

bool SharedController::write(void *value, int len)
{
	if (!create(len))
	{
		lib::log::error("Failed to create shared memory: {}",
			errorString().toStdString());
		return false;
	}

	lock();
	memcpy((char *) data(), value, qMin(size(), len));
	unlock();

	return true;
}

void SharedController::send(const std::string &message)
{
	QSharedMemory memory(SHARED_CONTROLLER_KEY);
	memory.lock();
	{
		if (memory.data() == nullptr)
		{
			lib::log::error("Failed to send data: No shared memory data");
		}
		else
		{
			auto callback = static_cast<std::function<void(const std::string &)> *>(memory.data());
			if (callback == nullptr)
				lib::log::error("Failed to send data: Not in a valid state");
			else
				(*callback)(message);
		}
	}
	memory.unlock();
}
