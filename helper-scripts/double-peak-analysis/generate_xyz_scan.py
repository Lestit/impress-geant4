import numpy as np
from datetime import datetime

pfx = datetime.now().strftime('%c').replace(' ', '-').replace(':', '.') + '_'
MACRO_OUT_FN = pfx + 'optical-xyz-scan.mac'
POSITIONS_OUT_FN = pfx + 'positions-xyz-scan.txt'

ATTENUATOR_THICK = 0
LENGTH = 5
LG_THICK = 8
# computed after playing around with detector spacing etc
Z_ANCHOR = LG_THICK/2 - 1/4 - ATTENUATOR_THICK/2
Z_START = Z_ANCHOR - LENGTH/2
Z_END = Z_START + LENGTH

NUM_R = 10
NUM_THETA = 10
NUM_Z = 10
RADIUS = 37/2 # mm
NUM_PHOTS = 10000

script_begin =\
f'''/gps/particle opticalphoton

/gps/pos/type Point

/gps/ang/type iso
/gps/polarization 1 0 0

/gps/ene/type Mono
/gps/ene/mono 3 eV

/run/printProgress {NUM_PHOTS//5}
'''

def pozition():
    ''' cylindrical position iterator over a quarter of the cylinder '''
    # account for degeneracy (specifically at r = 0)
    cache = []
    for r in np.linspace(0, RADIUS - 0.25, num=NUM_R):
        for theta in np.linspace(0, np.pi / 2, num=NUM_THETA):
            for z in np.linspace(Z_START + 0.25, Z_END - 0.25, num=NUM_Z):
                p = (r * np.cos(theta), r * np.sin(theta), z)
                if p in cache: continue
                cache.append(p)
                yield p

if __name__ == '__main__':
    with open(MACRO_OUT_FN, 'w') as macro_file, open(POSITIONS_OUT_FN, 'w') as pos_file:
        print(script_begin, file=macro_file)
        for (x, y, z) in pozition():
            print(f'/gps/pos/centre {x:.2f} {y:.2f} {z:.2f} mm', file=macro_file)
            print(f'/run/beamOn {NUM_PHOTS}\n', file=macro_file)
            print(f"{x:.2f} {y:.2f} {z:.2f}", file=pos_file)
