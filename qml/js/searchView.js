let results

function search(query) {
	rowSearch.enabled = false
	results = spotify.search(query)
	updateItems()
	rowSearch.enabled = true
}

function getSelectedType() {
	let name = menuSearchSelectedType.icon.name
	return name === "view-media-track"
		? "tracks" : name === "view-media-artist"
			? "artists" : name === "view-media-album-cover"
				? "albums" : "playlists"
}

function updateItems() {
	let items = results[getSelectedType()]

	searchListModel.clear()
	items.forEach(item => {
		searchListModel.append({
			"id": item.id,
			"name": item.name
		})
	})
}

function clickedItem(id) {
	switch (getSelectedType()) {
		case "tracks":
			spotify.playTrack(`spotify:track:${id}`)
			break
	}
}