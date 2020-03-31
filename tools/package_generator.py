#!/usr/bin/python3

import json
import sys


def generate_package(dest, src, plugin_contents):
    input_file = open(src, "r")
    output_file = open(dest, "w+")

    input_content = json.loads(input_file.read())
    input_content["plugins"] = []

    for p in plugin_contents:
        with open(p, "r") as f:
            input_content["plugins"].append(json.loads(f.read()))

    output_file.write(json.dumps(input_content, sort_keys=True, indent=4))

if __name__ == '__main__':
    dest = sys.argv[1]
    src = sys.argv[2]
    plugin_contents = sys.argv[3:]
    generate_package(dest, src, plugin_contents)