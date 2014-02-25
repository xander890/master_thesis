from math import *
from numpy import *
from numpy.linalg import * 
import matplotlib.pyplot as plt
from matplotlib.widgets import Slider, Button, RadioButtons

ni = 1.0
sigma_a =  0.01
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
	return max(S_infinite(xo - xi, w12, dr, no) - S_infinite(xo - xv, wv, dv, no),0.0)



def phi(r,x,w,sigma_tr,D):
	r = norm(x)
	cos = dot(x, w) / r
	res = (1/4*pi*D) * exp(-sigma_tr * r) * (1/r) * (1 + 3*D*(1 + sigma_tr * r)/r * cos)
	return res
	
if __name__ == '__main__':

	sigma_a =  0.01
	sigma_s = 1.0
	g = 0.0
	sigma_t_prime = sigma_s * (1 - g) + sigma_a
	D = 1 / (3 * sigma_t_prime)
	sigma_tr = sqrt(sigma_a / D)
	w = array([1,1,0])
	r = 0.1
	rq = sqrt(2) * r
	#rowmajor
	flux = 1
	r1 = []
	[r1.append(0.0) for i in range(0,8)]
	r1[0] = phi(rq,array([-r,r,0]),w,sigma_tr,D)
	r1[1] = phi(r ,array([0,r,0]),w,sigma_tr,D)
	r1[2] = phi(rq,array([r,r,0]),w,sigma_tr,D)
	r1[3] = phi(r ,array([-r,0,0]),w,sigma_tr,D)
	r1[4] = phi(r ,array([r,0,0]),w,sigma_tr,D)
	r1[5] = phi(rq,array([-r,-r,0]),w,sigma_tr,D)
	r1[6] = phi(r ,array([0,-r,0]),w,sigma_tr,D)
	r1[7] = phi(rq,array([r,-r,0]),w,sigma_tr,D)
	
	sum = sum(r1)
	r1 = [i/sum for i in r1]
	
	print(r1[0],"\t",r1[1],"\t",r1[2])
	print(r1[3],"\t","\t","\t",r1[4])
	print(r1[5],"\t",r1[6],"\t",r1[7])
	
	sum2 = 0.0
	for x in r1:
		sum2 += x
	print (sum2)
	
