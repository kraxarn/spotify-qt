#pragma once

#include <QString>
#include <QVariant>

class PlaylistSnapshot final
{
public:
	[[nodiscard]]
	static auto fromJson(const QVariant &json) -> PlaylistSnapshot;

	[[nodiscard]]
	auto snapshotId() const -> const QString &;

private:
	PlaylistSnapshot() = default;

	QString mSnapshotId;
};
