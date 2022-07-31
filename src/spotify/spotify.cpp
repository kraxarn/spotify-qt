#include "spotify/spotify.hpp"

spt::Spotify::Spotify(lib::settings &settings,
	const lib::http_client &httpClient, QObject *parent)
	: QObject(parent), lib::spt::api(settings, httpClient)
{
}

void spt::Spotify::select_device(const std::vector<lib::spt::device> &devices,
	lib::callback<lib::spt::device> &callback)
{
	auto *parentWidget = dynamic_cast<QWidget *>(parent());
	auto *dialog = new Dialog::DeviceSelect(devices, parentWidget);
	QDialog::connect(dialog, &Dialog::DeviceSelect::deviceSelected, callback);
	dialog->open();
}
