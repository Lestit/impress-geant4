import numpy as np
import matplotlib.pyplot as plt
import os
import sys

plt.style.use(os.getenv('MPL_DEFAULT_STYLE'))


def die():
    print(f"Usage: {os.path.basename(__file__)} input_spec cryst_spec")
    print("\tinput_spec: space-delimited text file with (keV, intensity) entries to overplot on the crystal spectrum")
    print("\tcryst_spec: Geant crystal spectra; multiple channels OK")
    sys.exit(1)


def plot_crystal_eng_list(energies, channels, bin_width=1):
    ''' returns max value for scaling later '''
    ax = plt.gca()
    dat = dict()
    for e, ch in zip(energies, channels):
        try: dat[ch].append(e)
        except KeyError: dat[ch] = [e]

    max_ = -1
    bins = np.arange(0, np.max(energies), bin_width)
    for ch, e_list in dat.items():
        n, _, _ = ax.hist(
            e_list, alpha=0.5, histtype='stepfilled',
            bins=bins, align='right', label=f'channel {int(ch)}')
        max_ = np.max(np.append(n, max_))

    return max_

def plot_spectrum_lines(engs, intens, scale_height, **mpl_kwds):
    ax = plt.gca()
    kwds = {
        'label': 'input spectrum', 'color': 'k',
        'linestyle': 'dotted', 'alpha': 1}
    kwds.update(mpl_kwds)
    ax.vlines(engs, ymin=0, ymax=intens * scale_height, **kwds)


def main():
    if len(sys.argv) != 3:
        die()

    spec_file, cryst_file = sys.argv[1:]
    fig, ax = plt.subplots()
    eng, chan = np.loadtxt(cryst_file, unpack=True)
    spec_eng, spec_intens = np.loadtxt(spec_file, unpack=True)

    max_val = plot_crystal_eng_list(eng, chan, bin_width=1)

    lab = os.path.basename(spec_file)
    lab = lab[:lab.index('.')].title()
    scale_height = eng.size / 2
    plot_spectrum_lines(
        spec_eng, spec_intens, scale_height, label=lab,
        alpha=0.4)

    ax.set_xlabel('Energy (keV)')
    ax.set_ylabel('Counts or scaled X-ray spectrum')
    ax.set_title('Geant4 crystal edep + X-ray spectrum')

    ax.legend()
    fig.tight_layout()
    plt.show()

if __name__ == '__main__': main()
