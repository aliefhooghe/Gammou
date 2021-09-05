#!/usr/bin/python3

import argparse
import json

def generate_plugin(dest, src, modules):
    """
        dest : destination plugin json file
        src  : source plugin json file
    """
    input_file = open(src, "rb")
    output_file = open(dest, "w+")
    input_content = json.loads(input_file.read())
    input_content["modules"] = modules
    output_file.write(json.dumps(input_content, sort_keys=True, indent=4))

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Generate a plugin content file with given module list")
    parser.add_argument("-o", "--output", help="plugin json output file", nargs=1, required=True)
    parser.add_argument("--plugin-content", help="plugin json source file", nargs=1, required=True)
    parser.add_argument("--modules", help="additional bytecode module", nargs='+', required=True)
    args = parser.parse_args()

    generate_plugin(
        args.output[0],
        args.plugin_content[0],
        args.modules)