#!/usr/bin/python3

import argparse
import json
import os

def unixPath(p):
    # Always write unix style path as they are understood in most OS
    return p.replace(os.path.sep, '/')

def generate_content_file(output, package_content, plugin_contents, common_libs):
    #   Build plugin list
    package_plugins = []
    for p in plugin_contents:
        with open(p, "r") as f:
            plugin = json.loads(f.read())
            plugin_src_dir = os.path.dirname(p)
            plugin_rel_dir_name = os.path.basename(plugin_src_dir)

            for i in range(len(plugin["modules"])):
                module_rel_path = os.path.relpath(plugin["modules"][i], plugin_src_dir)
                plugin["modules"][i] = unixPath(os.path.join(plugin_rel_dir_name, module_rel_path))
            package_plugins.append(plugin)

    #   Load and check input file content
    input_file = open(package_content, "r")
    input_content = json.loads(input_file.read())

    assert "package-name" in input_content
    assert "package-uid" in input_content

    #   Add plugin list and libs to input content file
    input_content["plugins"] = package_plugins

    if common_libs is not None and len(common_libs) > 0:
        output_dir = os.path.dirname(output)

        for i in range(len(common_libs)):
            common_libs[i] = unixPath(os.path.relpath(common_libs[i], output_dir))

        input_content["common-libs"] = common_libs

    #   Write the result to output file
    with open(output, "w+") as output_file:
        output_file.write(
            json.dumps(input_content, sort_keys=True, indent=4))

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Generate a package content file with given plugins and common lib modules")
    parser.add_argument("-o", "--output", help="package json output file", nargs=1, required=True)
    parser.add_argument("--package-content", help="package json source file", nargs=1, required=True)
    parser.add_argument("--plugins-content", help="plugins json sources files", nargs='*', required=False)
    parser.add_argument("--common-libs", help="additional bytecode module", nargs='*', required=False)
    args = parser.parse_args()

    generate_content_file(
        args.output[0],
        args.package_content[0],
        args.plugins_content,
        args.common_libs)