All contributions are very welcome, not just code-related contributions!

## Code style
Code style is different in lib/ application logic and src/ UI logic. lib/ tries to follow a 
style common with the standard library, see lib/style_guide.md for more details, while src/ tries 
to follow a style common with Qt.

## Folder structure
At least in practise. A lot of older code doesn't follow this.

| Folder | Sub-folder    | Description |
| ------ | ------------- | ----------- |
| docs/  |               | Configuration for Doxygen. |
| lib/   |               | Main application logic. Anything unrelated to the UI should be here. |
|        | include/      | Headers. |
|        | qt/           | Optional non-UI Qt utilities. |
|        | src/          | Source files. |
|        | test/         | Unit tests. |
| res/   |               | Non-code assets, like images and icons. |
| snap/  |               | Configuration for Snap. |
| src/   |               | Main application UI source code. |
|        | client/       | librespot/spotifyd logic. |
|        | dialog/       | Dialogs, or anything inheriting `QDialog`. |
|        | enum/         | Enums, either `enum` or `enum class`. |
|        | keyring/      | Password saving logic. |
|        | lib/          | Implementations of abstract classes from lib. |
|        | list/         | Lists, or anything inheriting `QListWidget`, `QTreeWidget`, etc. |
|        | listitem/     | Items in lists, or anything inheriting `QListWidgetItem`, etc. |
|        | mediaplayer/  | System media player implementations, like D-bus MPRIS. |
|        | menu/         | Menus, or anything inheriting `QMenu`. |
|        | model/        | Models for lists, or anything inheriting `QAbstractListModel`, etc. |
|        | role/         | Roles for models. |
|        | settingspage/ | Pages for `SettingsDialog`.
|        | spotify/      | Qt UI specific Spotify logic. |
|        | util/         | Various utilities. |
|        | view/         | Various views, usually not reused. |
|        | widget/       | Other various custom widgets, can be reused. |

## Namespaces
To better scope different parts of the UI, most classes are scoped to their respective directories,
following a similar naming scheme, except starting with an upper-case letter. For example,
all classes in the `dialog/` should be in a `Dialog` namespace. The only exception to this is
classes in the `util/`, `view/` and `widget/` folders. If those folders contain sub-folders 
however, the namespace should reflect the name of the sub-folder.