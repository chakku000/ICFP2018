from typing import *
from collections import deque

# helper

def pos_to_region(c: List[int]):
    return tuple(c*2)

def pos2_to_region(c0: List[int], c1: List[int]) -> Tuple[int]:
    x0, y0, z0 = c0
    x1, y1, z1 = c1
    if not x0 <= x1:
        x0, x1 = x1, x0
    if not y0 <= y1:
        y0, y1 = y1, y0
    if not z0 <= z1:
        z0, z1 = z1, z0
    return tuple([x0, y0, z0, x1, y1, z1])

def pos_add(c: List[int], dc: List[int]):
    x0, y0, z0 = c
    dx, dy, dz = dc
    return tuple([x0+dx, y0+dy, z0+dz])

def colli(r0: List[int], r1: List[int]) -> bool:
    x0, y0, z0, x1, y1, z1 = r0
    x2, y2, z2, x3, y3, z3 = r1
    return max(x0, x2) <= min(x1, x3) and max(y0, y2) <= min(y1, y3) and max(z0, z2) <= min(z1, z3)

def containFill(r: List[int], M: List[List[List[int]]]) -> int:
    x0, y0, z0, x1, y1, z1 = r
    for x in range(x0, x1+1):
        for y in range(y0, y1+1):
            for z in range(z0, z1+1):
                if M[x][y][z]:
                    return 1
    return 0

dd = ((1, 0, 0), (-1, 0, 0), (0, 1, 0), (0, -1, 0), (0, 0, 1), (0, 0, -1))
def checkGrounded(R: int, M: List[List[List[int]]]) -> bool:
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
                    print(i, j, k, M[i])
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

# ===== Nanobot =====

class Nanobot:
    def __init__(self, i: int, c: List[int]):
        self.id = i
        self.c = list(c)
        self.s = []

    def add_c(self, c: List[int]) -> None:
        x, y, z = c
        x0, y0, z0 = self.c
        self.c = [x+x0, y+y0, z+z0]

    def get_i(self) -> int:
        return self.id

    def set_c(self, c: List[int]) -> None:
        self.c = c

    def get_c(self) -> Tuple[int]:
        return tuple(self.c)

    def set_s(self, s: List[int]) -> None:
        self.seeds = s

    def get_s(self) -> List[int]:
        return list(self.seeds)

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
        assert 0

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

    def fusion(self) -> Tuple[int]:
        return None

    def is_primary_fusion(self) -> bool:
        return True

# Halt: 停止命令
class Halt(Cmd):
    def update(self, b: Nanobot) -> None:
        self.pos = b.get_c()

    def is_halt(self) -> bool:
        return True

    def volatile(self) -> List[List[int]]:
        return [pos_to_region(self.pos)]

# Wait: 待機命令
class Wait(Cmd):
    def update(self, b: Nanobot) -> None:
        self.pos = b.get_c()

    def volatile(self) -> List[List[int]]:
        return [pos_to_region(self.pos)]

# Flip: harmonicsの反転処理
class Flip(Cmd):
    def update(self, b: Nanobot) -> None:
        self.pos = b.get_c()

    def volatile(self) -> List[List[int]]:
        return [pos_to_region(self.pos)]

    def is_flip(self) -> bool:
        return True

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
        return (m, self.pos1)

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

    def fill(self):
        return self.pos1

# FusionP (Fusion Primary): Fusionの代表者
class FusionP(Cmd):
    def __init__(self, nd: List[int]):
        super().__init__()
        self.nd = nd
        assert check_ND(nd), nd

    def update(self, b: Nanobot) -> None:
        self.pos = b.get_c()
        self.pos1 = pos_add(self.pos)

    def volatile(self) -> List[List[int]]:
        return [pos_to_region(self.pos)]

    def fusion(self) -> List[int]:
        return self.pos1

    def is_primary_fusion(self) -> bool:
        return True

# FusionS (Fusion Secondary): Fusionの子供
class FusionS(Cmd):
    def __init__(self, nd: List[int]):
        super().__init__()
        self.nd = nd
        assert check_ND(nd), nd

    def update(self, b: Nanobot) -> None:
        self.pos = b.get_c()
        self.pos1 = pos_add(self.pos)

    def volatile(self) -> List[List[int]]:
        return [pos_to_region(self.pos)]

    def fusion(self) -> List[int]:
        return self.pos1

    def is_primary_fusion(self) -> bool:
        return False

class State:
    def __init__(self, R: int, targetM: List[List[List[int]]]):
        self.R = R
        self.enegy = 0
        self.harmonics = 0
        self.matrix = [[[0]*R for i in range(R)] for j in range(R)]
        self.target_matrix = targetM
        bot = Nanobot(1, (0, 0, 0))
        bot.set_s(tuple(range(2, 21)))
        self.bots = [bot]
        self.traces = []
        self.N = 1
        self.exit = 0

    def get_n(self) -> int:
        return self.N

    def is_exit(self) -> bool:
        return bool(self.exit)

    def set(self, cmds: List[Cmd]) -> None:
        assert not self.exit, "input commands after halt"
        assert self.N == len(cmds)

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
                return
            bot = bots[i]
            cmd.update(bot)
            rs.append(cmd.volatile())
            if cmd.is_flip():
                flip = True

        # region衝突チェック
        L = len(rs)
        for i in range(L):
            rs1 = rs[i]
            for j in range(i+1, L):
                rs2 = rs[j]
                for r1 in rs1:
                    for r2 in rs2:
                        assert not colli(r1, r2)
            for r1 in rs1:
                assert not containFill(r1, M)

        # flip harmonics
        if flip:
            self.harmonics ^= 1

        # low-harmonicsのチェック
        if self.harmonics == 0:
            assert checkGrounded(R, M)

        tM = self.target_matrix
        d = set()
        for i, cmd in enumerate(cmds):
            bot = bots[i]
            f = cmd.fill()
            if f:
                x, y, z = f
                assert M[x][y][z] == 0, (x, y, z)
                assert tM[x][y][z] == 1, (x, y, z)
                M[x][y][z] = 1
            g = cmd.generate()
            if g:
                m, c = g
                seeds = bot.get_s()
                assert 0 < len(seeds) and m+1 <= len(seeds)
                bot.set_s(seeds[m+1:])
                g_bot = Nanobot(seeds[0], c)
                g_bot.set_s(seeds[1:m+1])
                bots.append(g_bot)
            f = cmd.fusion()
            if f:
                j = b_map[f]
                assert cmds[j].is_fusion()
                assert cmd.is_primary_fusion() ^ cmds[j].is_primary_fusion()
                if cmd.is_primary_fusion():
                    seeds = bot.get_s() + bots[j].get_s() + (bots[j].get_i(),)
                else:
                    d.add(i)
        self.bots = [bots[i] for i in range(self.N) if i not in d]

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
