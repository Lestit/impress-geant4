import run_mod_impress_conf as rmic

MACRO_FN = 'macros/optical_3ev_inside_cryst.mac'

base_cfg = {
    'save-si-positions': 1, 'save-si-energies': 0,
    'air-gap-thickness': 10, 'cebr3-length': 5,
    'cebr3-edge-finish': 'Rough_LUT', 'cebr3-back-finish': 'Rough_LUT',
    'optical-photon-draw-freq': 1,
}

si_spacings_mm = [0.25, 1, 2, 3]
lg_thick_mm = [0.25, 3, 8]
build_lg_reflector = [1, 0]

configurations = list()
for si_sp in si_spacings_mm:
    for lgt in lg_thick_mm:
        for br in build_lg_reflector:
            thing = {
                'si-spacing': si_sp, 'light-guide-thickness': lgt,
                'build-light-guide-reflector': br
            }
            thing['data-folder-prefix'] = f'si-space{si_sp:.2f}mm_lg-thick{lgt:.2f}mm_{"yes" if br else "no"}-lg-ref'
            configurations.append(thing)
for c in configurations:
    print(c)

exit(1)
num_cfgs = len(configurations)
i = 0
while i < num_cfgs:
    try:
        c = configurations[i]
        c.update(base_cfg)
        rmic.update_conf(c)
        ret = rmic.run_macro(MACRO_FN)
        if ret.returncode != 0:
            print(ret)
            raise RuntimeError('process died')
    except KeyboardInterrupt:
        print('killing this attempt')
        i -= 1
    i += 1
