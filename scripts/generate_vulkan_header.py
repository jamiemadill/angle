#!/usr/bin/python
#
# Copyright 2016 The ANGLE Project Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
#
# generate_vulkan_header.py:
#   Workaround problems with Windows and GYP and the Vulkan loader paths.

import os, sys

if len(sys.argv) < 4:
    print("Usage: " + sys.argv[0] + " <product_dir> <layers_source_path) <output_file>")
    sys.exit(1)

product_dir = sys.argv[1]
layers_source_path = sys.argv[2]
output_file = sys.argv[3]

def p(inpath):
    return inpath.replace('\\', '/')

if not os.path.isdir(product_dir):
    print(source_dir + " is not a directory.")
    sys.exit(1)

with open(output_file, "w") as outfile:
    outfile.write("#define DEFAULT_VK_LAYERS_PATH \"" + p(product_dir) + "\"\n")
    outfile.write("#define LAYERS_SOURCE_PATH \"" + p(layers_source_path) + "\"\n")
