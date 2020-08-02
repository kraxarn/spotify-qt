
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

	footer.playPause.icon.name = `media-playback-${playback["is_playing"] ? "pause" : "start"}`
	footer.progress.to = item["duration"]
	footer.progress.value = playback["progress_ms"]
	footer.position.text = `${utils.formatTime(playback["progress_ms"])}/${utils.formatTime(item["duration"])}`
}