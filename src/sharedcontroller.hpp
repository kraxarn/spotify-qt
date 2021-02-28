#pragma once

#include <QSharedMemory>
#include <QBuffer>
#include <QDataStream>

#include <functional>

#include "lib/log.hpp"

class SharedController: QSharedMemory
{
public:
	explicit SharedController(QObject *parent);
	~SharedController() override;

	static void send(const std::string &message);

private:
	std::function<void(const std::string &data)> *callback = nullptr;

	bool write(void* data, int size);
};
