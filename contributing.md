All contributions are very welcome, not just code-related contributions!

## Code style

Code style is different in lib/ application logic and src/ UI logic. lib/ tries
to follow a style common with the standard library, see lib/style_guide.md for
more details, while src/ tries to follow a style common with Qt. Newer code
in lib/, making heavier use of Qt, tries to follow the same style as src/.

## Namespaces

To better scope different parts of the UI, most classes are scoped to their respective directories,
following a similar naming scheme, except starting with an upper-case letter. For example,
all classes in the `dialog/` should be in a `Dialog` namespace. The only exception to this is
classes in the `util/`, `view/` and `widget/` folders. If those folders contain sub-folders 
however, the namespace should reflect the name of the sub-folder.

## Generative AI policy

**Pull requests generated fully by generative AI, sometimes known as
"slop generators", are prohibited and won't be merged.** The simple reason for
this is that the quality is just way too poor to be maintainable, and because
it heavily discourages actual learning. Please keep these changes in your own
fork instead. Using AI as a tool, for example for single-line completion,
is allowed, as detecting these pull requests are more or less impossible anyway.
