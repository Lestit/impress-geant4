import ast
import numpy as np
import matplotlib.pyplot as plt
import os
import sys

def die():
    print("\tusage:", os.path.basename(__file__), "[crystal hits file] [title] . . .")
    print("\t[crystal hits file]: file containing the hits with channel + position information for crystal detector (not SiPM)")
    print("\t[title]: title of the plot")
    print("\trepeat until done")
    sys.exit(1)

def load_data(fn):
    es, cs, ps = [], [], []
    with open(fn, 'r') as f:
        for line in f:
            energy, channel, *positions = line.split()
            es.append(float(energy))
            cs.append(float(channel))
            ps.append([ast.literal_eval(p) for p in positions])
    return es, cs, ps

def make_marginal_plots(hitz, axs):
    crd = 'xyz'
    colors = 'rgb'

    for idx in range(3):
        positions = [h[idx] for h in hitz]
        axs[idx].hist(
            positions, bins=100, histtype='step',
            label=f'{crd[idx]} distribution',
            color = colors[idx]
        )
        axs[idx].set(
            ylabel='Counts',
            xlabel='Position along ' + crd[idx])
        axs[idx].legend(loc='upper center')
        axs[idx].invert_xaxis()

def main():
    if len(sys.argv) < 2:
        die()

    col_files, col_titles = sys.argv[1::2], sys.argv[2::2]
    num_cols = len(col_files)
    fig, axs = plt.subplots(
        nrows=3, ncols=num_cols,
        figsize=(num_cols * 4, 6)
    )
    for c in range(num_cols):
        _, _, hit_positions = load_data(col_files[c])
        make_marginal_plots(hit_positions, axs[:, c])
        axs[0, c].set_title(col_titles[c])
        print('done', col_titles[c])

    fig.suptitle('Crystal hit position distribution for different incident energies')
    fig.tight_layout()
    plt.show()


if __name__ == '__main__': main()
