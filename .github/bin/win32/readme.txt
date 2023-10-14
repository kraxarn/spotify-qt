Qt used to have OpenSSL prebuilt binaries for Windows x86, but have since removed them, probably because they dropped
support for Windows x86 with the release of Qt 6, now requiring Windows x64. To avoid further issues like this,
spotify-qt is planned to drop pre-built binaries for Windows x86 with the release of spotify-qt v4.0, but still keep
support for Qt 5 for those who wants to build it themselves.