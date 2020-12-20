# spotify-qt-lib documentation
Welcome to the work-in-progress documentation for the work-in-progress spotify-qt-lib library. This library is the 
core of the main spotify-qt application and is intended for use in creating your own user interface or application 
that works like a Spotify client, for example, if Qt is not your thing. It's intended as a "batteries included" 
library, that is, it works as a wrapper around the Spotify Web API, but also handles caching and configuration for 
you. Currently, it's in very early beta, so expect a lot of breaking changes.

### How do I use it?
spotify-qt-lib is current part of the main spotify-qt project, so if you want to use it as a submodule (recommended),
you have to include the entire spotify-qt repository. You only have to include the CMake file in the lib folder 
however. You also need to implement some classes yourself, such as `paths`, which will be more documented once the 
library is more completed and finalized.

### Why isn't it separated from spotify-qt?
As it's the main user interface, it simply makes it easier to bundle it with spotify-qt. This could be changed in 
the future.

### How does versioning work?
spotify-qt-lib currently doesn't really use any versioning, but instead gets its version from spotify-qt. A major 
update to spotify-qt doesn't have to necessarily mean breaking changes to the library, and a minor update to 
spotify-qt could lead to breaking changes in the library, though, it's not very likely.

### Does it depend on Qt?
No. The only required external dependency currently is OpenSSL, as the Spotify Web API requires HTTPS.