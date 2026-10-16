#!/usr/bin/env python3
from pycparser import c_parser, c_ast, c_generator
from pathlib import Path
from typing import Dict
import urllib.request
import urllib.parse
import argparse
import m2ctx
import json
import sys
import os

def generate_context(file: str) -> None:
    source = m2ctx.import_c_file(file)
    lines = source.splitlines()
    macro = ""
    source = ""
    for line in lines:
        if "GLOBAL_ASM" in line:
            continue
        if line.startswith('#define'):
            macro += line + '\n'
        else:
            source += line + '\n'

    parser = c_parser.CParser()
    try:
        ast = parser.parse(source, filename='<stdin>')
    except:
        e = sys.exc_info()[1]
        print("Parse error:" + str(e) + ' from ' + file)
        sys.exit()

    #prune function def
    new_ast = c_ast.FileAST([])
    for i, node in enumerate(ast.ext):
        if isinstance(node, c_ast.FuncDef):
            continue
        elif isinstance(node, c_ast.Decl) and isinstance(node.type, c_ast.FuncDecl):
            if "static" in node.storage:
                node.storage.remove("static")
        new_ast.ext.append(node)

    return macro + c_generator.CGenerator().visit(new_ast)

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("c_file")
    parser.add_argument('-d', '--dump', action='store_true', help='Dump context')
    args = parser.parse_args()

    api_base = os.environ.get("DECOMPME_API_BASE", "https://decomp.me")

    path = list(Path('asm/nonmatchings/').rglob(args.c_file + '*'))
    
    if len(path) == 0:
        print('Cannot find function ' + args.c_file)
        sys.exit(1)
    
    assert(len(path) == 1)
    path = path[0]
    print('Found function at ' + str(path))
    
    src_file = Path(*path.parts[2:-1])
    src_file = src_file.with_suffix('.c')
    
    print("Generating context...")
    context = generate_context(src_file)

    if args.dump:
        with open("ctx.c", 'w') as f:
            f.write(context)
        sys.exit(1)
    
    with open(path, 'r') as f:
        asm_cont = f.read()

    func_name = os.path.basename(path).split('.')[0]
    print("Uploading...")
    try:
        post_data = urllib.parse.urlencode(
            {
                "name": func_name,
                "target_asm": asm_cont,
                "context": context,
                "source_code": "",
                "compiler": "ido5.3",
                "compiler_flags": "-O2 -mips2 -Olimit 2000 -fullwarn",
                "diff_label": func_name,
            }
        ).encode("ascii")
        with urllib.request.urlopen(f"{api_base}/api/scratch", post_data) as f:
            resp = f.read()
            json_data: Dict[str, str] = json.loads(resp)
            if "slug" in json_data:
                slug = json_data["slug"]
                token = json_data.get("claim_token")
                if token:
                    print(f"{api_base}/scratch/{slug}/claim?token={token}")
                else:
                    print(f"{api_base}/scratch/{slug}")
            else:
                error = json_data.get("error", resp)
                print(f"Server error: {error}")
    except Exception as e:
        print(e)