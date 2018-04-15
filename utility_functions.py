from numpy.linalg import norm
import numpy as np
from math import sqrt



def circumcenter(vert1, vert2, vert3):
    # https://gist.github.com/dhermes/9ce057da49df63345c33
    # H/T: wikipedia.org/wiki/Circumscribed_circle
    Ax, Ay = vert1
    Bx, By = vert2
    Cx, Cy = vert3

    D = 2 * (Ax * (By - Cy) + Bx * (Cy - Ay) + Cx * (Ay - By))
    norm_A = Ax ** 2 + Ay ** 2
    norm_B = Bx ** 2 + By ** 2
    norm_C = Cx ** 2 + Cy ** 2

    Ux = norm_A * (By - Cy) + norm_B * (Cy - Ay) + norm_C * (Ay - By)
    Uy = -(norm_A * (Bx - Cx) + norm_B * (Cx - Ax) + norm_C * (Ax - Bx))

    return np.array([Ux, Uy]) / D

def area(a, b, c):
    '''numpy arrays'''
    return 0.5 * norm( np.cross( b-a, c-a ) )

def set_aspect_equal_3d(ax):
    """Fix equal aspect bug for 3D plots.
    https://stackoverflow.com/a/35126679/5011857"""

    xlim = ax.get_xlim3d()
    ylim = ax.get_ylim3d()
    zlim = ax.get_zlim3d()

    from numpy import mean
    xmean = mean(xlim)
    ymean = mean(ylim)
    zmean = mean(zlim)

    plot_radius = max([abs(lim - mean_)
                       for lims, mean_ in ((xlim, xmean),
                                           (ylim, ymean),
                                           (zlim, zmean))
                       for lim in lims])

    ax.set_xlim3d([xmean - plot_radius, xmean + plot_radius])
    ax.set_ylim3d([ymean - plot_radius, ymean + plot_radius])
    ax.set_zlim3d([zmean - plot_radius, zmean + plot_radius])

def distance(xone, yone, xtwo, ytwo):
    xdif = pow((xtwo - xone),2)
    ydif = pow((ytwo - yone),2)
    return sqrt(xdif + ydif)

def ccw(Ax, Bx, Cx, Ay, By, Cy):
    return (Cy-Ay) * (Bx-Ax) > (By-Ay) * (Cx-Ax)

def intersect(Ax, Bx, Cx, Dx, Ay, By, Cy, Dy):
    return ccw(Ax, Cx, Dx, Ay, Cy, Dy) != ccw(Bx, Cx, Dx, By, Cy, Dy) and ccw(Ax, Bx, Cx, Ay, By, Cy) != ccw(Ax, Bx, Dx, Ay, By, Dy)
