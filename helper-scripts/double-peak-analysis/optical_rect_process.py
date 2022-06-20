import generate_rect_xz_scan as grs
import numpy as np
import re
import os
import scipy.ndimage as ndimage
import scipy.interpolate as interp

def resample_optical_data(dat):
    x, z, ctz = dat
    xx, zz = np.mgrid[np.min(x):np.max(x):1000j, np.min(z):np.max(z):1000j]

    smooth_sig = 2
    smooth = ndimage.gaussian_filter(ctz, sigma=smooth_sig, order=0)
    interpolated_grid = interp.griddata((x, z), smooth, (xx, zz), method='cubic')
    return {'x_mesh': xx, 'z_mesh': zz, 'counts': interpolated_grid}

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

def plot_marginal_xz(dat, fig, axs, label=None):
    axs = axs.flatten()
    assert axs.size == 2

    new = {'z': dat['z_mesh'][0], 'x': dat['x_mesh'][:,0]}
    marg = {
        k: dat['counts'].sum(axis=ax)
        for k, ax in zip('xz', (1, 0))
    }

    for i, (k, marg) in enumerate(marg.items()):
        d = (marg.max() - marg.min()) / marg.min()
        axs[i].plot(new[k], marg, label=f'{label} ($\Delta = {d * 100:.2f}$%)')
        # axs[i].set_title(f'(max - min) / min = {100 * d:.2f}%')
        axs[i].set_xlabel(f'Launch along ${k}$ (mm)')
        axs[i].set_ylabel('number of hits on SiPMs')
        axs[i].set_title(f'${k}$ hit intensity (integrate out ${"z" if k == "x" else "x"}$)')

    fig.suptitle('x and z marginal distributions')
    fig.tight_layout()

def plot_2d_distr(dat, fig, ax, label=None):
    surf = ax.plot_surface(dat['x_mesh'], dat['z_mesh'], dat['counts'], label=label)
    # https://stackoverflow.com/questions/54994600/pyplot-legend-poly3dcollection-object-has-no-attribute-edgecolors2d
    surf._facecolors2d=surf._facecolor3d
    surf._edgecolors2d=surf._edgecolor3d

    ax.set_xlabel('Launch x (mm)')
    ax.set_ylabel('Launch z (mm)')
    ax.set_zlabel('Counts')
    ax.set_title('Counts vs position')
