function getSettings() {
	return Qt.createQmlObject("import com.kraxarn.settings 1.0; Settings{}", root)
}

function reload() {
	let settings = getSettings()

	// General
	let general = settings.general
	refreshInterval.currentIndex = general["refresh_interval"] === 1
		? 0 : general["refresh_interval"] === 10
			? 2 : 1

	for (let i = 0; i < style.model.length; i++) {
		if (general["style"] === style.model[i]) {
			style.currentIndex = i
			break
		}
	}

	spotifyPlaybackOrder.checked = general["spotify_playback_order"]
	whatsNewOnStart.checked = general["show_changelog"]

	// Spotify
	let spotify = settings.spotify
	sptPath.text = spotify["path"]
	startWithApp.checked = spotify["start_client"]
	alwaysStart.checked = spotify["always_start"]
	globalConfig.checked = spotify["global_config"]
}

function save() {
	let settings = getSettings()

	// General
	let general = settings.general
	general["refresh_interval"] = parseInt(refreshInterval.currentValue)
	general["style"] = style.currentValue
	general["spotify_playback_order"] = spotifyPlaybackOrder.checked
	general["show_changelog"] = whatsNewOnStart.checked

	// Spotify
	let spotify = settings.spotify
	spotify["path"] = sptPath.text
	spotify["start_client"] = startWithApp.checked
	spotify["always_start"] = alwaysStart.checked
	spotify["global_config"] = globalConfig.checked

	settings.general = general
	settings.spotify = spotify
	root.close()
}