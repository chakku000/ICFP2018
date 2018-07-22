from typing import *
from collections import deque

# helper

def pos_to_region(c: List[int]):
    return tuple(c*2) + (0,)

def pos2_to_region(c0: List[int], c1: List[int]) -> Tuple[int]:
    x0, y0, z0 = c0
    x1, y1, z1 = c1
    if not x0 <= x1:
        x0, x1 = x1, x0
    if not y0 <= y1:
        y0, y1 = y1, y0
    if not z0 <= z1:
        z0, z1 = z1, z0
    return tuple([x0, y0, z0, x1, y1, z1, 0])

def pos2_to_group_region(c0: List[int], c1: List[int]) -> Tuple[int]:
    x0, y0, z0 = c0
    x1, y1, z1 = c1
    if not x0 <= x1:
        x0, x1 = x1, x0
    if not y0 <= y1:
        y0, y1 = y1, y0
    if not z0 <= z1:
        z0, z1 = z1, z0
    return tuple([x0, y0, z0, x1, y1, z1, 1])

def pos_add(c: List[int], dc: List[int]):
    x0, y0, z0 = c
    dx, dy, dz = dc
    return tuple([x0+dx, y0+dy, z0+dz])

def colli(r0: List[int], r1: List[int]) -> bool:
    x0, y0, z0, x1, y1, z1, t0 = r0
    x2, y2, z2, x3, y3, z3, t1 = r1
    if t0 == t1 and r0 == r1:
        # GFill, GVoidのための特別判定
        return False
    return max(x0, x2) <= min(x1, x3) and max(y0, y2) <= min(y1, y3) and max(z0, z2) <= min(z1, z3)

def containFill(r: List[int], M: List[List[List[int]]]) -> int:
    x0, y0, z0, x1, y1, z1, t = r
    for x in range(x0, x1+1):
        for y in range(y0, y1+1):
            for z in range(z0, z1+1):
                if M[x][y][z]:
                    return 1
    return 0

dd = ((1, 0, 0), (-1, 0, 0), (0, 1, 0), (0, -1, 0), (0, 0, 1), (0, 0, -1))
def check_grounded(R: int, M: List[List[List[int]]]) -> bool:
    que = deque()
    used = set()
    for i in range(R):
        for j in range(R):
            if M[i][0][j]:
                que.append((i, 0, j))
                used.add((i, 0, j))
    while que:
        x, y, z = que.popleft()
        for dx, dy, dz in dd:
            nx = x + dx; ny = y + dy; nz = z + dz
            np = (nx, ny, nz)
            if not 0 <= nx < R or not 0 <= ny < R or not 0 <= nz < R or M[nx][ny][nz] == 0 or np in used:
                continue
            que.append(np)
            used.add(np)
    for i in range(R):
        for j in range(R):
            for k in range(R):
                if M[i][j][k] and (i, j, k) not in used:
                    #print(i, j, k, M[i])
                    return 0
    return 1

def check_LD(ld: List[int]) -> bool:
    x, y, z = ld
    xd = abs(x) == 0; yd = abs(y) == 0; zd = abs(z) == 0
    return xd^yd^zd == 0 and xd|yd|zd == 1

def check_SLD(sld: List[int]) -> bool:
    x, y, z = sld
    return check_LD(sld) and abs(x) + abs(y) + abs(z) <= 5

def check_LLD(lld: List[int]) -> bool:
    x, y, z = lld
    return check_LD(lld) and abs(x) + abs(y) + abs(z) <= 15

def check_ND(nd: List[int]) -> bool:
    ax, ay, az = map(abs, nd)
    return 0 < ax + ay + az <= 2 and max(ax, ay, az) == 1

def check_FD(fd: List[int]) -> bool:
    ax, ay, az = map(abs, fd)
    return 0 < max(ax, ay, az) <= 30

# ===== Nanobot =====

class Nanobot:
    def __init__(self, c: List[int], s: int):
        self.c = list(c)
        self.s = s

    def add_c(self, c: List[int]) -> None:
        x, y, z = c
        x0, y0, z0 = self.c
        self.c = [x+x0, y+y0, z+z0]

    def set_c(self, c: List[int]) -> None:
        self.c = c

    def get_c(self) -> Tuple[int]:
        return tuple(self.c)

    def get_s(self) -> int:
        return self.s

    def set_s(self, s: int) -> None:
        self.s = s

# ===== 各コマンド =====

