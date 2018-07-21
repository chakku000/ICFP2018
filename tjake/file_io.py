# バイナリファイルから1bitずつ読みだす
def bit_generator(f):
    *r8, = range(8)

    while 1:
        b = f.read(1)
        if not b:
            break
        v = b[0]
        for i in r8:
            yield (v >> i) & 1

def byte_generator(f):
    *r8, = range(8)

    while 1:
        b = f.read(1)
        if not b:
            break
        yield b[0]

def mdl_reader(f):
    g = bit_generator(f)
    R = 0
    for i in range(8):
        if next(g):
            R |= 1 << i
    M = [[[0]*R for i in range(R)] for j in range(R)]
    for i in range(R**3):
        v = next(g)
        M[i//R//R][(i//R)%R][i%R] = v
    return R, M

