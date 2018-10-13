#!/usr/bin/env python3

import sys

from argparse import ArgumentParser
from pathlib import Path
from subprocess import call


parser = ArgumentParser()
parser.add_argument('--debug', action='store_true')
parser.add_argument('--release', action='store_true')
args = parser.parse_args()

if not len(sys.argv) > 1:
    args.debug = True
    args.release = True

proj_dir = Path.cwd()
build_dir = proj_dir / 'build'
debug_dir = build_dir / 'debug'
release_dir = build_dir / 'release'

for dir in [build_dir, debug_dir, release_dir]:
    if not dir.exists():
        dir.mkdir()

cmake_debug_flag = '-DCMAKE_BUILD_TYPE=Debug'
cmake_release_flag= '-DCMAKE_BUILD_TYPE=Release'

for target, target_dir, cmake_flag in [(args.debug, debug_dir, cmake_debug_flag),
                                       (args.release, release_dir, cmake_release_flag)]:
    if not target:
        continue

    print('Building {}'.format(target_dir))
    if not (target_dir / 'CMakeCache.txt').exists():
        call(['cmake', proj_dir.resolve(), cmake_flag], cwd=target_dir)

    call(['make', '-j'], cwd=target_dir)
    call(['make', 'test', '-j'], cwd=target_dir)
