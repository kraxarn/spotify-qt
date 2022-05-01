#pragma once

#include <QCommandLineParser>

namespace Ipc
{
	/**
	 * Handles sending various supported IPC messages
	 */
	class Handler
	{
	public:
		static auto process(const QCommandLineParser &parser) -> bool;

	private:
		Handler() = default;
	};
}
