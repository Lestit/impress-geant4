import numpy as np
import matplotlib.pyplot as plt
import mpl_toolkits.mplot3d.art3d as art3d
import matplotlib as mpl
import matplotlib.patches
import os
import sys
import traceback

# plt.style.use(os.getenv('MPL_DEFAULT_STYLE'))
CYL_HEIGHT = 5
CYL_CENTER_Z = 1.25 + CYL_HEIGHT / 2
CYL_RADIUS = 37/2

# https://stackoverflow.com/questions/26989131/add-cylinder-to-plot
def data_for_cylinder_along_z(center_x, center_y, center_z, radius, height_z):
    z = np.linspace(-height_z/2, height_z/2, 50) + center_z
    theta = np.linspace(0, 2*np.pi, 50)
    theta_grid, z_grid=np.meshgrid(theta, z)
    x_grid = radius*np.cos(theta_grid) + center_x
    y_grid = radius*np.sin(theta_grid) + center_y
    return x_grid, y_grid, z_grid

def add_cylinder(ax):
    col = 'blue'
    alf = 0.1

    cyl_crds = data_for_cylinder_along_z(0, 0, CYL_CENTER_Z, CYL_RADIUS, CYL_HEIGHT)
    ax.plot_surface(*cyl_crds, color=col, alpha=alf)

    bottom = mpl.patches.Circle((0, 0), CYL_RADIUS, color=col, alpha=alf)
    top = mpl.patches.Circle((0, 0), CYL_RADIUS, color=col, alpha=alf)
    ax.add_patch(top)
    ax.add_patch(bottom)
    art3d.pathpatch_2d_to_3d(top, z=CYL_CENTER_Z + CYL_HEIGHT/2, zdir='z')
    art3d.pathpatch_2d_to_3d(bottom, z=CYL_CENTER_Z - CYL_HEIGHT/2, zdir='z')

def main():
    in_file = sys.argv[1]
    ret = load_pos_eng(in_file)

    xyze = np.array([ret['x'], ret['y'], ret['z'], ret['eng']]).T
    np_eng = np.array(ret['eng'])
    # cut = np_eng < 0.01
    # print(np_eng[cut])
    z = xyze[:,2]
    cut = np.ones_like(z, dtype=bool) #(z < 1.3) | (6.15 < z)

    fig, ax = plt.subplots(subplot_kw={'projection': '3d'})
    path_coll = ax.scatter(
        xyze[:,0][cut], xyze[:,1][cut], xyze[:,2][cut],
        cmap='plasma',
        norm=mpl.colors.LogNorm(vmin=1e-2, vmax=max(ret['eng'])),
        c=xyze[:,3][cut]
    )

    add_cylinder(ax)

    cb = fig.colorbar(path_coll, ax=ax, location='left')
    cb.set_label('Energy deposited (keV)')
    ax.set_xlabel('x (mm)')
    ax.set_ylabel('y (mm)')
    ax.set_zlabel('z (mm)')
    ax.set_zlim(-5, 10)
    ax.set_title(f'Position of crystal hits colored by energy deposit\n{os.path.dirname(in_file)}')
    ax.elev = 5
    fig.tight_layout()
    fig.savefig(f'{os.path.dirname(in_file)}.png', dpi=300)
    # plt.show()

def load_pos_eng(fn):
    # NEED TO SEPARATE EVENTS SOMEHOW
    ret = {'x': [], 'y': [], 'z': [], 'eng': []}
    with open(fn, 'r') as f:
        for line in f:
            ch, *evts = line.strip().split()
            ch = int(ch)
            for e in evts:
                try:
                    splitup = list(float(x) for x in e[1:-1].split(','))
                except ValueError as e:
                    print('*' * 30, 'wrong format simulation data (need energy too)')
                    traceback.print_exc()
                    sys.exit(1)
                x, y, z, eng = splitup
                ret['x'].append(x)
                ret['y'].append(y)
                ret['z'].append(z)
                ret['eng'].append(eng)
    return ret

if __name__ == '__main__': main()
