#!/usr/bin/env python3

import os


print("Compiling...")
os.system("make clean && make")

all_correct = True
for root, dirs, files in os.walk("test-data/"):
    for file in files:
        if file.startswith("input"):
            in_path = os.path.join(os.path.abspath(root), file)
            out_path = os.path.join(os.path.abspath(root), "output" + file[5:])

            cmd = "./vertexcol -f " + in_path + " -o " + out_path
            print(file[5:])

            os.system(cmd)

            with open(out_path) as f:
                line = f.readline()
                if line.startswith("Solution verified"):
                    print(line[:-1])
                else:
                    all_correct = False
                    print("Wrong solution.\n\n")

if all_correct:
    print("\nAll correct!")
