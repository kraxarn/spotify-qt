#include "util/process.hpp"

#include <QProcess>

auto Process::exec(const QString &path, const QStringList &args) -> lib::result<QString>
{
	QProcess process;
	process.start(path, args, QIODevice::ReadOnly);
	process.waitForFinished();

	if (process.error() != QProcess::UnknownError)
	{
		return lib::result<QString>::fail(process.errorString().toStdString());
	}

	const auto output = process.readAllStandardOutput().trimmed();
	if (!output.isEmpty())
	{
		return lib::result<QString>::ok(output);
	}

	const auto error = process.readAllStandardError().trimmed();
	return lib::result<QString>::ok(error);
}
