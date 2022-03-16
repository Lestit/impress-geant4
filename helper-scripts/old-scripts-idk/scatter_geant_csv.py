import numpy as np
import matplotlib.pyplot as plt
import sys

import common_csv

def scatter_channels(channels, run_type, maxe, loglin):
    fig, ax = plt.subplots()
    common_csv.common_plot_cfg(fig, ax)

def main():
    if len(sys.argv) < 2:
        print('usage: scatter_geant_csv (input file)')
        exit(1)
    channels, run_type, maxe, ll = common_load_csv.common_load()
    scatter_channels(channels, run_type, maxe, ll)