# Cmd: baseクラス
class Cmd:
    def __init__(self):
        self.__updated = 0

    def __set_updated(self) -> None:
        self.__updated = 1

    def __check_updated(self) -> bool:
        return self.__updated

    def update(self, b: Nanobot) -> None:
        # 各Nanobotに対する更新処理
        assert 0, "update() is not implemented"

    def is_halt(self) -> bool:
        # 停止させるか (Haltの時だけTrue)
        return False

    def is_flip(self) -> bool:
        # 反転させるか (Flipの時だけTrue)
        return False

    def volatile(self) -> List[List[int]]:
        # volatile -> 範囲のlistで返す
        # [(x0, y0, z0, x1, y1, z1), ...]
        return []

    def assertion(self, s) -> bool:
        return True

    def generate(self) -> Tuple[int, List[int]]:
        return None

    def fill(self) -> Tuple[int]:
        return None

    def void(self) -> Tuple[int]:
        return None

    def fusion(self) -> Tuple[int]:
        return None

    def is_primary_fusion(self) -> bool:
        return True

    def calc_cost(self) -> int:
        assert 0, "calc_cost() is not implemented"
        return 0

    def output(self) -> str:
        assert 0, "output() is not implemented"
        return ""

    def __str__(self) -> str:
        return self.output()

# Halt: 停止命令
class Halt(Cmd):
    def update(self, b: Nanobot) -> None:
        self.pos = b.get_c()

    def is_halt(self) -> bool:
        return True

    def volatile(self) -> List[List[int]]:
        return [pos_to_region(self.pos)]

    def calc_cost(self):
        return 0

    def output(self) -> str:
        return "Halt"

# Wait: 待機命令
class Wait(Cmd):
    def update(self, b: Nanobot) -> None:
        self.pos = b.get_c()

    def volatile(self) -> List[List[int]]:
        return [pos_to_region(self.pos)]

    def calc_cost(self):
        return 0

    def output(self) -> str:
        return "Wait"

# Flip: harmonicsの反転処理
class Flip(Cmd):
    def update(self, b: Nanobot) -> None:
        self.pos = b.get_c()

    def volatile(self) -> List[List[int]]:
        return [pos_to_region(self.pos)]

    def is_flip(self) -> bool:
        return True

    def calc_cost(self):
        return 0

    def output(self) -> str:
        return "Flip"

# SMove: 1回の移動
class SMove(Cmd):
    def __init__(self, lld: List[int]):
        super().__init__()
        self.lld = lld
        assert check_LLD(lld), lld

    def update(self, b: Nanobot) -> None:
        self.pos = b.get_c()
        b.add_c(self.lld)
        self.pos1 = b.get_c()

    def volatile(self) -> List[List[int]]:
        return [pos2_to_region(self.pos, self.pos1)]

    def calc_cost(self):
        x, y, z = self.lld
        return 2*(abs(x) + abs(y) + abs(z))

    def output(self) -> str:
        return "SMove %d %d %d" % self.lld

# LMove: 2回の移動
class LMove(Cmd):
    def __init__(self, sld1: List[int], sld2: List[int]):
        super().__init__()
        self.sld1 = sld1
        self.sld2 = sld2
        assert check_SLD(sld1), sld1
        assert check_SLD(sld2), sld2

    def update(self, b: Nanobot) -> None:
        self.pos = b.get_c()
        b.add_c(self.sld1)
        self.pos1 = b.get_c()
        b.add_c(self.sld2)
        self.pos2 = b.get_c()

    def volatile(self) -> List[List[int]]:
        return [pos2_to_region(self.pos, self.pos1), pos2_to_region(self.pos1, self.pos2)]

    def calc_cost(self):
        x0, y0, z0 = self.sld1
        x1, y1, z1 = self.sld2
        return 2*(abs(x0) + abs(y0) + abs(z0) + 2 + abs(x1) +abs(y1) + abs(z1))

    def output(self) -> str:
        return "LMove %d %d %d %d %d %d" % (*self.sld1, *self.sld2)

# Fission: 分裂
class Fission(Cmd):
    def __init__(self, nd: List[int], m: int):
        super().__init__()
        self.nd = nd
        self.m = m
        assert check_ND(nd), nd

    def update(self, b: Nanobot) -> None:
        self.pos = b.get_c()
        self.pos1 = pos_add(self.pos, self.nd)

    def volatile(self) -> List[List[int]]:
        return [pos_to_region(self.pos), pos_to_region(self.pos1)]

    def generate(self) -> Tuple[int, List[int]]:
        return (self.m, self.pos1)

    def calc_cost(self):
        return 24

    def output(self) -> str:
        return "Fission %d %d %d %d" % (*self.nd, self.m)

