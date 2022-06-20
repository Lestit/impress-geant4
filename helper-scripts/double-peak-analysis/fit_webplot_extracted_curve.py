import numpy as np
import scipy.optimize as sco
import matplotlib.pyplot as plt
import sys
# guess parameters
# amp1, amp2, mu1, mu2, sig1, sig2
p0 = [0.0015, 0.0015, 6650, 6750, 100, 100]
fn = '150-shim-dump.tab'

def g(x, a, m, s):
    return a * np.exp(-(x - m)**2 / s**2)

def double_g(x, a1, a2, m1, m2, s1, s2):
    return g(x, a1, m1, s1) + g(x, a2, m2, s2)

x, y = np.loadtxt(fn, unpack=True)

# plt.plot(x, y, label='data')
xx = np.linspace(np.min(x), np.max(x), num=1000)
# plt.plot(xx, double_g(xx, *p0), label='initial guess')
# plt.legend()
# plt.show()

doub_params, doub_cov = sco.curve_fit(double_g, x, y, p0=p0)
doub_err = np.sqrt(np.diag(doub_cov))
print("DOUBLE")
for p, e in zip(doub_params, doub_err):
    print(p, '+-', e)

single_params, single_cov = sco.curve_fit(g, x, y, p0=p0[::2])
single_err = np.sqrt(np.diag(single_cov))
print("SINGLE")
for p, e in zip(single_params, single_err):
    print(p, '+-', e)

plt.scatter(x, y, label='data', color='black', s=5)
plt.plot(xx, double_g(xx, *doub_params), label='double gaussian fit')
plt.plot(xx, g(xx, *single_params), label='single fit')
plt.legend()
plt.show()
