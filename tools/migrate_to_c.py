#!/usr/bin/env python3

import subprocess
import argparse
import m2ctx

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("c_file")
    args = parser.parse_args()
    asm = args.c_file
    c_file = asm.replace('asm/data/', '', 1).replace('.data.s', '.c')

    # Parse C file
    ctx = m2ctx.import_c_file(c_file)

    # Write m2c context
    with open('data.c', 'w') as f:
        f.write(ctx)

    # Decompile data
    decomp = subprocess.check_output(['tools/m2c/m2c.py', '--force-decimal', '--globals', 'all', '--context', 'data.c', asm], encoding="utf-8")

    # Write decompiled data
    with open('data.c', 'w') as f:
        f.write(decomp)