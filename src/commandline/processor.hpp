#pragma once

#include <QCommandLineParser>

namespace CommandLine
{
	/**
	 * Processes command line options
	 */
	class Processor
	{
	public:
		static auto process(const QCommandLineParser &parser) -> bool;

	private:
		Processor() = default;
	};
}
