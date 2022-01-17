import numpy as np
import matplotlib.pyplot as plt
import sys

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

    # one_dim_hist(pos)
    two_dim_hist(positions)

def one_dim_hist(positions):
    fig, ax = plt.subplots()
    bins = np.linspace(-9, 9, 1000)
    for k, v in positions.items():
        np_version = np.array(v)
        ax.hist(
            np_version, bins=bins, label=f'{k} position distribution', density=False,
            histtype='step', alpha=0.6
        )
    ax.set_xlabel('Position along x or y (G4 native units)')
    ax.set_ylabel('Frequency')
    ax.set_title('X and Y positions on the silicon detector')
    ax.legend()
    fig.set_size_inches(8, 6)
    fig.tight_layout()
    plt.show()

def two_dim_hist(positions):
    fig, ax = plt.subplots()
    bins = np.linspace(-9, 9, 1000)
    ax.hist2d(positions['x'], positions['y'], bins=bins)
    ax.set_xlabel('Position along x in G4 native units')
    ax.set_ylabel('Position along y in G4 native units')
    ax.set_title('2D histogram of detected photons')
    fig.set_size_inches(6, 6)
    fig.tight_layout()
    plt.show()

if __name__ == '__main__': main()
