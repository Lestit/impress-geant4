import matplotlib as mpl
import matplotlib.pyplot as plt
import numpy as np
import sys

# plt.style.use('/Users/settwi/mpl-styles/fig.mplstyle')
SPACING = 0.25
WIDTH = 3.88
DELTA = 1e-2
START = -(3*SPACING + 4*WIDTH) / 2
SIPM_EDGES = [
    (x - DELTA, x + WIDTH + DELTA) for x in np.arange(START, -START, WIDTH+SPACING)
]

'''
change me
'''
NUM_BINS_PER_PIXEL = 10
SIPM_BINS = np.array([
    np.linspace(e[0], e[1], NUM_BINS_PER_PIXEL) for e in SIPM_EDGES
]).flatten()

def main():
    file = sys.argv[1]
    positions = {k: list() for k in 'xy'}
    with open(file, 'r') as f:
        for line in f:
            cur_positions = line.split(' ')[1:]
            for tup in cur_positions:
                x, y, _ = tup.split(',')
                positions['x'].append(float(x[1:]))
                positions['y'].append(float(y))

    print('number of opticals detected', len(positions['x']))
    one_dim_hist(positions)
    # fpath = '/Users/settwi/Desktop/1dh.png'
    # plt.gca().set_ylim(0.055)
    # plt.gcf().savefig(fpath)
    two_dim_hist(positions)
    # fpath = '/Users/settwi/Desktop/2dh.png'
    # plt.gcf().savefig(fpath)
    # print('saved to', fpath)


def one_dim_hist(positions):
    fig, ax = plt.subplots(figsize=(8, 6))
    cols = ('blue', 'orange')
    for c, (k, v) in zip(cols, positions.items()):
        np_version = np.array(v)
        ax.hist(
            np_version, bins=SIPM_BINS, label=f'{k} hits', density=False,
            histtype='step', alpha=0.5, color=c, linewidth=2
        )
    ax.set_xlabel('Position along x or y (mm)')
    ax.set_ylabel('Density (cts/bin / cts/bin)')
    ax.set_title('William detections')#'X or Y positions on the silicon detector')
    ax.legend()
    # fig.set_size_inches(12, 6)
    fig.tight_layout()
    plt.show()


def two_dim_hist(positions):
    fig, ax = plt.subplots()
    # xy = bins[:-1] + np.diff(bins)/2
    counts, _, _, im = ax.hist2d(positions['x'], positions['y'], bins=SIPM_BINS, density=False)
    # ax.contour(xy, xy, counts, levels=[20, 40, 60, 80], colors=['black', 'red', 'pink', 'blue'])
    ax.set_xlabel('x position (mm)')
    ax.set_ylabel('y position (mm)')
    ax.set_title('2D hist of hits')

    fig.colorbar(im)
    fig.set_size_inches(7, 6)
    fig.tight_layout()
    plt.show()

if __name__ == '__main__': main()
