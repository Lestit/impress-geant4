import numpy as np
from datetime import datetime

pfx = datetime.now().strftime('%c').replace(' ', '-').replace(':', '.') + '_'
MACRO_OUT_FN = pfx + 'optical-xyz-scan.mac'
POSITIONS_OUT_FN = pfx + 'positions-xyz-scan.txt'

LENGTH = 5
Z_START = 3.75 - LENGTH/2
Z_END = 3.75  + LENGTH/2
NUM_Z = 10
NUM_X = 10
NUM_Y = 10
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
    max_xy = RADIUS / np.sqrt(2)
    for x in np.linspace(0, max_xy - 0.5, num=NUM_X):
        for y in np.linspace(0, max_xy - 0.5, num=NUM_Y):
            for z in np.linspace(Z_START + 0.5, Z_END - 0.5, num=NUM_Z):
                yield (x, y, z)

if __name__ == '__main__':
    with open(MACRO_OUT_FN, 'w') as macro_file, open(POSITIONS_OUT_FN, 'w') as pos_file:
        print(script_begin, file=macro_file)
        for (x, y, z) in pozition():
            print(f'/gps/pos/centre {x:.2f} {y:.2f} {z:.2f} mm', file=macro_file)
            print(f'/run/beamOn {NUM_PHOTS}\n', file=macro_file)
            print(f"{x:.2f} {y:.2f} {z:.2f}", file=pos_file)
