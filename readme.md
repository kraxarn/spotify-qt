# spotify-qt
A Spotify client using Qt as a simpler, lighter alternative to the official client, inspired by [spotify-tui](https://github.com/Rigellute/spotify-tui).
Much like spotify-tui, you need an actual Spotify client running, for example [spotifyd](https://github.com/Spotifyd/spotifyd). You can also use the embedded music player if you don't feel like setting more stuff up. Keep in mind it's still pretty buggy and creates a browser instance, however, it's still more lightweight than the official client.

Currently in beta. Most things should work, but don't be surprised when unexpected things happen.
It's also not currently available in any package manager, though, when it's a bit more stable, it will be available through AUR.

## Installing/building:
Make sure you have Qt (5.14+), CMake (3.5+) and various build tools like `make` and `g++`.
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
* All network requests are done on the main thread.

## Not implemented yet:
* Library.
* Media controls (d-bus).
* Settings (for spotifyd etc.).
* Logging out.
* Add to or remove from playlist.
* Searching.
* Artist/album view.