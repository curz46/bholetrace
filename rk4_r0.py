# This program computes hardcoded values with 4th-order Rutta-Kunge for testing.
# It takes reference from the below link, but is modified to converge on a solution
# and uses two differentials instead of one.
# https://www.codesansar.com/numerical-methods/runge-kutta-fourth-order-rk4-python-program.htm

import math 

def f(a,u,z):
    return z

def g(a,u,z): #du/da
    return -u + (3/2)*pow(u,2)

def rk4(a0,u0,z0,an,n):
    h = (an-a0)/n
    
    for i in range(n):
        k1 = h * (f(a0, u0, z0))
        l1 = h * (g(a0, u0, z0))
        k2 = h * (f((a0+h/2), (u0+k1/2), (z0+l1/2)))
        l2 = h * (g((a0+h/2), (u0+k1/2), (z0+l1/2)))
        k3 = h * (f((a0+h/2), (u0+k2/2), (z0+l2/2)))
        l3 = h * (g((a0+h/2), (u0+k2/2), (z0+l2/2)))
        k4 = h * (f((a0+h), (u0+k3), (z0+l3)))
        l4 = h * (g((a0+h), (u0+k3), (z0+l3)))
        k = (k1+2*k2+2*k3+k4)/6
        l = (l1+2*l2+2*l3+l4)/6
        un = u0 + k
        print('%.4f\t%.4f\t%.4f\t%.4f\t(%.4f)'% (a0,u0,z0,un,1/un) )
        print('-------------------------')
        u0 = un
        a0 = a0+h
        z0 = z0 + l
    
    print('\nAt a=%.4f, u=%.4f, z=%.4f' %(an,un,z0))
    print('1 / un = %.4f' % (1/un))


def rk4_step(a,u,z,h):
    k1 = h * (f(a, u, z))
    l1 = h * (g(a, u, z))
    k2 = h * (f((a+h/2), (u+k1/2), (z+l1/2)))
    l2 = h * (g((a+h/2), (u+k1/2), (z+l1/2)))
    k3 = h * (f((a+h/2), (u+k2/2), (z+l2/2)))
    l3 = h * (g((a+h/2), (u+k2/2), (z+l2/2)))
    k4 = h * (f((a+h), (u+k3), (z+l3)))
    l4 = h * (g((a+h), (u+k3), (z+l3)))
    k = (k1+2*k2+2*k3+k4)/6
    l = (l1+2*l2+2*l3+l4)/6
    return a+h,u+k,z+l

def rk4_r0(a0,u0,z0,u_max,da_target):
    """
    Use 4th-order Rutta-Kunge with an adaptive stepsize to find
    the u=1/r_0 for du/da = g(a, u, z).
    """
    MIN_ITER = 30
    MAX_ITER = 100
    h = 0.5
    al, ul = a0, u0
    for i in range(MAX_ITER): #max iterations
        a0,u0,z0 = rk4_step(a0,u0,z0,h)
        print(f"{i: >2}: {a0:.3f}  {u0:.5f}  {z0:.5f}")

        if u0>u_max:
            return a0,-1,z0
        if i>MIN_ITER and abs(al-a0)<da_target:
            return a0,u0,z0
        if ul>u0:
            h = -h/2

        al, ul = a0, u0

    return a0,u0,z0

b = 3
print(f"Impact parameter (b) = {b}")
a0,u0,z0 =0,0,1/b

print("    ai     ui       zi")
an,un,zn = rk4_r0(a0,u0,z0,10,0.00000001)
print("done:")
print(f"{an:.10f}\t{un:.10f}\t{zn:.3f}")
