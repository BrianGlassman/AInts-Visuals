import random

import numpy as np
from matplotlib import pyplot as plt

random.seed(1670526744)

pVectors = {}

def getFloat() -> float:
    mn = -1 ; mx = 1
    raw = random.random() / 1
    scaled = raw * (mx - mn)
    centered = scaled - (mx - mn) / 2
    return centered

def getPVector(coords: tuple[int]) -> tuple[float]:
    if coords not in pVectors:
        x = getFloat() 
        y = getFloat() 
        mag = np.sqrt(x*x + y*y)
        x /= mag ; y /= mag

        pVectors[coords] = (x, y)
    return pVectors[coords]

def lerp(fraction: float, low: float, high: float):
    return low + fraction * (high - low)

def lerpV(fraction: float, low: tuple[float], high: tuple[float]):
    x = lerp(fraction, low[0], high[0])
    y = lerp(fraction, low[1], high[1])
    return x, y

def getCorners(coords: tuple[float]):
    assert len(coords) == 2
    x, y = coords

    # Get corners and corresponding vectors
    xLow = np.floor(x) ; yLow = np.floor(y)
    xHigh = xLow + 1 ; yHigh = yLow + 1
    ll = getPVector((xLow, yLow))
    lh = getPVector((xLow, yHigh))
    hl = getPVector((xHigh, yLow))
    hh = getPVector((xHigh, yHigh))

    return xLow, xHigh, yLow, yHigh, ll, lh, hl, hh

def getNoise(coords: tuple[float]):
    xLow, xHigh, yLow, yHigh, ll, lh, hl, hh = getCorners(coords)

    # Find relative coordinates in cell
    x, y = coords
    x -= xLow ; y -= yLow

    # --- Interpolate ---
    # Y axis
    xl = lerpV(x, ll, hl)
    xh = lerpV(x, lh, hh)

    # X axis
    xy = lerpV(y, xl, xh)

    return xy

def DrawArrow(ax, x, y, dx, dy, **arrowprops):
    ax.annotate("", xy=(x + dx, y + dy), xytext=(x, y),
        arrowprops=arrowprops)

def DrawNoise(scale = 1/2, **kwargs):
    arrowprops = kwargs
    arrowprops['arrowstyle'] = '->'

    fig, ax = plt.subplots()

    for x in np.arange(-2, 2.1, 1):
        for y in np.arange(-2, 2.1, 1):
            p = getPVector((x, y))
            DrawArrow(ax, x, y, p[0]*scale, p[1]*scale, **arrowprops)
            
    ax.set_xlim([-2-scale, 2+scale])
    ax.set_ylim([-2-scale, 2+scale])
    ax.set_aspect('equal')
    ax.grid(True)
    return fig, ax

def PrepGrid():
    for x in np.arange(-2, 2.1, 1):
        for y in np.arange(-2, 2.1, 1):
            getPVector((x, y))
PrepGrid()

def ShowMethod():
    # Full field
    _, ax = DrawNoise(scale=1)

    # Applying to a point on the field
    _, ax = plt.subplots()
    x = 0.7 ; y = 1.8
    xLow, xHigh, yLow, yHigh, ll, lh, hl, hh = getCorners((x, y))
    DrawArrow(ax, xLow, yLow, ll[0], ll[1],
        arrowstyle = '->', color = 'grey')
    DrawArrow(ax, xHigh, yLow, hl[0], hl[1],
        arrowstyle = '->', color = 'grey')
    DrawArrow(ax, xLow, yHigh, lh[0], lh[1],
        arrowstyle = '->', color = 'grey')
    DrawArrow(ax, xHigh, yHigh, hh[0], hh[1],
        arrowstyle = '->', color = 'grey')
    ax.set_xlim([-1, 2])
    ax.set_ylim([ 0, 3])
    ax.set_xticks([0, 1])
    ax.set_yticks([1, 2])
    ax.set_aspect('equal')
    ax.grid(True)

    ax.plot(x, y, 'kx')
    p = getNoise((x, y))
    ax.plot(x+p[0], y+p[1], 'ko')
    DrawArrow(ax, x, y, p[0], p[1],
        arrowstyle = '->')
# ShowMethod()

def ShowCross(scale = 1):
    _, ax = plt.subplots()
    x = 0.2 ; y = 1.1
    xLow, xHigh, yLow, yHigh, ll, lh, hl, hh = getCorners((x, y))
    DrawArrow(ax, xLow, yLow, ll[0]*scale, ll[1]*scale,
        arrowstyle = '->', color = 'grey')
    DrawArrow(ax, xHigh, yLow, hl[0]*scale, hl[1]*scale,
        arrowstyle = '->', color = 'grey')
    DrawArrow(ax, xLow, yHigh, lh[0]*scale, lh[1]*scale,
        arrowstyle = '->', color = 'grey')
    DrawArrow(ax, xHigh, yHigh, hh[0]*scale, hh[1]*scale,
        arrowstyle = '->', color = 'grey')
    ax.set_xlim([-1, 2])
    ax.set_ylim([ 0, 3])
    ax.set_xticks([0, 1])
    ax.set_yticks([1, 2])
    ax.set_aspect('equal')
    ax.grid(True)

    p = getNoise((x, y))
    DrawArrow(ax, x, y, p[0]*scale, p[1]*scale,
        arrowstyle = '->', color = 'red')
    ax.plot(x, y, 'rx')
    ax.plot(x+p[0]*scale, y+p[1]*scale, 'ro')

    x = 0.8 ; y = 1.4
    p = getNoise((x, y))
    DrawArrow(ax, x, y, p[0]*scale, p[1]*scale,
        arrowstyle = '->', color = 'blue')
    ax.plot(x, y, 'bx')
    ax.plot(x+p[0]*scale, y+p[1]*scale, 'bo')
# ShowCross(scale =0.3)
# ShowCross(scale = 1)

def Progression(scale = 1):
    _, ax = plt.subplots()
    T = np.arange(0, 1.0001, 0.1)
    DrawArrow(ax, 0, 0, 1, 0)
    DrawArrow(ax, 1, 1, -1, 0)
    ax.plot(T, T, '.-')
    for t in T:
        p = lerp(t, 1, -1) * scale
        DrawArrow(ax, t, t, p, 0, arrowstyle = '->')
    ax.grid(True)
    ax.set_aspect('equal')
# Progression(1)
# Progression(0.5)

def GridShift():
    scale = 0.3
    fig, ax = plt.subplots()
    ax.grid(True)
    ax.set_aspect('equal')
    # Vertical lines
    for x in np.arange(0, 3.1, 1):
        X = []
        Y = []
        for y in np.arange(0, 3.1, 1):
            p = getNoise((x, y))
            p = [p[0]*scale, p[1]*scale]
            X.append(x + p[0])
            Y.append(y + p[0])
        ax.plot(X, Y, 'k.-')
    # Horizontal lines
    for y in np.arange(0, 3.1, 1):
        X = []
        Y = []
        for x in np.arange(0, 3.1, 1):
            p = getNoise((x, y))
            p = [p[0]*scale, p[1]*scale]
            X.append(x + p[0])
            Y.append(y + p[0])
        ax.plot(X, Y, 'k.-')
    ax.set_xticks([0, 1, 2, 3])
    ax.set_yticks([0, 1, 2, 3])
# GridShift()