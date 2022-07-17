#!/bin/python
# Checks for updates in third-party libraries
import os
import typing

import requests

success = True


def log(lib_name: str, current_version: str, latest_version: str):
	if current_version == latest_version:
		print(f"✓️ {lib_name} is up-to-date ({current_version})")
	else:
		print(f"⨯ {lib_name} {latest_version} is available (current is {current_version})")
		global success
		success = False


def get_latest_tag(repo_name: str, include_prerelease: bool) -> str:
	tags = requests.get(f"https://api.github.com/repos/{repo_name}/tags").json()
	for tag in tags:
		if include_prerelease or "-" not in tag["name"]:
			return tag["name"]
	return ""


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
			log(name, version, latest)


# .github/workflows

def get_qt_version_from_workflow(filename: str) -> typing.Optional[str]:
	with open(filename, "r") as file:
		for line in file:
			if "QT_VERSION" in line:
				return "v{0}".format(line[line.index("\"") + 1:line.rindex("\"")])
	return None


latest_qt = get_latest_tag("qt/qtbase", False)
workflows_dir = os.fsencode("../.github/workflows/")

for file in os.listdir(workflows_dir):
	basename = file.decode()
	file_path = os.path.join(workflows_dir, file).decode()
	version = get_qt_version_from_workflow(file_path)
	if version is None or version.startswith("v5"):
		continue
	log(f"Qt ({basename})", version, latest_qt)

# res/ic

latest_ic = get_latest_tag("KDE/breeze-icons", True)
with open("../res/ic/version", "r") as f:
	current_ic = f.read()
log("breeze-icons", current_ic, latest_ic)

exit(0 if success else 1)
