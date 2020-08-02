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

function authenticate() {
	if (!clientId.text || !clientSecret.text) {
		authError("Both client id and client secret are required")
		return
	}

	authWeb.url = utils.sptAuthUrl(clientId.text)
	authDrawer.open()
}

function authError(msg) {
	authErrorMsg.text = msg
	authErrorDialog.open()
}

function webLoadingChanged(request) {
	if (!request.url.toString().startsWith("http://localhost:8888")) {
		return
	}
	authDrawer.close()

	let code = utils.extractUrlQuery(request.url)["code"]
	if (!code) {
		authError("Authentication failed")
		return
	}

	let status = utils.sptAuth(code, clientId.text, clientSecret.text)
	if (!status.success) {
		authError(status.message)
		return
	}

	root.close()
	Qt.quit()
}