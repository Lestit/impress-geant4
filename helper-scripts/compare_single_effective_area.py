import traceback
import numpy as np
import matplotlib as mpl
import matplotlib.pyplot as plt
import os
import sys

try:
    plt.style.use('/Users/settwi/grad_school/glesener/agu-2021/agu.mplstyle')
except Exception as e:
    traceback.print_exc()
    input("any key to continue")
    raise

choice = ''
options = ('1m', '300k')
opt_nums = [1000000, 300000]
opt_map = {k: v for (k, v) in zip(options, opt_nums)}
while choice not in options:
    choice = input(f"how many geant counts ({'. '.join(options)}) ").lower()
tot_cnts = opt_map[choice]

analytical_data_dir = os.path.join(os.path.dirname(__file__), f'photons-{choice}')
analytical_files = os.listdir(analytical_data_dir)
in_fn = sys.argv[1]
options = ('C1', 'M1', 'M5', 'X1')
choice = ''

for o in options:
    if o in in_fn or o.lower() in in_fn:
        choice = o
        break

while choice not in options:
    choice = input(f"what flare attenuator did you want to compare with ({', '.join(options)}) ").upper()

chosen_idx = options.index(choice)
ana_fn = os.path.join(
    analytical_data_dir, next(f for f in analytical_files if choice in f))

an_es, an_is = np.loadtxt(ana_fn, unpack=True)
ge_es, ch_ids = np.loadtxt(in_fn, unpack=True)

print("real analytical counts", np.trapz(x=an_es, y=an_is))

channels = dict()
# emax = -1
for e, ch in zip(ge_es, ch_ids):
    try:
        channels[int(ch)].append(e)
    except KeyError:
        channels[int(ch)] = [e]
    # finally:
    #     emax = max(emax, e)

emax = np.max((np.max(an_es), np.max(ge_es)))
bins = np.linspace(1, emax, num=200)
try:
    hg, ed = np.histogram(channels[chosen_idx], bins=bins)
except KeyError:
    print('key not in histogram dict, assuming there\'s only one channel')
    chosen_idx = 0
    hg, ed = np.histogram(channels[chosen_idx], bins=bins)
finally:
    geant_counts = len(channels[chosen_idx])

centers = ed[:-1] + np.diff(ed)
hist_pts = np.array(hg, dtype=np.float64)

an_cts = []
print(f"bins start: {np.min(bins)}; bins end: {np.max(bins)}")
for i in np.arange(bins.size - 1):
    start, end = bins[i], bins[i+1]
    condition = np.logical_and(an_es >= start, an_es < end)
    cur_cts = np.sum(an_is[condition])
    an_cts.append(cur_cts)


an_cts = np.asarray(an_cts)
print(np.sum(an_cts))
print("analytical counts^")

fig, ax = plt.subplots()

ax.bar(
    centers, an_cts, width=np.diff(bins), align='center',
    label=f'{np.sum(an_cts):.0f} counts analytical', linewidth=1, color='red',
    alpha=0.6, zorder=1)

# scale because the Geant model has some extra crystal space
full_diam = 43 # mm
cryst_diam = 37 # mm
g4cts = np.array(channels[chosen_idx])
g4bins, _ = np.histogram(g4cts, bins=bins)
g4bins = np.array(g4bins, dtype=np.float64)
g4bins *= (43 / 37)**2
g4lab = f'{np.sum(g4bins):.0f} counts geant'
ax.bar(
    centers, g4bins, width=np.diff(bins), label=g4lab,
    color='black', alpha=1, zorder=0,
    align='center')

ax.set_title(f'G4 & analytical {choice}-optimized flat')
ax.set_xscale('linear')
ax.set_yscale('linear')
ax.set_xlabel('Energy (keV)')
ax.set_ylabel('Counts')

# ax.axvline(34.7, label='secondary line Ce', color='black')
ax.axvline(40.44, label='Cerium k edge @ 40.44 keV', color='blue')

leg = ax.legend()

fig.tight_layout()

plt.show()
