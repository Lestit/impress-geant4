import numpy as np
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit
import sys


def main():
    if len(sys.argv) != 3:
        print("Usage:")
        print("\tgain_calibrate.py input_spectrum_file geant_output_file")
        exit(1)

    # set to pick out the indices to find the maxima.
    peak_slices = [[1550, 1750], [4100, 4550]]

    in_energies, in_intensities = np.loadtxt(sys.argv[1], unpack=True)
    geant_counts = np.loadtxt(sys.argv[2])
    # set these to "calibrate the gain"
    sorted_in_energies = list(reversed(in_energies[np.argsort(in_intensities)]))
    for i, e in enumerate(sorted_in_energies):
        print(f'{e:<10.3f} ({i})')
    idxs = input("Which energies do you want to use for alignment (type indices as 'i j'): ")
    alignment_peaks = [sorted_in_energies[int(x)] for x in idxs.strip().split()]

    slope, intercept = calibrate_gain(alignment_peaks, geant_counts, peak_slices, debug_plot=True)
    print(f'line is: E = {slope:.5f}(bin) + {intercept:.5f}')


def calibrate_gain(real_peaks, raw_geant, geant_slices, boxcar=50, debug_plot=False):
    bins = np.arange(np.max(raw_geant) + 1)
    hist_vals, edges = np.histogram(raw_geant, bins=bins)

    # smoothing
    kernel = np.ones(boxcar) / boxcar
    smoothed = np.convolve(hist_vals, kernel, mode='same')

    centers = []
    for psl in geant_slices:
        centers.append(fit_peak_slice(smoothed[slice(*psl)]))

    # pin the line to the given values
    slope = np.abs(np.diff(real_peaks) / np.diff(centers))[0]
    intercept = real_peaks[0] - centers[0] * slope

    if debug_plot:
        modded_bins = intercept + slope * bins[:-1]
        plt.plot(modded_bins, smoothed)
        plt.xlabel('energy keV')
        plt.ylabel('g4 counts')
        plt.show()

    return slope, intercept


def gaussian_shape(x, amp, shift, width, m, b):
    line = 0#m * x + b
    exp = amp * np.exp(-(x - shift) * (x - shift) / width)
    return line + exp


def lorentzian_shape(x, amp, shift, sharp):
    return amp / ((x - shift) * (x - shift) + sharp)


def fit_peak_slice(slc):
    crds = np.arange(slc.size)
    plt.plot(crds, slc)
    popt, pcov = curve_fit(lorentzian_shape, slc, crds)
    plt.plot(crds, lorentzian_shape(crds, *popt))
    plt.show()
    plt.close()
    print(popt, pcov)
    input()

if __name__ == '__main__':
    main()
