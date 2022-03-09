import hist_counts_with_positions as hcp
import matplotlib.pyplot as plt
import numpy as np
import os
import scipy.stats as st
import sys

plt.style.use('/Users/settwi/mpl-styles/fig.mplstyle')

def slice_disp(s):
    return ' '.join(s.split('-')[:-1])

def mean_iqr_fmt(s, iqr=0):
    return f'$r = {slice_disp(s).split()[0]}$ cm'

def cts_fmt(s):
    return slice_disp(s)

if len(sys.argv) == 1:
    print('pass in folders that contain "*-si-out.tab" files to histogram')
    exit(1)

fig, (hist_ax, trend_ax) = plt.subplots(nrows=1, ncols=2)
# fig, trend_ax = plt.subplots()
col_headers = [
    'displacement',
    'median counts',
    'mean counts',
    'mode counts',
    'stddev of counts',
    'iqr of counts',
    'skewness of counts',
    'tailedness (kurtosis) of counts'
]
head_lens = [len(x) for x in col_headers]
with open('summary_stats.txt', 'w') as f:
    print('some summary statistics', file=f)
    header_fmt = '| ' + ''.join(f'{{:^{w}}} | ' for w in head_lens)
    head = header_fmt.format(*col_headers)
    print(head, file=f)
    print('-' * len(head), file=f)

    # dat_fmt = f'| {{:^{head_lens[0]}}} | ' + ''.join(f'{{:^{w}.2f}} | ' for w in head_lens[1:])
    for folder in sys.argv[1:]:
        fn = next(f for f in os.listdir(folder) if '-si-out.tab' in f)
        cts = hcp.slice_counts(os.path.join(folder, fn))

        plt.sca(hist_ax)
        hcp.mean_iqr_cts(cts, label=mean_iqr_fmt(folder))
        # for not 2x plots
        # _ = next(hcp.COLORZ)
        # hcp.hist_cts(cts, label=cts_fmt(folder))

        plt.sca(trend_ax)
        hcp.mean_counts_scatter(cts, label=cts_fmt(folder))

        # sd = np.std(cts)
        # iqr = np.diff(np.quantile(cts, [0.25, 0.75]))[0]
        # med = np.median(cts)
        # mu = np.mean(cts)
        # skew = st.skew(cts)
        # kurt = st.kurtosis(cts)
        # distance_str = slice_disp(folder)
        # print(dat_fmt.format(distance_str, med, mu, st.mode(cts).mode[0], sd, iqr, skew, kurt), file=f)

ylimz = (5e1, 4e3)
for ax in (hist_ax, trend_ax):
    ax.set_yscale('log')
    ax.set_ylim(*ylimz)

hist_ax.set_title(r'Mean # of collected photons + peak width')
hist_ax.set_xlabel('Number of optical detections')
hist_ax.set_ylabel('Relative intensity (arb. units)')
hist_ax.legend(loc='upper left')

trend_ax.set_title('Mean # of collected photons')
trend_ax.set_xlabel('Mean # of optical detections')
trend_ax.set_ylabel('Intensity (arb. units)')
trend_ax.set_yscale('log')
trend_ax.legend()

fig.suptitle('10000 30keV photons incident')
sz = (32, 12)
fig.set_size_inches(list(1.2 * x for x in sz))
fig.tight_layout()
fig.savefig('iqr-plus-scatter.png')
# plt.show()
