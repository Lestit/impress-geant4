import run_mod_impress_conf as rmc

NUM_RUNS = 1000
MACRO_FN = 'macros/arrival_timez.mac'
MACRO_CONTENTS_FMT = '''
/impe/distributionType gps
/gps/particle gamma
/gps/pos/centre 0 0 {zpos:.2f} cm
/gps/pos/type Point
/gps/direction 0 0 -1
/gps/ene/type Mono
/gps/ene/mono {energy:.2f} keV
/run/beamOn {num_runs}
'''

def main():
    configs = [
        {'cebr3-length': 5, 'scint-time-const': 20},
        {'cebr3-length': 37, 'scint-time-const': 20},
        {'cebr3-length': 5, 'scint-time-const': 230},
        {'cebr3-length': 37, 'scint-time-const': 230}
    ]

    energies = [8, 60]

    for e in energies:
        for c in configs:
            l, t = c['cebr3-length'], c['scint-time-const']
            c['data-folder-prefix'] = f'{e}keV-{l}mm-{t}ns-{NUM_RUNS}runs'
            rmc.update_conf(c)
            with open(MACRO_FN, 'w') as f:
                print(MACRO_CONTENTS_FMT.format(zpos=(l + 2), energy=e, num_runs=NUM_RUNS), file=f)
            rmc.run_macro(MACRO_FN)

if __name__ == '__main__': main()
