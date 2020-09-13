
function playbackChanged() {
	let playback = spotify.playback
	let item = playback["item"]

	if (!playback["is_playing"] && playback["item"]["name"] === "(no name)") {
		currentArtist.text = null
		currentTrack.text = "No media playing"
		footer.playPause.icon.name = "media-playback-start"
		return
	}

	currentArtist.text = item["artist"]
	currentTrack.text = item["name"]
	currentAlbum.source = item["image"]
	currentTrackId = item["id"]

	footer.playPause.icon.name = `media-playback-${playback["is_playing"] ? "pause" : "start"}`
	footer.progress.to = item["duration"]
	footer.progress.value = playback["progress_ms"]
	footer.position.text = `${utils.formatTime(playback["progress_ms"])}/${utils.formatTime(item["duration"])}`
}

function playTrack(id) {
	let status = spotify.playTracksWithContext(`spotify:track:${id}`, root.currentContext)
	if (status) {
		console.log(status)
	} else {
		spotify.requestPlayback()
	}
}

function loadTracks(tracks) {
	trackListModel.clear()
	tracks.forEach(track => trackListModel.append({
		"id": track["id"],
		"artist": track["artist"],
		"track": track["name"]
	}))
}

function loadArtist(id) {
	drawer.close()
	artistDrawer.load(spotify.getArtist(id))
}

function logOut(mode) {
	settings.logOut(mode)
	dialogLoggedOut.open()
}