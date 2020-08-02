function getWelcomeText() {
	return "Before using the app, you need to setup your Spotify Web API keys.\n" +
		"You can do this by opening the Spotify Dashboard, create a new app and \n" +
		"set the redirect uri (not website) to http://localhost:8888.\n" +
		"Then, enter your Client ID and secret below from the same application page:"
}

function openDashboard() {
	if (Qt.openUrlExternally("https://developer.spotify.com/dashboard/applications")) {
		return
	}
	openLinkDialog.linkText = "https://developer.spotify.com/dashboard/applications"
	openLinkDialog.open()
}