# Fill: 埋める
class Fill(Cmd):
    def __init__(self, nd: List[int]):
        super().__init__()
        self.nd = nd
        assert check_ND(nd), nd

    def update(self, b: Nanobot) -> None:
        self.pos = b.get_c()
        self.pos1 = pos_add(self.pos, self.nd)

    def volatile(self) -> List[List[int]]:
        return [pos_to_region(self.pos), pos_to_region(self.pos1)]

    def fill(self) -> Tuple[int]:
        return pos_to_region(self.pos1)

    def calc_cost(self):
        return 0

    def output(self) -> str:
        return "Fill %d %d %d" % self.nd

# Void: 消す
class Void(Cmd):
    def __init__(self, nd: List[int]):
        super().__init__()
        self.nd = nd
        assert check_ND(nd), nd

    def update(self, b: Nanobot) -> None:
        self.pos = b.get_c()
        self.pos1 = pos_add(self.pos, self.nd)

    def volatile(self) -> List[List[int]]:
        return [pos_to_region(self.pos), pos_to_region(self.pos1)]

    def void(self) -> Tuple[int]:
        return pos_to_region(self.pos1)

    def calc_cost(self):
        return 0

    def output(self) -> str:
        return "Void %d %d %d" % self.nd

# FusionP (Fusion Primary): Fusionの代表者
class FusionP(Cmd):
    def __init__(self, nd: List[int]):
        super().__init__()
        self.nd = nd
        assert check_ND(nd), nd

    def update(self, b: Nanobot) -> None:
        self.pos = b.get_c()
        self.pos1 = pos_add(self.pos, self.nd)

    def volatile(self) -> List[List[int]]:
        return [pos_to_region(self.pos)]

    def fusion(self) -> List[int]:
        return self.pos1

    def is_primary_fusion(self) -> bool:
        return True

    def calc_cost(self):
        return -24

    def output(self) -> str:
        return "FusionP %d %d %d" % self.nd

# FusionS (Fusion Secondary): Fusionの子供
class FusionS(Cmd):
    def __init__(self, nd: List[int]):
        super().__init__()
        self.nd = nd
        assert check_ND(nd), nd

    def update(self, b: Nanobot) -> None:
        self.pos = b.get_c()
        self.pos1 = pos_add(self.pos, self.nd)

    def volatile(self) -> List[List[int]]:
        return [pos_to_region(self.pos)]

    def fusion(self) -> List[int]:
        return self.pos1

    def is_primary_fusion(self) -> bool:
        return False

    def calc_cost(self):
        # FusionPでコストが計算される
        return 0

    def output(self):
        return "FusionS %d %d %d" % self.nd

class GFill(Cmd):
    def __init__(self, nd: List[int], fd: List[int]):
        super().__init__()
        self.nd = nd
        self.fd = fd
        assert check_ND(nd), nd
        assert check_FD(fd), fd

    def update(self, b: Nanobot) -> None:
        self.pos = b.get_c()
        self.pos1 = pos_add(self.pos, self.nd)
        self.pos2 = pos_add(self.pos1, self.fd)

    def volatile(self) -> List[List[int]]:
        return [pos_to_region(self.pos), pos2_to_group_region(self.pos1, self.pos2)]

    def fill(self) -> Tuple[int]:
        return pos2_to_region(self.pos1, self.pos2)

    def calc_cost(self) -> int:
        return 0

    def output(self) -> str:
        return "GFill %d %d %d %d %d %d" % (*self.nd, *self.fd)

class GVoid(Cmd):
    def __init__(self, nd: List[int], fd: List[int]):
        super().__init__()
        self.nd = nd
        self.fd = fd
        assert check_ND(nd), nd
        assert check_FD(fd), fd

    def update(self, b: Nanobot) -> None:
        self.pos = b.get_c()
        self.pos1 = pos_add(self.pos, self.nd)
        self.pos2 = pos_add(self.pos1, self.fd)

    def volatile(self) -> List[List[int]]:
        return [pos_to_region(self.pos), pos2_to_group_region(self.pos1, self.pos2)]

    def void(self) -> Tuple[int]:
        return pos2_to_region(self.pos1, self.pos2)

    def calc_cost(self) -> int:
        return 0

    def outut(self):
        return "GVoid %d %d %d %d %d %d" % (*self.nd, *self.fd)

