#include "util/process.hpp"

#include <QProcess>

auto Process::exec(const QString &path, const QStringList &args) -> Result<QString>
{
	QProcess process;
	process.start(path, args, QIODevice::ReadOnly);
	process.waitForFinished();

	if (process.error() != QProcess::UnknownError)
	{
		return Result<QString>::fail(process.errorString());
	}

	const QByteArray output = process.readAllStandardOutput().trimmed();
	if (!output.isEmpty())
	{
		return Result<QString>::ok(output);
	}

	const QByteArray error = process.readAllStandardError().trimmed();
	return Result<QString>::ok(error);
}
