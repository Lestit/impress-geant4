import generate_rect_xz_scan as grs
import matplotlib.pyplot as plt
from mpl_toolkits import mplot3d
import numpy as np
import re
import os
import scipy.ndimage as ndimage
import scipy.interpolate as interp
import sys

smooth = True
smooth_sig = 2

def main():
    fold = sys.argv[1]
    x, z, num_counts = extract_optical_data(fold)
    plot_2d_surf(x, z, num_counts)
    plt.gcf().tight_layout()
    plt.show()

def plot_2d_surf(x, z, num_counts):
    fig = plt.figure()
    ax = fig.add_subplot(projection='3d')
    if not smooth:
        ax.plot_trisurf(x, z, num_counts)
    else:
        smoothed = ndimage.gaussian_filter(num_counts, sigma=smooth_sig, order=0)
        ax.plot_trisurf(x, z, smoothed)
    ax.set_xlabel('Position along $x$ (mm)')
    ax.set_ylabel('Position along $z$ (mm)')
    ax.set_zlabel('Number of SiPM hits')
    fig.suptitle(f'{grs.NUM_PHOTS} launched')

def extract_optical_data(containing_folder, data_fn='c1-si-out.tab'):
    run_folders = os.listdir(containing_folder)
    # sort by run number
    # regex to match run#... where #... means 'at least one numeric character'
    key = lambda f: int(re.findall('run([0-9]+)', f)[0])
    run_folders.sort(key=key)

    ret = []
    for (x, z), fold in zip(grs.pozition(), run_folders):
        with open(os.path.join(containing_folder, fold, data_fn)) as f:
            num_hits = f.read().count('\n')
        ret.append([x, z, num_hits])

    return np.array(ret).T

if __name__ == '__main__':
    main()
