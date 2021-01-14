#!/bin/env python3

import os, sys, subprocess, shlex, collections, hashlib

def get_file_type(pth):
    ft = subprocess.check_output(["file", "-b", shlex.quote(pth)])
    ft = (ft.decode("utf-8").strip()+",").split(",")[0]
    return ft

def dig(pth):
    digger  = hashlib.sha256()
    with open(pth, "rb") as fd:
        while True:
            buff = fd.read(2**16)
            if not buff: break
            digger.update(buff)
    return digger.hexdigest()

def get_dir_stats(dname):
    all_files = collections.defaultdict(list)
    n_files = 0
    n_dirs = 0
    largest_file = ""
    largest_file_size = -1
    total_file_size = 0
    ftypes = collections.defaultdict(int)
    for root, dirs, files in os.walk(dname, followlinks=True):
        for file in files:
            pth = os.path.join(root,file)
            s = os.path.getsize(pth)
            if s > largest_file_size:
                largest_file_size = s
                largest_file = pth
            total_file_size += s
            all_files[dig(pth)].append(pth)
            ft = get_file_type(pth)

            ftypes[ft] += 1
        n_dirs += len(dirs)
        n_files += len(files)

    print(f"Largest file:      {largest_file}")
    print(f"Largest file size: {largest_file_size}")
    print(f"Number of files:   {n_files}")
    print(f"Number of dirs:    {n_dirs}")
    print(f"Total file size:   {total_file_size}")

    ftypes = sorted( ftypes, key=ftypes.get, reverse=True)
    ftypes = ftypes[:5]
    print("Most common file types:")
    for ft in ftypes: print(f" - {ft}")

    all_files = sorted(all_files.items(), key=lambda x: len(x[1]), reverse=True)[:5]
    gcount = 1
    for grp in all_files:
        if(len(grp[1]) < 2): break
        print(f"Duplicate file - group {gcount}")
        gcount += 1
        for f in grp[1]:
            print(f" - {f}")

def main( argv):
    if( len(argv) != 2):
        print(f"Usage: {argv[0]} dirname")
        sys.exit(-1)
    get_dir_stats(argv[1])

if __name__ == "__main__":
    main( sys.argv)
