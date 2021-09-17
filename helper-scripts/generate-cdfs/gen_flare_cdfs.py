import numpy as np
import os
import matplotlib.pyplot as plt

DIAGNOSTIC = False

def plt_diagnostic(eng, cdf):
    fig, ax = plt.subplots()
    ax.plot(eng, cdf)
    ax.set_xlabel('energy in keV')
    ax.set_ylabel('cumulative density function')
    ax.set_title('cdf diag')
    ax.set_xscale('log')
    ax.set_yscale('log')
    fig.tight_layout()
    plt.show()
    plt.close('all')


def cdf_from_prob(ary, up_to_idx):
    return np.sum(ary[:up_to_idx])


def main():
    raw_dir = '../raw-flare-data'
    cdf_dir = 'flare-cdfs'
    for f in os.listdir(raw_dir):
        full_f = os.path.join(raw_dir, f)
        eng, intens = np.loadtxt(full_f, unpack=True)
        intens = intens / np.sum(intens)
        print(np.sum(intens))
        cdf = np.array([cdf_from_prob(intens, i) for i in np.arange(intens.size)])
        if DIAGNOSTIC:
            plt_diagnostic(eng, cdf)

        flare_size = f.split('_')[1][:2]
        np.savetxt(os.path.join(cdf_dir, f'cdf_{flare_size}.txt'), np.transpose([eng, cdf]), fmt=('%.3f', '%.64f'))

if __name__ == '__main__': main()
