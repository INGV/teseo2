#!/bin/bash

echo "Your container args are ($#): \"$@\""

if [ $# -eq 0 ]; then
	bash -i -c "gimp"
else
	# An interactive shell is one started without non-option arguments and without
	# the -c option whose standard input and error are both connected to terminals
	# ... or one started with the -i option.
	# bash -i -c "$*"
	$*
fi

