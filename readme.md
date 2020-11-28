# spotify-qt v4.0

The current version for spotify-qt v4.0, breaking out the main logic in a separate project to allow better 
support for other user interfaces than Qt Widgets, like Qt Quick and GTK. All Qt dependencies should also be 
removed in the main library, to allow other user interfaces to be used without any Qt dependencies.