import matplotlib as mpl
import matplotlib.pyplot as plt
import numpy as np
import os
import sys
sys.path.append(os.path.dirname(__file__) + '/..')
from optical_params_shared import NUM_RUNS, RADIUSES
INCIDENT_COUNTS = NUM_RUNS
plt.style.use(os.getenv('MPL_DEFAULT_STYLE'))

PREFIX_FIRST_DECODE = {
    'Polished_LUT': 'raw+polishedLUT', 'Rough_LUT': 'raw+roughLUT',
    'PolishedTeflon_LUT': 'teflon+polishedLUT', 'RoughTeflon_LUT': 'teflon+roughLUT',
    'DirectlyCoupled': 'perfectly diffuse wrapping'
}
PFX_MAP = dict()
for first_pfx, first_decode in PREFIX_FIRST_DECODE.items():
    for second_pfx, second_decode in PREFIX_FIRST_DECODE.items():
        PFX_MAP[f'e{first_pfx}-b{second_pfx}'] = f'edge {first_decode}, back {second_decode}'

def die():
    print()
    print(f'\tusage: python {__file__.split("/")[-1]} diff-finish-path')
    print('\tdiff-finish-path: folder containing a bunch of folders with the following prefixes, identifying different finishes:')
    print('\t\t' + '; '.join(PREFIX_FIRST_DECODE.keys()))
    print()
    sys.exit(1)


def y_map(y_vals):
    return np.array(y_vals) #/ INCIDENT_COUNTS


def srt_help(s):
    # sort based on run number
    return int(s.split('run')[1].split('202')[0].replace('-', ''))


def main():
    try:
        if not os.path.exists(sys.argv[1]):
            die()
    except IndexError as e:
        die()

    radial_data = load_radial_data(sys.argv[1])
    fig, ax = plt.subplots()
    plot_func_rad(radial_data)

    fig.tight_layout()
    # fig.savefig('fixed.pdf')
    plt.show()


def load_radial_data(rad_dir: str) -> dict:
    folds = os.listdir(rad_dir)
    att_fn = None
    dat = dict()
    for pfx in PFX_MAP.keys():
        dat[pfx] = list()
        wanted = [fold for fold in folds if pfx in fold]
        wanted.sort(key=srt_help)
        print([srt_help(w) for w in wanted])
        for w in wanted:
            full_path_no_file = os.path.join(rad_dir, w)
            att_fn = att_fn or next(f for f in os.listdir(full_path_no_file) if 'si-out' in f)
            with open(os.path.join(full_path_no_file, att_fn)) as inf:
                dat[pfx].append(inf.read().count(os.linesep) + 1)
        print(pfx, dat[pfx])

    return dat


def plot_func_rad(dat: dict):
    fig, ax = plt.gcf(), plt.gca()
    for lab, cts in dat.items():
        if len(cts) == 0: continue
        # last run has truncated data
        num_plot = len(cts) #if len(cts) == len(RADIUSES) else (len(cts) - 1)
        ax.plot(RADIUSES[:num_plot], y_map(cts[:num_plot]), label=PFX_MAP[lab], marker='o', ms=8, alpha=0.8)

    ax.axhline(y=y_map(INCIDENT_COUNTS), label=f'{INCIDENT_COUNTS} launched opticals', color='k')
    ax.set_title('Counts vs launch radius, different finishes')
    ax.set_ylabel('SiPM counts')
    # ax.set_ylabel('Proportion of opticals detected')
    ax.set_xlabel('Radius along $x$ (mm)')
    ax.set_ylim(0, 55000)
    ax.legend()


if __name__ == '__main__': main()
