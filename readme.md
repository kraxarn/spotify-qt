# spotify-qt
A Spotify client using Qt as a simpler, lighter alternative to the official client, inspired by [spotify-tui](https://github.com/Rigellute/spotify-tui).
Much like spotify-tui, you need an actual Spotify client running, for example [spotifyd](https://github.com/Spotifyd/spotifyd), which can be configured from within the app.

Currently in beta. Most things should work, but don't be surprised when unexpected things happen.
It's also not currently available in any package manager, though, when it's a bit more stable, it will be available through AUR.

## Installing/building:
Make sure you have Qt (5.14+), CMake (3.5+) and various build tools like `make`, `g++` and `git`.
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
* Spotify API is pinged every second for updates.
* Probably very unstable.
* All network requests are done on the main thread.
* D-Bus media controller is buggy (see issue [#4](https://github.com/kraxarn/spotify-qt/issues/4)).

## Not implemented yet:
* Library (made for you, recently played, etc.).