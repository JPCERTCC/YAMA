#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
import argparse
from typing import List


def KSA(key:List[int]):
    keylen = len(key)
    S = [i for i in range(256)]
    j = 0
    for i in range(256):
        j = (j+S[i]+ key[i%keylen]) %256
        S[i],S[j] = S[j],S[i]
    return S


def PRGA(S: List[int]):
    i,j = 0,0
    while True:
        i = (i+1) %256
        j = (j+S[i]) % 256
        S[i],S[j] = S[j],S[i]
        K = S[(S[i]+S[j])%256]
        yield K

def ARC4(key:str):
    S = KSA(key)
    return PRGA(S)


def main():
    prs = argparse.ArgumentParser()
    prs.add_argument("-f", "--file", type=str,required=True)
    prs.add_argument("-k", "--key", type=str, default="YetAnotherMemoryAnalyzer")
    args = prs.parse_args()

    data = None
    with open(args.file, "rb") as f:
        data = f.read()
    
    key = [ord(c) for c in args.key]
    keystream = ARC4(key)

    enc = []
    for c in data:
        e = c ^ keystream.__next__()
        enc.append(e)

    with open(args.file + ".enc", "wb") as f:
        f.write(bytearray(enc))
        sys.stdout.write(f"[+] encode with ARC4: {args.file}\n")

    return

if __name__ == "__main__":
    main()
