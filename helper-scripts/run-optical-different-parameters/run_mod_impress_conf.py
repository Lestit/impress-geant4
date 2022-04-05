import os
import subprocess
import sys

G4_EXECUTABLE = 'impress'
CONFIG_FN = 'impress.conf'
# yes turn on scintillation, only model detector, use C1-optimized attenuator
DEFAULT_INPUT = 'ydc1'

def update_conf(new_settings):
    if not os.path.exists(CONFIG_FN):
        raise RuntimeError('Run this in the executible directory (where impress.conf lives)')

    settingz = dict()
    with open(CONFIG_FN, 'r') as f:
        for line in f:
            k, v = line.split()
            settingz[k] = v

    settingz.update(new_settings)
    with open(CONFIG_FN, 'w') as f:
        print(os.linesep.join(f'{k} {v}' for (k, v) in settingz.items()), file=f)


def run_macro(macro_fn, input=DEFAULT_INPUT):
    ret = subprocess.run([f'./{G4_EXECUTABLE}', macro_fn], input=input, text=True)
    return ret
