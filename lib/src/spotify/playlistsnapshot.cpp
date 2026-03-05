#include "lib/spotify/playlistsnapshot.hpp"
#include "lib/json.hpp"

auto PlaylistSnapshot::fromJson(const QVariant &json) -> PlaylistSnapshot
{
	PlaylistSnapshot result;

	JsonUtil::getTo(json.toMap(), QStringLiteral("snapshot_id"), result.mSnapshotId);

	return result;
}

auto PlaylistSnapshot::snapshotId() -> const QString &
{
	return mSnapshotId;
}
