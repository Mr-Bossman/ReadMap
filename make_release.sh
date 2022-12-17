#!/bin/bash
shopt -s expand_aliases
if command -v podman &> /dev/null
then
	echo "Using podman"
	alias containers="podman"
else
	if ! command -v docker &> /dev/null
	then
		echo "Requers docker or podman could not be found"
		exit
	fi
	echo "Using docker"
	alias containers="docker"
fi

cat <<EOF > Dockerfile
FROM archlinux:latest
WORKDIR /mnt
CMD ./makewin.sh
EOF

containers build -t readmap .
containers run -it --rm -v $(pwd):/mnt readmap
containers image rm readmap
