# (decoded) traceの動きを模擬
from base import *
from file_io import *

import sys
argv = sys.argv
argc = len(argv)
if argc < 3:
    print("ｵﾌﾄｩﾝ")
    exit(0)


filename = argv[1]
cnt = 0
with open(filename, 'rb') as f:
    R, M = mdl_reader(f)

def command_generator(f):
    for line in f:
        yield f.readline().split()

state = State(R, M)
filename = argv[2]
with open(filename, 'r') as f:
    while not state.is_exit():
        cmds = []
        for i in range(state.get_n()):
            cmd, *prms = f.readline().split()
            *prms, = map(int, prms)
            if cmd == "Halt":
                cmds.append(Halt())
            elif cmd == "Fill":
                cmds.append(Fill(prms))
            elif cmd == "SMove":
                cmds.append(SMove(prms))
            elif cmd == "Flip":
                cmds.append(Flip())
            else:
                assert 0, "unknown command %s" % (cmd, prms)
        state.set(cmds)
    print("finish")
state.check()
