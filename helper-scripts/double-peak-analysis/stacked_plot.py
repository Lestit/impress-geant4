from collections import OrderedDict
import json
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.ticker as tck
import os

import overplot as op

# EXP_DPATH = 'cs137_new.dat'
raise ValueError("set experimental data path in argv (william update the code)")
do_dens = False
cts_or_density = lambda: 'counts density' if do_dens else 'counts'
do_kde = do_dens
do_hist = True

def fill_minor_log_ticks(ax, which='y'):
    locmin = tck.LogLocator(
        base=10.0,
        subs=np.arange(0, 10)/10,
        numticks=1000
    )
    apply = ax.yaxis if which == 'y' else ax.xaxis
    apply.set_minor_locator(locmin)
    apply.set_minor_formatter(tck.NullFormatter())


def main():
    dat = load_cs_data()
    max_cts = 45000 #find_max_cts(dat)

    fig, axs = plt.subplots(
        nrows=len(dat.keys())+1, ncols=1, figsize=(12, 2 + 2*len(dat.keys()))
    )
    axs = axs.flatten()
    bins = np.linspace(
        0,
        max_cts,
        num=2000
    )

    for k, ax in zip(dat.keys(), axs):
        for kk in dat[k].keys():
            op.plot_geant_counts(
                ax, dat[k][kk],
                bins=bins,
                label=f'{k} {kk}',
                do_histogram=do_hist,
                do_density=do_dens,
                do_kde=do_kde)
        ax.set_ylabel(f'{k}\n{cts_or_density()}')
        if do_dens:
            ax.set_ylim(8e-7, 1e-3)
        ax.set_yscale('log')
        ax.minorticks_on()
        ax.grid(visible=True, which='major')
        fill_minor_log_ticks(ax, which='y')

        # ax.legend(loc='upper center')
        ax.set_xlabel('Number of photons (scintillated or detected)')

    plot_exp_data(axs[-1])

    fig.suptitle('${}^{137}$Cs comparison: 5mm and 40mm long Geant, experiment')
    fig.tight_layout()
    # fig.savefig('cs137-overplotted.pdf')
    plt.show()


def plot_exp_data(ax):
    with open(EXP_DPATH, 'r') as f:
        exp_hg = json.loads(f.read())['histogram']
    bins = np.arange(4097)
    if do_dens:
        exp_hg = exp_hg / (np.diff(bins)[0] * np.sum(exp_hg))
    ax.stairs(
        exp_hg, bins, color='orange', label=f'experimental {cts_or_density()}'
    )
    ax.legend(loc='upper center')
    ax.set_xlabel('ADC channel')
    ax.set_ylabel(f'exp. {cts_or_density()}')
    ax.set_yscale('log')
    ax.minorticks_on()
    ax.grid(visible=True, which='major')
    ax.set_xlim(left=-2000, right=6000)
    if do_dens:
        ax.set_ylim(1e-5, 5e-3)
    fill_minor_log_ticks(ax, which='y')


def load_cs_data():
    convert = OrderedDict()
    convert['c1-scint-phots-out.tab'] = 'scintillated'
    convert['c1-si-out.tab'] = 'si counts'

    folders = [
        'cs137-5mm-rough',
        # 'cs137-5mm-polished',
        # 'cs137-40mm-polished',
        'cs137-40mm-rough',
        # 'cs137-5mm-rough-only662',
        # 'cs137-40mm-iso-polished-incorrect-placement'
    ]
    ret = dict()
    for fold in folders:
        fold_key = fold[1+fold.index('-'):]
        ret[fold_key] = dict()
        for fn, human_name in convert.items():
            with open(os.path.join(fold, fn), 'r') as f:
                ret[fold_key][human_name] = np.loadtxt(f)

    return ret


def find_max_cts(d):
    ret = 0
    for v in d.values():
        for vv in v.values():
            ret = max(ret, np.max(vv))
    return ret

if __name__ == '__main__': main()
