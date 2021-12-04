All contributions are very welcome, not just code-related contributions!

## Code style
Code style is different in lib/ application logic and src/ UI logic. lib/ tries to follow a 
style common with the standard library, see lib/style_guide.md for more details, while src/ tries 
to follow a style common with Qt.

## Namespaces
To better scope different parts of the UI, most classes are scoped to their respective directories,
following a similar naming scheme, except starting with an upper-case letter. For example,
all classes in the `dialog/` should be in a `Dialog` namespace. The only exception to this is
classes in the `util/`, `view/` and `widget/` folders. If those folders contain sub-folders 
however, the namespace should reflect the name of the sub-folder.