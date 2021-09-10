import os

def discern_infinity(val):
    if val == float('inf'):
        return '1.00000e+20'
    return f'{val:.5e}'

def c_ary_print(data: dict):
    STEP_SZ = 8
    for k, v in data.items():
        unit_str = ('*' + v['units']) if v['units'] else ''
        dat = v['data']

        print(f"const std::vector<G4double> {k} =", "{")

        for i in range(0, dat.size, STEP_SZ):
            row = ', '.join(f'{discern_infinity(val)}{unit_str}' for val in dat[i:i+STEP_SZ])
            print(4*' ' + row, end='')
            if i + STEP_SZ <= dat.size:
                print(',')

        print(os.linesep + '};', end=2*os.linesep)
