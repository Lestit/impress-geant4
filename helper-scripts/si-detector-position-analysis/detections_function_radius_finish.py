import matplotlib as mpl
import matplotlib.pyplot as plt
import numpy as np
import os
import sys

plt.style.use('/Users/settwi/mpl-styles/fig.mplstyle')

PREFIXES = [
    'erou-brou', 'epol-brou',
    'erou-bpol', 'epol-bpol'
]
DECODED = [
    'rough edge & rough back', 'polished edge & rough back',
    'rough edge & polished back', 'polished edge & polished back'
]
PFX_MAP = {k: v for (k, v) in zip(PREFIXES, DECODED)}
RADIUSES = np.linspace(0, 37/2 - 1, num=10)
INCIDENT_COUNTS = 50000

def die():
    print()
    print(f'\tusage: python {__file__.split("/")[-1]} diff-finish-path')
    print('\tdiff-finish-path: folder containing a bunch of folders with the following prefixes, identifying different finishes:')
    print('\t\t' + '; '.join(PREFIXES))
    print()
    sys.exit(1)


def y_map(y_vals):
    return np.array(y_vals) #/ INCIDENT_COUNTS


def srt_help(s):
    return int(s.split('run')[1].split('202')[0].replace('-', ''))


def main():
    try:
        if not os.path.exists(sys.argv[1]):
            die()
    except IndexError as e:
        die()

    plot_func_rad(sys.argv[1])


def plot_func_rad(direc):
    folds = os.listdir(direc)
    att_fn = None
    dat = dict()
    for pfx in PREFIXES:
        dat[pfx] = list()
        wanted = [fold for fold in folds if pfx in fold]
        wanted.sort(key=srt_help)
        print([srt_help(w) for w in wanted])
        for w in wanted:
            full_path_no_file = os.path.join(direc, w)
            att_fn = att_fn or next(f for f in os.listdir(full_path_no_file) if 'si-out' in f)
            with open(os.path.join(full_path_no_file, att_fn)) as inf:
                dat[pfx].append(inf.read().count(os.linesep) + 1)
        print(pfx, dat[pfx])

    fig, ax = plt.subplots()
    for lab, cts in dat.items():
        # last run has truncated data
        num_plot = len(cts) if len(cts) == len(RADIUSES) else (len(cts) - 1)
        ax.plot(RADIUSES[:num_plot], y_map(cts[:num_plot]), label=PFX_MAP[lab], marker='o', ms=8, alpha=0.8)

    ax.axhline(y=y_map(INCIDENT_COUNTS), label=f'{INCIDENT_COUNTS} launched opticals', color='k')
    ax.set_title(f'Counts vs launch radius, different finishes')
    ax.set_ylabel('SiPM counts')
    # ax.set_ylabel('Proportion of opticals detected')
    ax.set_xlabel('Radius along $x$ (mm)')
    ax.legend()
    fig.tight_layout()
    fig.savefig('fixed.pdf')
    # plt.show()


if __name__ == '__main__': main()
