#!/bin/python
# Checks for updates in third-party libraries
import os
import typing

import httpx

success = True

http = httpx.Client(headers={
	"Accept": "application/vnd.github.v3+json",
})


def log(lib_name: str, current_version: str, latest_version: str):
	if current_version == latest_version:
		print(f" {lib_name} is up-to-date ({current_version})")
	else:
		print(f" {lib_name} {latest_version} is available (current is {current_version})")
		global success
		success = False


def get_latest_tag(repo_name: str, include_prerelease: bool) -> str:
	tags = http.get(f"https://api.github.com/repos/{repo_name}/tags").json()
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
			latest = get_latest_tag(f"{repo[len(repo) - 2]}/{repo[len(repo) - 1]}", False)
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

actions = {}

for filename in os.listdir(workflows_dir):
	basename = filename.decode()
	file_path = os.path.join(workflows_dir, filename).decode()
	with open(file_path, "r") as file:
		for line in file:
			if "uses:" in line:
				action = line[line.index("uses:") + 5:].strip().split("@")
				if action[0] in actions.keys() and action[1] != actions[action[0]]:
					print(f" Mismatch {action[0]}: {action[1]} != {actions[action[0]]}")
				else:
					actions[action[0]] = action[1]

for action_name, action_version in actions.items():
	repo_name = "/".join(action_name.split("/")[:2])
	latest_tag = get_latest_tag(repo_name, False)
	action_latest = latest_tag if latest_tag.startswith("v0") else latest_tag[:2]
	log(action_name, action_version, action_latest)

# res/ic

latest_ic = get_latest_tag("KDE/breeze-icons", True)
with open("../res/ic/version", "r") as f:
	current_ic = f.read()
log("breeze-icons", current_ic, latest_ic)

http.close()
exit(0 if success else 1)
