import numpy as np
import matplotlib.pyplot as plt
import os
import sys

choice = ''
options = ('1m', '300k')
while choice not in options:
    choice = input(f"how many geant counts ({'. '.join(options)}) ").lower()

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
    x, y = an_es[condition], an_is[condition]
    cur_cts = np.trapz(x=x, y=y)
    an_cts.append(cur_cts)

an_cts = np.asarray(an_cts)
# an_cts = an_cts / np.sum(an_cts) * np.sum(hg)
print(np.sum(an_cts))
print("analytical counts^")
# an_cts = geant_counts * an_cts / np.sum(an_cts)

fig, ax = plt.subplots()

ax.bar(
    centers, an_cts, width=np.diff(bins), align='center',
    label='analytical', linewidth=1, color='steelblue',
    alpha=1, zorder=0)
ax.hist(
    channels[chosen_idx], bins=bins, label='geant',
    histtype='step', color='black', alpha=1, zorder=1, linewidth=1.5,
    align='right')
ax.legend()

ax.set_title(f'Compare G4 to {choice}-optimized flat response : {geant_counts} events')
ax.set_xscale('linear')
ax.set_yscale('linear')
ax.set_xlabel('Energy (keV)')
ax.set_ylabel('Counts')

fig.set_size_inches(8, 6)
fig.tight_layout()
plt.show()
