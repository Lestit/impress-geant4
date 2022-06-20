import numpy as np

OUT_FN = 'optical_xyz_scan.mac'

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
    for x in np.linspace(0, RADIUS - 0.1, num=NUM_X):
        for y in np.linspace(0, RADIUS - 0.1, num=NUM_Y):
            for z in np.linspace(Z_START + 0.1, Z_END - 0.1, num=NUM_Z):
                yield (x, y, z)

if __name__ == '__main__':
    with open(OUT_FN, 'w') as f:
        print(script_begin, file=f)
        for (x, y, z) in pozition():
            print(f'/gps/pos/centre {x:.2f} {y:.2f} {z:.2f} mm', file=f)
            print(f'/run/beamOn {NUM_PHOTS}\n', file=f)
