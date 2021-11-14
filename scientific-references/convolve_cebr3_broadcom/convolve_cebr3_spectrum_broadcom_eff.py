import matplotlib.pyplot as plt
import numpy as np
import scipy.interpolate as si
import scipy.constants as sc

# assume both are columns of (wavelength, percent eff OR relative intensity)
CEBR3_SPECTRUM_FILE = '../cebr3/cebr3-optical-burst.raw.tab'
BROADCOM_EFF_FILE = '../optical-detection/sipm-efficiency-raw-wavelength.tab'

cebr3_wav, cebr3_intens = np.loadtxt(CEBR3_SPECTRUM_FILE, unpack=True)
cebr3_intens /= np.max(cebr3_intens)
bc_wav, bc_eff = np.loadtxt(BROADCOM_EFF_FILE, unpack=True)
bc_eff /= 100

bc_interp = si.interp1d(bc_wav, bc_eff)
multiplied = cebr3_intens * bc_interp(cebr3_wav)
average = np.mean(multiplied)

fig, ax = plt.subplots()
fig.set_size_inches(8, 6)

nm_to_ev = lambda x: sc.h * sc.c / (x * 1e-9) / sc.e
cb_eng = nm_to_ev(cebr3_wav)
bc_eng = nm_to_ev(bc_wav)

ax.plot(cb_eng, cebr3_intens, label='cerium bromide spectrum')
ax.plot(bc_eng, bc_eff, label='broadcom efficiency')
ax.plot(cb_eng, multiplied, label='multiplied together')
ax.axhline(
    average, label=f'average efficiency {average * 100:.1f}%',
    color='black')

ax.set_xlabel('Optical photon energy (eV)')
ax.set_ylabel('unitless (intensity or efficiency)')
ax.set_title('Compare cerium bromide spectrum to Broadcom SiPM efficiency')
ax.set_xlim(np.min(cb_eng), np.max(cb_eng))
ax.legend()

fig.tight_layout()
fig.savefig('cb-bc-eff-ev.png')
