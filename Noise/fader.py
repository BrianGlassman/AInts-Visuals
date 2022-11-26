# count = 0
# total = 0
# for a in         range(-100, 100):
#     for b in     range(-100, 100):
#         for c in range(-100, 100):
#             total+= 1

#             f = lambda t: t**3 * (a*t**2 - b*t + c)
#             if f(0) != 0: continue
#             if f(1) != 1: continue

#             good = True
#             for t in range(0, 100):
#                 t = t * 0.01
#                 if abs(f(0.5 - t) - (1 - f(0.5 + t))) > 1e-5:
#                     good = False
#                     break
#             if (not good): continue

#             print(a, b, c)
#             count += 1
# print(f"{count} / {total}")

#%%
import numpy as np
from matplotlib import pyplot as plt

a = 6
b = 15
c = 10
perlin = lambda t: t**3 * (a*t**2 - b*t + c)

P0 = np.array([0, 0])
P1 = np.array([0.25, 0])
P3 = np.array([1, 1])
P2 = P3 - P1
def _bezier(t):
    it = 1 - t
    it2 = it**2
    it3 = it**3
    t2 = t**2
    t3 = t**3
    return it3*P0 + 3*it2*t*P1 + 3*it*t2*P2 + t3*P3
def bezier(t):
    return [_bezier(_t) for _t in t]

t = np.arange(0, 1.01, 0.1)
# t = np.arange(0, 0.1, 0.01)

fig, axs = plt.subplots(1, 2)
fig.suptitle(P1[0])
axs[0].set_title("red = x(t), blue = y(t)")
axs[1].set_title("y(x)")
axs[0].plot(t, t, 'k--')
axs[1].plot(t, t, 'k--')
axs[1].plot(t, perlin(t), 'k.')
# ax.plot(t, bezier(t), 'r')
ans = bezier(t)
x = [row[0] for row in ans]
y = [row[1] for row in ans]
axs[1].plot(x, y, 'b.')
axs[0].plot(t, x, 'r')
axs[0].plot(t, y, 'b')
# r = [np.sqrt(x_**2 + y_**2) for x_,y_ in zip(x, y)]
# ax.plot(t, r, 'k.')

P1 = np.array([1, 0])
P2 = P3 - P1
ans = bezier(t)
x = [row[0] for row in ans]
y = [row[1] for row in ans]
axs[1].plot(x, y, 'r.')

axs[0].grid(True)
axs[1].grid(True)

#%%

P0 = 0
# 0.5 approximates Perlin fade, lower values are earlier ramp (good)
# 0.0 is a straight line
P1 = 0.3
P2 = 1 - P1
P3 = 1
def _bezierX(t):
    it = 1 - t
    it2 = it**2
    it3 = it**3
    t2 = t**2
    t3 = t**3
    v = it3*P0 + 3*it2*t*P1 + 3*it*t2*P2 + t3*P3
    return v
def _bezierY(t):
    it = 1 - t
    t2 = t**2
    t3 = t**3
    v = 3*it*t2 + t3
    return v

def bezier(x):
    t = x
    for i in range(100):
        xCalc = _bezierX(t)
        # print(f"{i}: {t:0.5f} --> {xCalc:0.5f}, d = {d:0.5f}")
        if abs(xCalc - x) < 1e-9:
            break
        # t = t - d*(xCalc - x)
        t = t - (xCalc - x)
    else:
        raise RuntimeError("Value not converged")
    return _bezierY(t)


def perlin(t):
    a = 6
    b = 15
    c = 10
    return t**3 * (a*t**2 - b*t + c)

import numpy as np
from matplotlib import pyplot as plt
# x = np.arange(0, 1.01, 0.1)
x = [0.2]

fig, ax = plt.subplots()
ax.plot(x, [bezier(_x) for _x in x], '.-')
ax.plot(x, [perlin(_x) for _x in x], 'k.-')
ax.set_title(P1)
ax.grid(True)
