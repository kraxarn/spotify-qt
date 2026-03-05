#pragma once

#include <QString>

class PlaylistSnapshot final
{
public:
	[[nodiscard]]
	static auto fromJson(const QVariant &json) -> PlaylistSnapshot;

	[[nodiscard]]
	auto snapshotId() -> const QString &;

private:
	PlaylistSnapshot() = default;

	QString mSnapshotId;
};