class State:
    def __init__(self, R: int, targetM: List[List[List[int]]]):
        self.R = R
        self.energy = 0
        self.harmonics = 0
        self.matrix = [[[0]*R for i in range(R)] for j in range(R)]
        self.target_matrix = targetM
        bot = Nanobot((0, 0, 0), 39)
        self.bots = [bot]
        self.traces = []
        self.N = 1
        self.exit = 0

    def get_n(self) -> int:
        return self.N

    def get_energy(self) -> int:
        # 最終ステップのコストを含んでいないため、それを含めたものを返す
        return self.energy + 3*self.R**3 + 20

    def is_exit(self) -> bool:
        return bool(self.exit)

    def set(self, cmds: List[Cmd]) -> None:
        assert not self.exit, "input commands after halt"
        assert self.N == len(cmds), (self.N, len(cmds))

        R = self.R
        M = self.matrix

        bots = self.bots
        flip = False
        b_map = {}
        for i, bot in enumerate(bots):
            b_map[bot.get_c()] = i

        rs = []
        for i, cmd in enumerate(cmds):
            # 終了コマンド
            if cmd.is_halt():
                assert self.N == 1, "halt when N = %d" % self.N
                assert self.harmonics == 0, "halt when harmonics is High"
                bot = self.bots[0]
                assert bot.get_c() == (0, 0, 0), "halt when bots[0] = %s" % bot.get_c()
                self.exit = 1
                self.command_output(cmds)
                return
            bot = bots[i]
            cmd.update(bot)
            rs.append(cmd.volatile())
            if cmd.is_flip():
                flip = True

            self.energy += cmd.calc_cost()
        self.energy += 20 * self.N

        if self.harmonics:
            self.energy += 30 * R**3
        else:
            self.energy += 3 * R**3

        # region衝突チェック
        L = len(rs)
        for i in range(L):
            rs1 = rs[i]
            for j in range(i+1, L):
                rs2 = rs[j]
                for r1 in rs1:
                    for r2 in rs2:
                        assert not colli(r1, r2), (r1, r2)
            for r1 in rs1:
                assert not containFill(r1, M), r1

        # flip harmonics
        if flip:
            self.harmonics ^= 1

        # low-harmonicsのチェック
        if self.harmonics == 0:
            assert check_grounded(R, M)

        tM = self.target_matrix
        d = set()
        uf = set(); uv = set()
        for i, cmd in enumerate(cmds):
            bot = bots[i]
            f = cmd.fill()
            if f:
                if f in uf:
                    continue
                uf.add(f)
                x0, y0, z0, x1, y1, z1, t = f
                for x in range(x0, x1+1):
                    for y in range(y0, y1+1):
                        for z in range(z0, z1+1):
                            assert M[x][y][z] == 0, (x, y, z)
                            assert tM[x][y][z] == 1, (x, y, z)
                            if M[x][y][z] == 0:
                                self.energy += 12
                            else:
                                self.energy += 6
                            M[x][y][z] = 1
            v = cmd.void()
            if v:
                if v in uv:
                    continue
                uv.add(v)
                x0, y0, z0, x1, y1, z1, t = v
                for x in range(x0, x1+1):
                    for y in range(y0, y1+1):
                        for z in range(z0, z1+1):
                            assert M[x][y][z] == 1, (x, y, z)
                            #assert tM[x][y][z] == 0, (x, y, z)
                            if M[x][y][z] == 0:
                                self.energy += 3
                            else:
                                self.energy -= 12
                            M[x][y][z] = 0
            g = cmd.generate()
            if g:
                m, c = g
                seeds = bot.get_s()
                assert 0 < seeds and m+1 <= seeds
                bot.set_s(seeds - m - 1)
                g_bot = Nanobot(c, m)
                bots.append(g_bot)
                self.N += 1
            f = cmd.fusion()
            if f:
                j = b_map[f]
                assert cmds[j].fusion() is not None
                assert cmd.is_primary_fusion() ^ cmds[j].is_primary_fusion()
                if cmd.is_primary_fusion():
                    seeds = bot.get_s() + bots[j].get_s() + 1
                    bot.set_s(seeds)
                else:
                    d.add(i)
        self.bots = [bots[i] for i in range(self.N) if i not in d]
        self.N = len(self.bots)

        self.command_output(cmds)

    def command_output(self, cmds: List[Cmd]) -> None:
        print(*cmds, sep='\n')

    def nanobot_debug(self):
        a = []
        for i in range(self.N):
            bot = self.bots[i]
            a.append("%s" % (bot.get_c(),))
        print("< %s >" % " ".join(a))


    def check(self):
        M = self.matrix
        tM = self.target_matrix
        R = self.R
        # まずNanobotが終了しているか
        assert self.exit

        # 終了後のmatrix内の状態が理想の状態と一致するかを確認
        for i in range(R):
            for j in range(R):
                for k in range(R):
                    assert M[i][j][k] == tM[i][j][k], (i, j, k)
