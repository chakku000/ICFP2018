# 適宜Highを用いながら、2体で線分を生成していくやつ
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

state = State(R, M)
LD = 15
SD = 5

def single_move(p0, q0, sw):
    cs = []
    x0, y0 = p0; x1, y1 = q0
    if sw:
        while abs(x1 - x0) > 0:
            l = max(min(x1 - x0, LD), -LD)
            cs.append(SMove((l, 0, 0)))
            x0 += l
        while abs(y1 - y0) > 0:
            l = max(min(y1 - y0, LD), -LD)
            cs.append(SMove((0, 0, l)))
            y0 += l
    else:
        while abs(y1 - y0) > 0:
            l = max(min(y1 - y0, LD), -LD)
            cs.append(SMove((0, 0, l)))
            y0 += l
        while abs(x1 - x0) > 0:
            l = max(min(x1 - x0, LD), -LD)
            cs.append(SMove((l, 0, 0)))
            x0 += l
    return cs

def test_collinsion(rs0, rs1):
    for r0 in rs0:
        for r1 in rs1:
            if colli(r0, r1):
                return 1
    return 0

def add_commands_with_blocking(p0, p1, cs0, cs1):
    x0, z0 = p0; x1, z1 = p1
    cs = []
    i0 = i1 = 0
    w = Wait()
    while i0 < len(cs0) and i1 < len(cs1):
        c0 = cs0[i0]; c1 = cs1[i1]
        rs0 = c0.temp_volatile((x0, 0, z0))
        rs1 = c1.temp_volatile((x1, 0, z1))
        dx0, _, dz0 = c0.move_vector()
        dx1, _, dz1 = c1.move_vector()
        ok = 1
        if test_collinsion(rs0, rs1):
            t0 = test_collinsion(rs0, w.temp_volatile((x1, 0, z1)))
            t1 = test_collinsion(w.temp_volatile((x0, 0, z0)), rs1)
            if t0 or t1:
                assert not (t0 and t1), "deadlock!"
                if t1:
                    cs.append((cs0[i0], Wait()))
                    i0 += 1
                    x0 += dx0; z0 += dz0
                else:
                    cs.append((Wait(), cs1[i1]))
                    i1 += 1
                    x1 += dx1; z1 += dz1
            else:
                if len(cs0) - i0 > len(cs1) - i1:
                    cs.append((cs0[i0], Wait()))
                    i0 += 1
                    x0 += dx0; z0 += dz0
                else:
                    cs.append((Wait(), cs1[i1]))
                    i1 += 1
                    x1 += dx1; z1 += dz1
        else:
            cs.append((cs0[i0], cs1[i1]))
            i0 += 1; i1 += 1
            x0 += dx0; z0 += dz0
            x1 += dx1; z1 += dz1
    while i0 < len(cs0):
        cs.append((cs0[i0], Wait()))
        i0 += 1
    while i1 < len(cs1):
        cs.append((Wait(), cs1[i1]))
        i1 += 1
    return cs


xf = zf = yf = 0
#state.set((Flip(),))

