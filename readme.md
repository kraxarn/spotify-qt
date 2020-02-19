# spotify-qt
A Spotify client using Qt as a simpler, lighter alternative to the official client, inspired by [spotify-tui](https://github.com/Rigellute/spotify-tui).
Much like spotify-tui, you need an actual Spotify client running, for example [spotifyd](https://github.com/Spotifyd/spotifyd).

Currently in beta. Most things should work, but don't be surprised when unexpected things happen.
It's also not currently available in any package manager, though, when it's a bit more stable, it will be available through AUR.

## Installing/building:
```
git clone https://github.com/kraxarn/spotify-qt.git
cd spotify-qt
mkdir build
cd build
cmake ..
make
./spotify-qt
```

## Known issues:
* Nothing is cached, so album art, playlists, etc. is fetched every time.
* Spotify API is pinged every second for updates.
* Probably very unstable.
* `SPOTIFY_QT_ID` and `SPOTIFY_QT_SECRET` needs to be set before using the app.
* First sign in isn't very good.

## Not implemented yet:
* Library.
* Media controls (d-bus).
* Settings (for spotifyd etc.).
* Logging out.