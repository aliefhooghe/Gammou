#!/usr/bin/python3

import json
import sys


def generate_plugin(dest, src, modules):
    """
        dest : desitnation plugin json file
        src  : source plugin json file
    """
    input_file = open(src, "rb")
    output_file = open(dest, "w+")
    input_content = json.loads(input_file.read())
    input_content["modules"] = modules
    output_file.write(json.dumps(input_content, sort_keys=True, indent=4))

if __name__ == '__main__':
    dest = sys.argv[1]
    src = sys.argv[2]
    modules = sys.argv[3:]
    generate_plugin(dest, src, modules)