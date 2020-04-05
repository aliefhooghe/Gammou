#!/usr/bin/python3

import argparse
import json
import sys

def generate_content_file(output, package_content, plugin_contents, common_libs):
    #   Build plugin list
    package_plugins = []
    for p in plugin_contents:
        with open(p, "r") as f:
            package_plugins.append(json.loads(f.read()))

    #   Load and check input file content
    input_file = open(package_content, "r")
    input_content = json.loads(input_file.read())

    assert "package-name" in input_content
    assert "package-uid" in input_content

    #   Add plugin list and libs to input content file
    input_content["plugins"] = package_plugins

    if common_libs is not None and len(common_libs) > 0:
        input_content["common-libs"] = common_libs

    #   Write the result to output file
    with open(output, "w+") as output_file:
        output_file.write(
            json.dumps(input_content, sort_keys=True, indent=4))

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("-o", "--output", help="package json source file", nargs=1, required=True)
    parser.add_argument("--package-content", help="package json source file", nargs=1, required=True)
    parser.add_argument("--plugins-content", help="plusing json sources files", nargs='*', required=False)
    parser.add_argument("--common-libs", help="additional bytecode module", nargs='*', required=False)
    args = parser.parse_args()

    generate_content_file(
        args.output[0],
        args.package_content[0],
        args.plugins_content,
        args.common_libs)