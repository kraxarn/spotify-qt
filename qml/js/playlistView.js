function loadPlaylists() {
	spotify.playlists.forEach(playlist => {
		listModel.append({
			"id": playlist.id,
			"name": playlist.name,
		})
	})
}

function loadPlaylist(id) {
	trackList.enabled = false

	let tracks = spotify.getPlaylistTracks(id)
	trackListModel.clear()

	tracks.forEach(track => trackListModel.append({
		"id": track["id"],
		"artist": track["artist"],
		"track": track["name"]
	}))

	trackList.enabled = true
	root.currentContext = `spotify:playlist:${id}`
}

function clickedPlaylist(model) {
	drawer.close()
	loadPlaylist(model.id)
}