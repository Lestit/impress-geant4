import numpy as np

Z_START = 1.25
Z_END = 6.25
DZ = 0.5 # mm
DX = 2 # mm
NUM_PHOTS = 100000

script_begin =\
f'''/impe/distributionType gps

/gps/particle opticalphoton

/gps/pos/type Point

/gps/ang/type iso
/gps/polarization 1 0 0

/gps/ene/type Mono
/gps/ene/mono 3 eV

/run/printProgress {NUM_PHOTS/5}

'''

def pozition():
    for x in np.arange(0, 37/2, DX):
        for z in np.arange(Z_START + 0.1, Z_END, DZ):
            yield (x, z)

if __name__ == '__main__':
    with open('optical_vary_xz.mac', 'w') as f:
        print(script_begin, file=f)
        for (x, z) in pozition():
            print(f'/gps/pos/centre {x:.2f} 0 {z:.2f} mm', file=f)
            print(f'/run/beamOn {NUM_PHOTS}\n', file=f)
