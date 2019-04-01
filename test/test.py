#!/usr/bin/env python3
# Processes test images and compares
# the result against expected images.

import os
import subprocess

from PIL import Image


BINARY     = "../build/fillalpha"
INPUT_DIR  = "input"
EXPECT_DIR = "expect"
OUTPUT_DIR = "output"


def testImage(expectFile):
	# Extract options
	tokens = expectFile[:-4].split("_")
	basename = tokens[0]
	options = tokens[1:]

	# Build file paths
	inputPath  = INPUT_DIR  + "/" + basename + ".png"
	outputPath = OUTPUT_DIR + "/" + expectFile
	expectPath = EXPECT_DIR + "/" + expectFile

	# Process image
	subprocess.call([BINARY] + options + [inputPath, outputPath])
	outputImage = Image.open(outputPath).convert("RGBA")
	expectImage = Image.open(expectPath).convert("RGBA")

	# Compare
	return list(outputImage.getdata()) == list(expectImage.getdata())


if __name__ == "__main__":
	# Prepare directory
	if not os.path.exists(OUTPUT_DIR):
		os.makedirs(OUTPUT_DIR)

	# Run tests
	allOk = True
	for expectFile in sorted(os.listdir(EXPECT_DIR)):
		isOk = testImage(expectFile)
		allOk &= isOk
		print("  ok " if isOk else " fail", expectFile)
	print("  OK" if allOk else " FAIL")
