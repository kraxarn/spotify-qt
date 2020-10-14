<p align="center">
    <img src=".github/img/logo.svg"/>
</p>

![spotify-qt with breeze dark](.github/img/spotify-qt.png)

A Spotify client using Qt as a simpler, lighter alternative to the official client, inspired by [spotify-tui](https://github.com/Rigellute/spotify-tui).
Much like spotify-tui, you need an actual Spotify client running, for example [spotifyd](https://github.com/Spotifyd/spotifyd), which can be configured from within the app. Also like other clients, controlling music playback requires Spotify Premium.

Check out the [spotify-qt-quick](https://github.com/kraxarn/spotify-qt-quick) repo if you're interested in a Qt Quick UI.

## Supported platforms
Tested: Linux, Windows

Supported: macOS, *BSD, Haiku, any platform [officially supported](https://doc.qt.io/qt-5/supported-platforms.html) by Qt5

## Installing
spotify-qt is available from some package managers. If there are any package issues, contact the respective package maintainers, not me, unless I'm the package maintainer.
If you want to maintain the package on a system not in the list below, you are very much free to. Just please open an issue, or get in contact with me, so I can add it to the list.

| Platform | Package | Maintainer |
| -------- | ------- | ---------- |
| Arch (or Arch-based) | [AUR](https://aur.archlinux.org/packages/spotify-qt) | [kraxarn](https://github.com/kraxarn) |
| Haiku | [haikuports](https://github.com/haikuports/haikuports/tree/master/media-sound/spotify_qt) | [Begasus](https://github.com/Begasus) |
| NetBSD | [pkgsrc](https://mail-index.netbsd.org/pkgsrc-changes/2020/10/11/msg222510.html) | [voidpin](https://github.com/voidpin) |

## Building from Git
Make sure you have Qt (5.12+/6.0+), CMake (3.5+) and various build tools like `make`, `g++` and `git`.
```
git clone https://github.com/kraxarn/spotify-qt.git
cd spotify-qt
mkdir build
cd build
cmake ..
make
./spotify-qt
```