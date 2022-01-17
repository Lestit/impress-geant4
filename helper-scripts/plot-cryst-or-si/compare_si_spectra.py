import itertools
import numpy as np
import matplotlib.pyplot as plt
import matplotlib as mpl
import scipy.stats as st
import os
import sys

# try:
#     import traceback
#     plt.style.use('/Users/settwi/grad_school/glesener/agu-2021/agu.mplstyle')
# except Exception as e:
#     traceback.print_exc()
#     input("any key to continue")
#     raise

# weird config
colorz = itertools.cycle('bkrgmcy')
just_spec_fn = 'ba133.txt'
try:
    input_spec_file = os.path.join(
        os.path.dirname(__file__), just_spec_fn)
except (NameError, TypeError):
    input_spec_file = None
do_fit = False
density_hist = False
bin_width = 1
boxcar_width = 1
low_cutoff = 5
# manual fit params
man_start, man_end = None, None #7000, 10000
title_override = None #f"Geant4 ${{}}^{{133}}$Ba (boxcar = {boxcar_width})"
label_override = None #'Optical photon detections'

if len(sys.argv) < 4:
    print("Usage: compare_crystal_spectra incident_energy num_spectra [spec1] [spec2] ... (optional: gain calibration intercept, slope)")
    print("NB: assumes only one crystal channel")
    print("NB: incident energy in keV")
    exit(1)

# for fitting purposes later on
COUNTS_CUTOFF = 10
inc_e = sys.argv[1]
n_spec = int(sys.argv[2])
END_START = 3
end_arg = END_START + n_spec
files = sys.argv[END_START:end_arg]
try:
    intercept, slope = (float(x) for x in sys.argv[end_arg:])
    print(f'line is E = {intercept} + {slope:.2f}ch')
except ValueError:
    print("no gain calibration intercept or slope provided.")
    intercept, slope = None, None

low, up = 1e9, 0
si_counts = list()
for f in files:
    cnts = np.loadtxt(f, unpack=True)
    si_counts.append(cnts)
    up = max(np.max(cnts), up)
    low = min(np.min(cnts), low)

input_e, input_intens = np.loadtxt(input_spec_file, unpack=True) if input_spec_file else (None, None)

fig, ax = plt.subplots()
ax.set_ylabel('$\\propto$ number of events')
if intercept is None:
    ax.set_xlabel('Number of optical photons detected')
else:
    ax.set_xlabel('Calibrated energy (keV)')

all_bins = np.arange(low - 1, up + 1, step=bin_width)
start_xlim, end_xlim = 1e5, 0
for i, cnts in enumerate(si_counts):
    cnts = cnts[cnts > low_cutoff]
    bn = os.path.basename(files[i])
    kept = bn.replace('.tab', '').split('-')
    try:
        eff = next(k for k in kept if '.' in k)
        eff = float(eff)
    except StopIteration:
        eff = 1

    nice_name = '-'.join(kept)
    lab = f'{nice_name} | {cnts.size} optical bursts total, ' +\
          f'{100 * eff:.1f}% "detector efficiency"'
    hg_cnts, _ = np.histogram(
        cnts, bins=all_bins, density=density_hist)

    plot_bins = all_bins[:-1] + np.diff(all_bins)
    if intercept is not None:
        plot_bins = plot_bins * slope + intercept
    smooth_cnts = np.convolve(np.ones(boxcar_width) / boxcar_width, hg_cnts, mode='same')
    print(smooth_cnts)

    cur_col = next(colorz)
    # ax.hist(cnts, bins=all_bins, label=lab, alpha=0.5, color=cur_col, edgecolor=cur_col)
    ax.plot(plot_bins, smooth_cnts, label=(label_override or lab), alpha=0.5, color=cur_col)
    ax.fill_between(
        x=plot_bins, y1=np.zeros_like(plot_bins), y2=smooth_cnts, color=cur_col, alpha=0.2)
    keep = plot_bins[hg_cnts > COUNTS_CUTOFF]
    fit_start, fit_end = (man_start or np.min(keep) - 1), (man_end or np.max(keep) + 1)
    start_xlim, end_xlim = min(start_xlim, fit_start), max(end_xlim, fit_end)

    if input_e is not None:
        ax.vlines(
            input_e, ymin=0, ymax=input_intens * np.max(smooth_cnts) / np.max(input_intens),
            color='k', linestyle='dotted', alpha=1, label='Input spectrum')

    if do_fit:
        mu, sig = st.norm.fit(cnts[np.logical_and(cnts > fit_start, cnts < fit_end)])
        fwhm = 2 * np.sqrt(2 * np.log(2)) * sig
        ax.plot(
            all_bins, st.norm.pdf(all_bins, mu, sig),
            label=f'{nice_name} normal fit: $(\\mu, \\sigma, \\mathrm{{FWHM}}) = ({mu:.2f}, {sig:.2f}, {fwhm/mu * 100:.1f}\\%)$',
            alpha=0.8)

s, e = ax.get_xlim()
if man_start is None:
    ax.set_xlim(min(s, start_xlim * 0.9), max(e, end_xlim * 1.05))
ax.set_ylim(np.array(ax.get_ylim()) * 1.1)
fwhm_str = "; $\\mathrm{FWHM} = \\left(2 \\sqrt{2 \\log 2}\\right) \\sigma / \mu$." if do_fit else ""
ax.set_title(title_override or f'Optical detections @ {inc_e} keV incident')
ax.legend(loc='upper center')

ax.set_yscale('log')
ax.set_ylim(0.8, ax.get_ylim()[1])
fig.tight_layout()
plt.show()
