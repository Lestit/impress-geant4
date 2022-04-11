from mpl_toolkits import mplot3d
import matplotlib.pyplot as plt
import matplotlib as mpl
import numpy as np
import os
import sys

LAUNCHED = 50000
DO_3D = True
SCATTER_CMAP = 'plasma'
SCATTER_CMAP_NORM = plt.Normalize(vmin=0, vmax=LAUNCHED)
LINE_COLOR = 'blue'

def extract_params_dict(folder_name: str):
    full_folder = os.path.join(sys.argv[1], folder_name)
    counts_fn = next(f for f in os.listdir(full_folder) if 'si-out' in f)
    full_counts_fn = os.path.join(full_folder, counts_fn)
    with open(full_counts_fn, 'r') as f:
        cts = len(f.read().split('\n'))

    ret = {'counts': cts}
    space, lg  = folder_name.split('_')[:-1]
    for cut, ary in zip(('si-space', 'lg-thick'), (space, lg)):
        ret[cut] = float(ary[len(cut):][:-2])

    return ret

def cts_space_lgthk(folds: list):
    runs = [extract_params_dict(f) for f in folds]
    counts = [r['counts'] for r in runs]
    space = [r['si-space'] for r in runs]
    lg_thick = [r['lg-thick'] for r in runs]
    return {'counts': counts, 'si-space': space, 'lg-thick': lg_thick}

def plot_2d_vary_params(folders: list, cbar: bool=True, yaxis: bool=True):
    cleaned = cts_space_lgthk(folders)

    ax = plt.gca()

    path_coll = ax.scatter(
        cleaned['lg-thick'], cleaned['si-space'], s=60,
        c=counts, cmap=SCATTER_CMAP, norm=SCATTER_CMAP_NORM, zorder=1)

    xmid = np.diff(ax.get_xlim()) / 2
    ymid = np.diff(ax.get_ylim()) / 2

    x, y = cleaned['lg-thick'], cleaned['si-space']
    for i, cts in enumerate(counts):
        ann = f'{cts} hits'
        offset = (
            -50 if x[i] > xmid else 10,
            -20 if y[i] > ymid else 10)
        xy = np.array((x[i], y[i]))
        _ = ax.annotate(
            text=ann, xy=xy,
            textcoords='offset points',
            xytext=offset
        )

    if cbar:
        cb = plt.gcf().colorbar(path_coll, ax=ax)
        cb.set_label('SiPM counts')
    if yaxis:
        ax.set_ylabel('SiPM spacing (mm)')

    ax.set_xlabel('Light guide thickness (mm)')


def plot_surface_vary_params(folder_names: list):
    dat = cts_space_lgthk(folder_names)
    ax = plt.gca()
    s = ax.plot_trisurf(
        dat['lg-thick'],
        dat['si-space'],
        dat['counts'],
        cmap=SCATTER_CMAP,
        norm=SCATTER_CMAP_NORM
    )
    ax.set_xlabel('Light guide thickness (mm)')
    ax.set_ylabel('SiPM spacing (mm)')
    ax.set_zlabel('SiPM counts')
    ax.set_zlim(0, LAUNCHED)
    cb = plt.gcf().colorbar(s, ax=ax)
    cb.set_label('SiPM counts')

def main():
    try:
        analysis_folder = sys.argv[1]
    except IndexError:
        print()
        print(f'\tUsage: {os.path.basename(__file__)} (directory with all the varied light guide stuff)')
        print()
        sys.exit(69)

    all_folders = [f for f in os.listdir(analysis_folder) if os.path.isdir(f)]
    split_on_lg = dict()
    split_on_lg['lightguide-yes-wrap'] = [f for f in all_folders if 'yes' in f]
    split_on_lg['lightguide-no-wrap'] = [f for f in all_folders if 'no' in f]


    fig = plt.figure(figsize=(8, 6))
    ax = fig.add_subplot(111, projection=('3d' if DO_3D else None))
    for i, (k, v) in enumerate(split_on_lg.items()):
        plt.sca(ax)

        wrap_desc = 'WITH' if 'yes' in k else 'WITHOUT'
        ax.set_title(f'{LAUNCHED} opticals $\\rightarrow$ {wrap_desc} light guide wrapping')

        # plot_2d_vary_params(v)
        plot_surface_vary_params(v)
        fig.tight_layout()
        # fig.savefig(f'lightguide-sipm-{k}.pdf')
    plt.show()


if __name__ == '__main__': main()
