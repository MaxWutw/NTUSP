#!/bin/bash

sample_directory="./sample-testcases"
public_directory="./public-testcases"

for file in "$sample_directory"/*; do
	echo "$file"
	if [[ "$file" == *.in ]]; then
		echo "$file"
	fi
done
