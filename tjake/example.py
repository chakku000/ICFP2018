from base import *
from file_io import *

import sys
argv = sys.argv
argc = len(argv)
if argc < 2:
    print("ｵﾌﾄｩﾝ")
    exit(0)


filename = argv[1]
cnt = 0
with open(filename, 'rb') as f:
    R, M = mdl_reader(f)
    for i in range(R):
        print(*M[i], sep='\n')
        print()


