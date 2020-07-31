
function loadPlaylists() {
	spotify.playlists.forEach(playlist => {
		listModel.append({
			"text": playlist.name
		})
	})
}