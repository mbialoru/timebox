#!/usr/bin/python3
""" Scans given directory for pattern-designated files and generates chosen hash
    based on their content and location. By default pattern is '*.py' and hash
    is MD5. """

from enum import Enum, unique
from pathlib import Path, PosixPath
from typing import Union
import datetime as dt
import argparse
import hashlib
import sys
import os

__version__ = "1.3.1"
__author__ = "Marek 'Saligia' Białoruski"

argv = argparse.ArgumentParser()
argv.add_argument("path", metavar="PATH", type=str,
                    help="Path to scan", nargs="?", default=".")
argv.add_argument("pattern", metavar="PATTERN", type=str,
                    help="Pattern for scanning", nargs="?", default="*.py")
argv.add_argument("-a", "--algorithm", default="md5", choices=["md5", "sha1", "sha256", "sha512"],
                    help="Hashing algorithm")
argv.add_argument("-v", "--verbose", action="count", default=0,
                    help="Verbose output - multiply for more output")
argv.add_argument("-c", "--color", action="store_true", default=False,
                    help="Colored output in terminal")
argv = argv.parse_args()


def printc(*args):
    """ Print colored text in console """
    @unique
    class Colors(Enum):
        PRPL = "\033[95m"
        BLUE = "\033[94m"
        CYAN = "\033[96m"
        GREN = "\033[92m"
        WARN = "\033[93m"
        FAIL = "\033[91m"
        REST = "\033[0m"
        BOLD = "\033[1m"
        UNDR = "\033[4m"

    if len(args) % 2:
        raise TypeError(f"printc takes 2n arguments {len(args)} given")

    res, it = '', iter(args)
    for i in it:
        msg, color = i, next(it)
        try:
            res += f"{Colors[color].value}{msg}"
        except KeyError:
            res += f"{Colors.REST.value}{msg}"
    res += f"{Colors.REST.value}"
    return print(res)


def FileDetails(argv: argparse.Namespace, file: PosixPath):
    """ Verbosity dependent file details printing logic """
    if argv.verbose == 1:
        if argv.color:
            printc(f"{file}", "PRPL")
        else:
            print(f"{file}")
    elif argv.verbose == 2:
        mTime = str(dt.datetime.fromtimestamp(file.stat().st_mtime))[:-7]
        if argv.color:
            printc(f"{file}", "PRPL",f" Modified: {mTime}", "GREN")
        else:
            print(f"{file} Modified: {mTime}")
    elif argv.verbose == 3:
        relPath = os.path.relpath(file)
        mTime = str(dt.datetime.fromtimestamp(file.stat().st_mtime))[:-7]
        if argv.color:
            printc(f"{relPath}", "PRPL", f" Modified: {mTime}", "GREN")
        else:
            print(f"{relPath} Modified: {mTime}")
    elif argv.verbose == 4:
        absPath = os.path.abspath(file)
        mTime = str(dt.datetime.fromtimestamp(file.stat().st_mtime))[:-7]
        if argv.color:
            printc(f"{absPath}", "PRPL", f" Modified: {mTime}", "GREN")
        else:
            print(f"{absPath} Modified: {mTime}")
    elif argv.verbose >= 5:
        absPath = os.path.abspath(file)
        mTime = str(dt.datetime.fromtimestamp(file.stat().st_mtime))[:-7]
        cTime = str(dt.datetime.fromtimestamp(file.stat().st_ctime))[:-7]
        if argv.color:
            printc(f"{absPath}", "PRPL", f" Modified: {mTime}", "GREN", f" Created: {cTime}", "BLUE")
        else:
            print(f"{absPath} Modified: {mTime} Created: {cTime}")


def InitHash(hashType: str):
    if hashType == "md5":
        return hashlib.md5()
    elif hashType == "sha1":
        return hashlib.sha1()
    elif hashType == "sha256":
        return hashlib.sha256()
    elif hashType == "sha512":
        return hashlib.sha512()


def main(argv: argparse.Namespace) -> Union[str, bool, None]:
    fHash = InitHash(argv.algorithm)
    for file in Path(argv.path).rglob(argv.pattern):
        FileDetails(argv, file)
        with open(file, "rb") as handle:
            while chunk := handle.read(8192):
                fHash.update(chunk)

    if len(list(Path(argv.path).rglob(argv.pattern))) == 0:
        if argv.color:
            printc("", "BOLD", "\nNo files found!\n", "FAIL")
        else:
            print("\nNo files found!\n")
        return False

    if argv.verbose > 0:
        if argv.color:
            printc(f"\n{str(argv.algorithm).upper()}: ", "BOLD",
                   f"{fHash.hexdigest()}\n", "CYAN")
        else:
            print(f"\n{str(argv.algorithm).upper()}: {fHash.hexdigest()}\n")
    else:
        if argv.color:
            printc(f"{str(argv.algorithm).upper()}: ", "BOLD",
                   f"{fHash.hexdigest()}", "CYAN")
        else:
            return f"{str(argv.algorithm).upper()}: {fHash.hexdigest()}"


if __name__ == "__main__":
    sys.exit(main(argv) or 0)
