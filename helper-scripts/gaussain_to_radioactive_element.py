import matplotlib.pyplot as plt
import numpy as np
import scipy.stats as st

fn = '../data-tables/element-intens/ba133.txt'
e, intens = np.loadtxt(fn, unpack=True)
print(e, intens)

plot_range = np.arange(np.max(e) + 100)

photons_per_kev = 60
fano = 0.3
sds = fano * np.sqrt(e * photons_per_kev) #fano * np.sqrt(e * photons_per_kev)
norms = list()
for loc, scale in zip(e, sds):
    norms.append(st.norm(loc=loc, scale=scale))

norm_plots = np.array([n.pdf(plot_range) for n in norms])
tiled_intensity = np.tile(intens, (norm_plots.shape[1], 1)).T
to_plot = np.sum(norm_plots * tiled_intensity, axis=0)

fig, ax = plt.subplots()
ax.plot(plot_range, to_plot, label=f'Stddev = ${fano} \\cdot \\sqrt{{E_\\mu}}$')
ax.set_title('What happens if we just make the emission lines Gaussian?')
ax.set_xlabel('Energy (keV)')
ax.set_ylabel('Line intensity')
ax.legend()
fig.tight_layout()
plt.show()
