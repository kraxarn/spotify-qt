#!/bin/python
# Checks for unused icons
import os
import pathlib


def icon_dir() -> pathlib.Path:
	return pathlib.Path.cwd().parent.joinpath("res", "ic")


def source_dir() -> pathlib.Path:
	return pathlib.Path.cwd().parent.joinpath("src")


icon_names: list[str] = []
for filename in os.listdir(os.path.join(icon_dir(), "dark")):
	icon_names.append(pathlib.Path(filename).stem)
total_count = len(icon_names)

for root, dirs, files in os.walk(source_dir()):
	root_path = pathlib.Path(root)
	for filename in files:
		file_path = root_path.joinpath(filename)
		if file_path.suffix != ".cpp":
			continue
		with open(file_path, "r") as file:
			content = file.read()
			for icon_name in icon_names:
				if icon_name in content:
					icon_names.remove(icon_name)

if len(icon_names) == 0:
	print("Found no unused icons")
	exit(0)

count = len(icon_names)
suffix = "icon" if count == 1 else "icons"
print(f"Found {count}/{total_count} unused {suffix}:")
print(os.linesep.join(icon_names))
exit(1)
