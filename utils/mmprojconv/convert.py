import argparse
import os
from pathlib import Path
import logging
import traceback

from utils.mmprojconv.converter.Converter import Converter

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Converts a given xml-based mmprj MegaMol project file to the new lua-based definition.")
    parser.add_argument("input", help=".mmprj file to convert")
    parser.add_argument("-v", "--verbosity", action="count", default=0)
    args = parser.parse_args()

    logging.basicConfig()

    if args.verbosity == 1:
        logging.getLogger().setLevel(logging.INFO)
    elif args.verbosity > 1:
        logging.getLogger().setLevel(logging.DEBUG)

    files = []
    if Path(args.input).is_dir():
        for f in os.listdir(args.input):
            if f.endswith('.mmprj'):
                files.append(os.path.join(args.input, f))
    elif Path(args.input).is_file():
        files = [args.input]

    logging.debug(f"Converting {len(files)} files.")

    for file in files:
        logging.info(f"Parsing file {os.path.abspath(file)}.")
        try:
            c = Converter(file)
            with open(file.replace(".mmprj", ".lua"), "w", encoding='utf-8') as the_file:
                logging.info(f"Saving converted project to {os.path.abspath(file.replace('.mmprj', '.lua'))}")
                print(f"-- converted from {os.path.abspath(file)}", file=the_file)
                [print(l, file=the_file) for l in c.to_lua()]
        except Exception as e:
            logging.error(f"Could not convert: {os.path.abspath(file)}")
            traceback.print_exc()
            continue
