#!/bin/bash

# Read the current version from version.txt
current_version=$(cat version.txt)

# Extract the build version
new_build=$(echo $current_version | awk -F '.' '{print $3 + 1}')

# Update version.txt with the new build
printf "%s" "$current_version" | awk -v new_build="$new_build" -F'.' '{$3=new_build; OFS="."; print $1, $2, $3}' > version.txt

# Display the updated version
echo $(cat version.txt)