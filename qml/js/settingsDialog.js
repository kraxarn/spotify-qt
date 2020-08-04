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

	if (general["style"] && style.model[general["style"]]) {
		for (let i = 0; i < style.model.length; i++) {
			if (general["style"] === style.model) {
				style.currentIndex = i
				break
			}
		}
	}

	spotifyPlaybackOrder.checked = general["spotify_playback_order"]
	whatsNewOnStart.checked = general["show_changelog"]
}

function save() {
	let settings = getSettings()

	// General
	let general = settings.general
	general["refresh_interval"] = parseInt(refreshInterval.currentValue)
	general["style"] = style.currentValue
	general["spotify_playback_order"] = spotifyPlaybackOrder.checked
	general["show_changelog"] = whatsNewOnStart.checked

	// ...

	settings.general = general
	root.close()
}