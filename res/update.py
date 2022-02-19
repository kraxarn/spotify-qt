#!/bin/python
# Checks for updates in third-party libraries
import os
import typing

import requests

# lib/thirdparty

with open("../lib/thirdparty/readme.md", "r") as file:
	name: str
	latest: str
	for line in file:
		if line.startswith("##"):
			name = line[line.index("[") + 1:line.index("]")]
			repo = line[line.index("(") + 1:line.index(")")].split("/")
			url = f"https://api.github.com/repos/{repo[len(repo) - 2]}/{repo[len(repo) - 1]}/tags"
			latest = requests.get(url).json()[0]["name"]
		if line.startswith("v"):
			version = line[:line.index(",")]
			if latest == version:
				print(f"{name} is up-to-date ({version})")
			else:
				print(f"{name} {latest} is available (current is {version})")


# .github/workflows

def get_latest_qt_version() -> str:
	for tags in requests.get("https://api.github.com/repos/qt/qtbase/tags").json():
		if "-" not in tags["name"]:
			return tags["name"]
	return ""


def get_qt_version_from_workflow(filename: str) -> typing.Optional[str]:
	with open(filename, "r") as file:
		for line in file:
			if "QT_VERSION" in line:
				return "v{0}".format(line[line.index("\"") + 1:line.rindex("\"")])
	return None


latest = get_latest_qt_version()
workflows_dir = os.fsencode("../.github/workflows/")

for file in os.listdir(workflows_dir):
	basename = file.decode()
	file_path = os.path.join(workflows_dir, file).decode()
	version = get_qt_version_from_workflow(file_path)
	if version is None or version.startswith("v5"):
		continue
	if latest == version:
		print(f"Qt in {basename} is up-to-date ({version})")
	else:
		print(f"Qt {latest} is available in {basename} (current is {version})")