for y in range(R):
    # どっちの方向の線分を引いていくかを決定する
    #print(y)

    # z方向
    num0 = 0
    p0 = []
    for x in range(R):
        cnt = 0
        tmp = []
        for z in range(R):
            if M[x][y][z]:
                cnt += 1
                if cnt == 30:
                    tmp.append((x, z-cnt+1, x, z))
                    cnt = 0
                    num0 += 1
            else:
                if cnt > 0:
                    num0 += 1
                    tmp.append((x, z-cnt, x, z-1))
                cnt = 0
        if cnt > 0:
            num0 += 1
            tmp.append((x, R-cnt, x, R-1))
        p0.append(tmp)

    # x方向
    num1 = 0
    p1 = []
    for z in range(R):
        cnt = 0
        tmp = []
        for x in range(R):
            if M[x][y][z]:
                cnt += 1
                if cnt == 30:
                    tmp.append((x-cnt+1, z, x, z))
                    cnt = 0
                    num1 += 1
            else:
                if cnt > 0:
                    num1 += 1
                    tmp.append((x-cnt, z, x-1, z))
                cnt = 0
        if cnt > 0:
            num1 += 1
            tmp.append((R-cnt, z, R-1, z))
        p1.append(tmp)

    # y = y に存在しない => ここで終了
    if num0 == num1 == 0:
        break
    yf = y+1

    mv = None
    if num0 < num1:
        # z方向を採用
        mv = p0
    else:
        # x方向を採用
        mv = p1

    # 場所まで移動
    j = None
    for i in range(R):
        if len(mv[i]) > 0:
            x0, z0, x1, z1 = mv[i][0]
            j = i
            break
    cs = single_move((xf, zf), (x0, z0), 1)
    for c in cs:
        state.set((c,))

    # 分裂
    if num0 < num1:
        state.set((Fission((0, 0, 1), 20),))
        x1, z1 = x0, z0+1
    else:
        state.set((Fission((1, 0, 0), 20),))
        x1, z1 = x0+1, z0

    state.set((SMove((0, 1, 0)), SMove((0, 1, 0))))
    while j < R:
        #print(">", j)
        if len(mv[j]) == 0:
            j += 1
            continue
        #print(j, mv[j])
        if num0 < num1:
            one = 0
            for xa, za, xb, zb in mv[j]:
                assert xa == xb == x0 == x1
                one = (za == zb)
                if one:
                    zb += 1
                while za != z0 or zb != z1:
                    ca = cb = None
                    if za == z0:
                        ca = Wait()
                    else:
                        if z1 <= za:
                            # 相方によって経路を塞がれている場合
                            if z1-1 - z0 == 0:
                                ca = Wait()
                            else:
                                ca = SMove((0, 0, min(LD, z1-1 - z0)))
                            z0 += min(LD, z1-1-z0)
                        else:
                            ca = SMove((0, 0, min(LD, za - z0)))
                            z0 += min(LD, za-z0)
                    if zb == z1:
                        cb = Wait()
                    else:
                        cb = SMove((0, 0, min(LD, zb - z1)))
                        z1 += min(LD, zb-z1)
                    state.set((ca, cb))
                if one:
                    state.set((Fill((0, -1, 0)), Wait()))
                else:
                    state.set((GFill((0, -1, 0), (0, 0, zb-za)), GFill((0, -1, 0), (0, 0, -(zb-za)))))
                x0 = xa; z0 = za
                x1 = xb; z1 = zb
            # 次の場所まで移動
            j += 1
            while j < R and len(mv[j]) == 0:
                j += 1
            if j < R:
                xa, za, xb, zb = mv[j][0]
                if za == zb:
                    zb += 1
                cs0 = single_move((x0, z0), (xa, za), 0)
                l1 = max(min(SD, xb - x1), -SD)
                l2 = max(min(SD, zb - z1), -SD)
                if l2 == 0:
                    l1 = max(min(LD, xb - x1), -LD)
                    cs1 = [SMove((l1, 0, 0))]
                    cs1.extend(single_move((x1 + l1, z1), (xb, zb), 0))
                else:
                    cs1 = [LMove((l1, 0, 0), (0, 0, l2))]
                    # x1 += l1; z1 += l2
                    cs1.extend(single_move((x1 + l1, z1 + l2), (xb, zb), 1))

                cs = add_commands_with_blocking((x0, z0), (x1, z1), cs0, cs1)
                for c0, c1 in cs:
                    state.set((c0, c1))

                """
                for i in range(max(len(cs0), len(cs1))):
                    c0 = Wait() if len(cs0) <= i else cs0[i]
                    c1 = Wait() if len(cs1) <= i else cs1[i]
                    state.set((c0, c1))
                """
                x0 = xa; z0 = za
                x1 = xb; z1 = zb
        else:
            #state.nanobot_debug()
            one = 0
            for xa, za, xb, zb in mv[j]:
                #print(xa, za, xb, zb)
                assert za == zb == z0 == z1
                one = (xa == xb)
                if one:
                    xb += 1

                while xa != x0 or xb != x1:
                    ca = cb = None
                    if xa == x0:
                        ca = Wait()
                    else:
                        if x1 <= xa:
                            # 相方によって経路を塞がれている場合
                            if x1 - 1 - x0 == 0:
                                ca = Wait()
                            else:
                                ca = SMove((min(LD, x1-1 - x0), 0, 0))
                            x0 += min(LD, x1-1-x0)
                        else:
                            ca = SMove((min(LD, xa - x0), 0, 0))
                            x0 += min(LD, xa-x0)
                    if xb == x1:
                        cb = Wait()
                    else:
                        cb = SMove((min(LD, xb - x1), 0, 0))
                        x1 += min(LD, xb-x1)
                    state.set((ca, cb))
                #state.nanobot_debug()
                #print("g")
                if one:
                    state.set((Fill((0, -1, 0)), Wait()))
                    #state.nanobot_debug()
                else:
                    #state.nanobot_debug()
                    state.set((GFill((0, -1, 0), (xb-xa, 0, 0)), GFill((0, -1, 0), (-(xb-xa), 0, 0))))
                x0 = xa; z0 = za
                x1 = xb; z1 = zb
            # 次の場所まで移動
            j += 1
            while j < R and len(mv[j]) == 0:
                j += 1
            if j < R:
                xa, za, xb, zb = mv[j][0]
                if xa == xb:
                    xb += 1
                cs0 = single_move((x0, z0), (xa, za), 1)
                l1 = max(min(SD, xb - x1), -SD)
                l2 = max(min(SD, zb - z1), -SD)
                if l1 == 0:
                    l2 = max(min(LD, zb - z1), -LD)
                    cs1 = [SMove((0, 0, l2))]
                    cs1.extend(single_move((x1, z1 + l2), (xb, zb), 0))
                else:
                    cs1 = [LMove((0, 0, l2), (l1, 0, 0))]
                    #x1 += l1; z1 += l2
                    cs1.extend(single_move((x1 + l1, z1 + l2), (xb, zb), 0))
                #print(cs0, cs1, x0, z0, xa, za, x1, z1, xb, zb)
                #state.nanobot_debug()
                cs = add_commands_with_blocking((x0, z0), (x1, z1), cs0, cs1)
                for c0, c1 in cs:
                    state.set((c0, c1))
                """
                for i in range(max(len(cs0), len(cs1))):
                    c0 = Wait() if len(cs0) <= i else cs0[i]
                    c1 = Wait() if len(cs1) <= i else cs1[i]
                    state.set((c0, c1))
                    #state.nanobot_debug()
                """
                x0 = xa; z0 = za
                x1 = xb; z1 = zb
            #print("m", j)
            #state.nanobot_debug()
    # 合体
    while x0+1 < x1:
        l = min(x1 - x0 - 1, LD)
        state.set((Wait(), SMove((-l, 0, 0))))
        x1 -= l
    while z0+1 < z1:
        l = min(z1 - z0 - 1, LD)
        state.set((Wait(), SMove((0, 0, -l))))
        z1 -= l
    if x0 != x1:
        state.set((FusionP((1, 0, 0)), FusionS((-1, 0, 0))))
    else:
        state.set((FusionP((0, 0, 1)), FusionS((0, 0, -1))))
    xf = x0; zf = z0

#state.set((Flip(),))

# 帰る
cs = single_move((xf, zf), (0, 0), 1)
for c in cs:
    state.set((c,))

while yf > 0:
    l = min(yf, LD)
    #state.nanobot_debug()
    state.set((SMove((0, -l, 0)),))
    yf -= l

state.set((Halt(),))

state.check()
