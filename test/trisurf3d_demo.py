from mpl_toolkits.mplot3d import Axes3D
from matplotlib import cm
import matplotlib.pyplot as plt
import numpy as np
from math import *

n_angles = 36
n_radii = 36

# An array of radii
# Does not include radius r=0, this is to eliminate duplicate points
radii = np.linspace(0.125, 1.0, n_radii)

# An array of angles
angles = np.linspace(0, 2*np.pi, n_angles, endpoint=False)

# Repeat all angles for each radius
angles = np.repeat(angles[...,np.newaxis], n_radii, axis=1)

# Convert polar (radii, angles) coords to cartesian (x, y) coords
# (0, 0) is added here. There are no duplicate points in the (x, y) plane
x = np.append(0, (radii*np.cos(angles)).flatten())
y = np.append(0, (radii*np.sin(angles)).flatten())


def phi(r,x,w,sigma_tr,D):
	r = np.linalg.norm(x)
	cos = np.dot(x, w) / r
	res = (1/4*pi*D) * exp(-sigma_tr * r) * (1/r) * (1 + 3*D*(1 + sigma_tr * r)/r * cos)
	return res
	
sigma_a =  0.01
sigma_s = 1.0
g = 0.0
sigma_t_prime = sigma_s * (1 - g) + sigma_a
D = 1 / (3 * sigma_t_prime)
sigma_tr = sqrt(sigma_a / D)
print(sigma_t_prime,D)
w = np.array([0,1,0])
w = w  / np.linalg.norm(w)
# Pringle surface
z = []
for i in range(0,len(x)):
	xx = np.array([x[i],.5,y[i]])
	z.append(phi(len(xx),xx,w,sigma_tr,D))
if isnan(z[0]):
	z[0] = 0.0
z = np.array(z)
 
print(len(x),len(y),len(z), z[0])
from matplotlib.patches import FancyArrowPatch
from mpl_toolkits.mplot3d import proj3d

class Arrow3D(FancyArrowPatch):
    def __init__(self, xs, ys, zs, *args, **kwargs):
        FancyArrowPatch.__init__(self, (0,0), (0,0), *args, **kwargs)
        self._verts3d = xs, ys, zs

    def draw(self, renderer):
        xs3d, ys3d, zs3d = self._verts3d
        xs, ys, zs = proj3d.proj_transform(xs3d, ys3d, zs3d, renderer.M)
        self.set_positions((xs[0],ys[0]),(xs[1],ys[1]))
        FancyArrowPatch.draw(self, renderer)

a = Arrow3D([0,w[0]],[0,w[2]],[0,w[1]], mutation_scale=20, lw=1, arrowstyle="-|>", color="k")


fig = plt.figure()
ax = fig.gca(projection='3d')
ax.add_artist(a)

minbound = -1.0
maxbound = 1.0
ax.auto_scale_xyz([minbound, maxbound], [minbound, maxbound], [minbound, maxbound])

ax.plot_trisurf(x, y, z, cmap=cm.jet, linewidth=0.2)

plt.show()