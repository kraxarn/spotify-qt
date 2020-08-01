
function loadPlaylists() {
	spotify.playlists.forEach(playlist => {
		listModel.append({
			"id": playlist.id,
			"name": playlist.name,
		})
	})
}

function clickedPlaylist(model) {
	console.log(`"${model.name}": spotify:playlist:${model.id}`)
}