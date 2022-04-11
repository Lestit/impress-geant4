import os
import subprocess
import sys
import traceback as tb

G4_EXECUTABLE = 'impress'
CONFIG_FN = 'impress.conf'
# yes turn on scintillation, only model detector, use C1-optimized attenuator
DEFAULT_INPUT = 'ydc1'

DEFAULT_CONFIG = {
    'save-si-positions': 1,
    'save-si-energies': 0,
    'air-gap-thickness': 10, # um
    'cebr3-length': 5, # mm
    'cebr3-edge-finish': 'Rough_LUT',
    'cebr3-back-finish': 'Rough_LUT',
    'optical-photon-draw-freq': 1,
    'data-folder-prefix': 'default',
    'si-spacing': 0.2, # mm
    'light-guide-thickness': 8, # mm
    'build-light-guide-reflector': 1,
    'scint-time-const': 20 # ns
}

def update_conf(new_settings):
    if not os.path.exists(CONFIG_FN):
        raise RuntimeError('Run this in the executible directory (where impress.conf lives)')
    to_update = dict(DEFAULT_CONFIG).update(new_settings)

    settingz = dict()
    with open(CONFIG_FN, 'r') as f:
        for line in f:
            try:
                k, v = line.split()
                settingz[k] = v
            except ValueError as e:
                tb.print_exc()
                continue

    settingz.update(new_settings)
    with open(CONFIG_FN, 'w') as f:
        print(os.linesep.join(f'{k} {v}' for (k, v) in settingz.items()), file=f)


def run_macro(macro_fn, input=DEFAULT_INPUT):
    ret = subprocess.run([f'./{G4_EXECUTABLE}', macro_fn], input=input, text=True)
    return ret
