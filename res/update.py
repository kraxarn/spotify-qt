#!/bin/python
# Checks for updates in third-party libraries

import requests

# lib/thirdparty

with open("../lib/thirdparty/readme.md") as file:
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

with open("../.github/workflows/linux.yml") as file:
	version: str
	latest: str
	# Get current from file
	for line in file:
		if "QT_VERSION" in line:
			version = "v{0}".format(line[line.index("\"") + 1:line.rindex("\"")])
			break
	# Get latest stable from API
	for tags in requests.get("https://api.github.com/repos/qt/qtbase/tags").json():
		if "-" not in tags["name"]:
			latest = tags["name"]
			break
	# Check
	if latest == version:
		print(f"Qt is up-to-date ({version})")
	else:
		print(f"Qt {latest} is available (current is {version})")
