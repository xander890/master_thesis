from math import *
from numpy import *
from numpy.linalg import * 
import matplotlib.pyplot as plt
from matplotlib.widgets import Slider, Button, RadioButtons

# Parameter declaration, assuming not separated light on wavelength.
ni = 1.0
sigma_a =  0.1
sigma_s = 1.0
g = 0.0
l = None
ax = None
angle = 0

sigma_t_prime = 0.0
D = 0.0
sigma_tr = 0.0
C_s = 0.0
C_E = 0.0
C_s_inv = 0.0

def C_sigma(ni):
	res = 0.919317 - 3.4793 * ni + 6.75355 * (ni ** 2) - 7.80989 * (ni ** 3)
	res += 4.98554 * (ni ** 4) - 1.36881 * (ni**5)
	return 0.25 * (1 - res)

def C_e(ni):
	res = 0.828421 - 2.62051 * ni + 3.362316 * (ni ** 2) - 1.95284 * (ni ** 3)
	res += 0.236494 * (ni ** 4) + 0.145787 * (ni**5)
	return 0.5 * (1 - res)
		
def S_infinite(x, w, r, n_o):
	
	if r == 0:
		return 4000

	oneplus = 1 + sigma_tr * r

	#dot products
	dot_x_w = dot(x,w)
	dot_x_n = dot(x,n_o)
	dot_w_n = dot(w,n_o)

	initial_coefficients = 0.25 * 0.25 * 1 / (pi * pi) * 1 / (C_s_inv) * exp(-sigma_tr * r) / (r**3)
	first_term = C_s * ((r**2) / D + 3 * oneplus * dot_x_w)
	intermediate = (3 * oneplus / (r**2) + sigma_tr) * 3 * D * dot_x_w
	second_term = -C_E  * (3 * D * oneplus * dot_w_n - (oneplus + intermediate) * dot_x_n)
	#print("1   ",initial_coefficients * (first_term))
	#print("2   ",initial_coefficients * (second_term))
	#print("1+2 ",initial_coefficients * (second_term + first_term))

	return initial_coefficients * (first_term + second_term)
	
	
def S_finite(xi, wi, xo, nin, no):
	print("Calculating BSSRDF: S_a " + str(sigma_a) + ", wi = " + str(wi))
	global ni
	global sigma_a
	global sigma_s
	global g
	
	#derivated quantities
	global sigma_t_prime 
	sigma_t_prime = sigma_s * (1 - g) + sigma_a
	global D 
	D = 1 / (3 * sigma_t_prime)
	global sigma_tr
	sigma_tr = sqrt(sigma_a / D)
	global C_s
	C_s = C_sigma(ni)
	global C_E
	C_E = C_e(ni)
	global C_s_inv
	C_s_inv = C_sigma(1.0/ni)

	x = xo - xi
	if norm(x) == 0.0:
		return 1.0
	c = - dot(nin, wi)
	w12 = ni * wi + (ni * c - sqrt(1 - ni * ni * (1 - c * c))) * nin 
	mu = - dot(no,w12)
	
	reduced_albedo = (sigma_s * (1 - g)) / sigma_t_prime
	de = 2.131 * D / sqrt(reduced_albedo)
	A = (1 - C_E) / (2 * C_s)
	r = norm(x)
	
	if (mu > 0):
		dr = r ** 2 + D * mu * (D * mu + 2 * de * sqrt((r ** 2 - dot(x,w12) ** 2)/(r ** 2 + de ** 2)))
	else:
		dr = r ** 2 + 1 / ((3 * sigma_t_prime) ** 2)
	dr = sqrt(dr)
	xnorm = x / norm(x)
	nicross = cross(nin,x)
	nicross /= norm(nicross)
	nistar = cross(xnorm,nicross, axis = 0)
	xv = xi + 2 * A * de * nistar
	dv = norm(xo - xv)
	wv = w12 - 2 * dot(w12,nistar) * nistar

	print("dr",dr)
	print("dv",dv)
	print("reduced",reduced_albedo)
	print("sigma tr", sigma_tr)
	print("de",de)
	s1 = S_infinite(xo - xi, w12, dr, no)
	s2 = S_infinite(xo - xv, wv, dv, no)
	print("S1",s1)
	print("S2",s2)
	return max(s1-s2,0.0)

def update_sa(val):	
	global sigma_a
	sigma_a = val
	data = [S_finite(xi,wi,array([r,0,0]),n,n) for r in input]
	l.set_ydata(data)
	fig = plt.gcf()
	ax.set_ylim([min(data), max(data)])
	
	fig.canvas.draw_idle()

def update_ni(val):	
	global ni
	ni = val
	data = [S_finite(xi,wi,array([r,0,0]),n,n) for r in input]
	l.set_ydata(data)
	fig = plt.gcf()
	ax.set_ylim([min(data), max(data)])
	
	fig.canvas.draw_idle()
	
def update_angle(val):	
	angle = 90 - val
	angle_rad = angle * pi / 180
	wi = array([cos(angle_rad),sin(angle_rad),0])
	l.set_ydata([S_finite(xi,wi,array([r,0,0]),n,n) for r in input])
	fig = plt.gcf()
	fig.canvas.draw_idle()

if __name__ == '__main__':
	angle_rad = (90 - angle) * pi / 180
	xi = array([0,0,0])
	wi = array([cos(angle_rad),sin(angle_rad),0])
	n = array([0,1,0])
	plt.subplots_adjust(bottom=0.25)
	print("BSSRDF: ", S_finite(xi,wi,array([1,0,0]),n,n))
"""	
	input = [r/10.0 for r in range(-120,120)]
	output = [S_finite(xi,wi,array([r,0,0]),n,n) for r in input]
	plt.yscale('log')
	plt.ylabel(r'BSSRDF [$cm^{-1} sr^{-1}$]')
	plt.xlabel('x [cm]')
	ax = plt.gca()
	l, = plt.plot(input,output)
	
	axcolor = 'lightgoldenrodyellow'
	axfreq = plt.axes([0.2, 0.12, 0.65, 0.03], axisbg=axcolor)
	axfreq.set_xscale('log')
	slid = Slider(axfreq, r'$\sigma_a$', 0.01, 10.0, valinit=sigma_a)
	slid.on_changed(update_sa)
	
	ax2 = plt.axes([0.2, 0.07, 0.65, 0.03], axisbg=axcolor)
	slid2 = Slider(ax2, r'$\omega_i$ (angle)', 0.0, 90.0, valinit=angle)
	slid2.on_changed(update_angle)

	ax3 = plt.axes([0.2, 0.02, 0.65, 0.03], axisbg=axcolor)
	slid3 = Slider(ax3, r'$\eta$', 0.5, 4.0, valinit=ni)
	slid3.on_changed(update_ni)

	
	plt.show()
"""