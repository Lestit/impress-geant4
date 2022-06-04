import numpy as np
import os
import sys
import subprocess
sys.path.append(os.path.dirname(__file__) + '/../..')
from optical_params_shared import NUM_RUNS, RADIUSES

MACRO_OUT_FN = 'macros/optical_position_run.mac'

G4_EXECUTABLE = 'impress'
G4_CFG_FN = 'impress.conf'
# yes turn on scintillation, only model detector, use C1-optimized attenuator
DEFAULT_INPUT = 'ydc1'

BACK_FINISH_KEY = 'cebr3-back-finish'
EDGE_FINISH_KEY = 'cebr3-edge-finish'
PREFIX_KEY = 'data-folder-prefix'
ROUGH_TEF = 'RoughTeflon_LUT'     # with teflon
POLISH_TEF = 'PolishedTeflon_LUT' # with teflon
POLISH = 'Polished_LUT'           # bare
ROUGH = 'Rough_LUT'               # bare
FINISH_PAIRS = [
    {'edge': ROUGH, 'back': ROUGH},
    {'edge': POLISH, 'back': POLISH},
]

# tell ImpEnergyPicker to use G4GeneralParticleSource
IMPE_INIT_CMD = '/impe/distributionType gps'
# format strings for G4GeneralParticleSource macro commands that we're gonna run
GPS_INIT_MACRO = 'optical_diagnose.mac'
GPS_INIT_CMD = '/control/execute macros/' + GPS_INIT_MACRO
GPS_POS_CMD_FMT = '/gps/pos/centre {:.3f} 0 3 mm'
GPS_RUN_CMD = f'/run/beamOn {NUM_RUNS}'


def generate_run_macro_file():
    with open(MACRO_OUT_FN, 'w') as f:
        print(IMPE_INIT_CMD, GPS_INIT_CMD, sep=os.linesep, file=f)
        for displacement in RADIUSES:
            print(file=f)
            print(GPS_POS_CMD_FMT.format(displacement), file=f)
            print(GPS_RUN_CMD, file=f)

    print('generated macro file', MACRO_OUT_FN)


def update_config_finish(back=ROUGH_TEF, edge=POLISH_TEF):
    props = dict()
    with open(G4_CFG_FN, 'r+') as cfg_file:
        for line in cfg_file:
            k, v = line.split()
            props[k] = v

        props[EDGE_FINISH_KEY] = edge
        props[BACK_FINISH_KEY] = back
        props[PREFIX_KEY] = f'e{edge}-b{back}'
        cfg_file.seek(0)
        cfg_file.write(os.linesep.join(f'{k} {v}' for (k, v) in props.items()))
        cfg_file.truncate()


def execute_run(back=ROUGH_TEF, edge=POLISH_TEF):
    update_config_finish(back=back, edge=edge)
    ret = subprocess.run([f'./{G4_EXECUTABLE}', MACRO_OUT_FN], input=DEFAULT_INPUT, text=True)
    print('completed:')
    print(ret)


def run_different_finishes():
    for fin in FINISH_PAIRS:
        execute_run(**fin)


def main():
    filez = os.listdir()
    if G4_EXECUTABLE not in filez or G4_CFG_FN not in filez:
        print('** ERROR run this script in the directory with the executible and macros folder after compiling it')
        exit(1)

    generate_run_macro_file()
    choice = str()
    while choice.lower() not in ('y', 'n'):
        choice = input('run the simulation? (y/n) ')
    if choice == 'y':
        run_different_finishes()


if __name__ == '__main__': main()
