import matplotlib.pyplot as plt
import numpy as np
import json
import os
import scipy.stats as st
import sys

NUM_ROI_DRAWS = int(1e6)
ROI_BOUNDS = [2000, 3000]

class FwhmEstimator:
    NUM_KDE_EVAL_PTS = 1000
    def __init__(self, data: dict, verbose=False):
        self.dat = {k: np.array(v) for k, v in data.items()}
        self.verbose = verbose
        self.roi_samples = None
        self.kde_eval_pts = None
        self.kde_xpts = None
        self.kde_ypts = None
        self.half_maxes = np.empty(2, dtype=int)
        self.max = None

    def estimate_fwhm(self):
        self.sample_roi()
        self.compute_kde()
        self.compute_fwhm_params()

    def sample_roi(self):
        if self.verbose: print('sampling from ROI')
        cutout = slice(*ROI_BOUNDS)
        roi = np.array(self.dat['histo_entries'])[cutout]
        roi_pmf = roi / roi.sum()

        centers = (self.dat['bin_edges'][:-1] + self.dat['bin_edges'][1:])/2
        self.roi_samples = np.random.choice(
            centers[cutout],
            size=NUM_ROI_DRAWS, p=roi_pmf)

    def compute_kde(self):
        if self.verbose: print('computing kde')
        edges = self.dat['bin_edges']
        roi_kde = st.gaussian_kde(self.roi_samples)
        self.kde_xpts = np.linspace(np.min(edges), np.max(edges), num=self.NUM_KDE_EVAL_PTS)
        self.kde_ypts = roi_kde(self.kde_xpts)

    def compute_fwhm_params(self):
        if self.verbose: print('finding fwhm parameters')
        max_kde_xidx = np.argmax(self.kde_ypts)
        first_slice = slice(int(max_kde_xidx * 0.5), max_kde_xidx)
        second_slice = slice(max_kde_xidx, int(1.5 * max_kde_xidx))

        pre_half_maxes = np.array(self.half_maxes)
        for i, sl in enumerate((first_slice, second_slice)):
            hm_x_loc = np.argmin(
                np.abs(self.kde_ypts[sl] - self.kde_ypts[max_kde_xidx]/2))
            pre_half_maxes[i] = sl.start + hm_x_loc

        self.half_maxes = self.kde_xpts[pre_half_maxes] #+ ROI_BOUNDS[0]
        self.max = self.kde_xpts[max_kde_xidx]


def load_exported(fn):
    with open(fn, 'r') as f:
        return json.loads(f.read())


def plot_find_regions(dat):
    fig, ax = plt.subplots(figsize=(8, 6))
    ax.stairs(dat['histo_entries'], dat['bin_edges'])
    fig.tight_layout()
    plt.show()


def display_hist_data(dat, density=False, bin_shift=0, gain_cal=None, edges=None):
    dat = np.array(dat, dtype=np.float64)
    if density: dat /= dat.sum()

    if edges is None:
        edges = bin_shift + np.arange(0, dat.size + 1)
    if gain_cal is not None:
        edges = gain_cal(edges)

    fig, ax = plt.gcf(), plt.gca()

    ax.stairs(dat, edges=edges, label='histogram data')

    if gain_cal is None:
        xlab = 'adc bin'
    else: xlab = 'calibrated energy (keV)'
    ax.set_xlabel(xlab)
    ax.set_ylabel(f'count{" density" if density else "s"}')

    fig.tight_layout()
    return edges


def plot_fwhm(fwhm_est):
    fig, ax = plt.subplots(figsize=(9, 4))
    display_hist_data(
        fwhm_est.dat['histo_entries'],
        density=True, gain_cal=None,
        edges=fwhm_est.dat['bin_edges'])

    ax.plot(
        fwhm_est.kde_xpts,
        fwhm_est.kde_ypts,
        label='kernel density estimator'
    )
    ax.vlines(
        fwhm_est.half_maxes, ymin=0,
        ymax=0.5*np.max(fwhm_est.kde_ypts), alpha=1,
        label='half max', color='green', linewidth=2
    )
    ax.vlines(
        fwhm_est.max, ymin=0, ymax=np.max(fwhm_est.kde_ypts),
        alpha=1, label=f'$E_\\mu = {fwhm_est.max:.1f}$ adc bin',
        color='black', linewidth=2
    )
    ax.legend()

    just_fn = os.path.basename(sys.argv[1])
    fwhm_val = (np.diff(fwhm_est.half_maxes) / fwhm_est.max)[0]
    ax.set_title(f'{just_fn} $\\rightarrow$ FWHM = $\\Delta E / E_\\mu = {100 * fwhm_val:.2f}\\%$')

    fig.tight_layout()
    extension = '.pdf'
    fig.savefig(f'{just_fn}{extension}')
    print(f'saved {just_fn}{extension}')
    # plt.show()


def main():
    try:
        data = load_exported(sys.argv[1])
    except IndexError:
        print('Supply a file name of processed G4 JSON containing keys "bin_edges" and "histo_entries".')
        sys.exit(1)


    # plot_find_regions(data)
    input(f'Using ROI {ROI_BOUNDS}; ok? ')
    estimator = FwhmEstimator(data, verbose=True)
    estimator.estimate_fwhm()
    hms, m = estimator.half_maxes, estimator.max
    fwhm = np.diff(hms) / m
    print('estimated fwhm for the given ROI bounds:', fwhm[0] * 100, '%')

    plot_fwhm(estimator)

if __name__ == '__main__':
    main()
