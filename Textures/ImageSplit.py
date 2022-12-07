import os

orig_dim = 2048
orig_filename = "dirtGroundTileable"

splits = 4
new_dim = int(orig_dim / splits)

os.chdir("Textures")
print(f"Splitting {orig_dim}x{orig_dim} {orig_filename} into {splits*splits} {new_dim}x{new_dim}")

y_offset = 0
for y in range(splits):
    x_offset = 0
    for x in range(splits):
        out_filename = f"{orig_filename}_x{x}_y{y}"
        cmd = f"./magick convert -extract {new_dim:d}x{new_dim:d}+{x_offset:d}+{y_offset:d} {orig_filename}.bmp {out_filename}.bmp"
        print(cmd)
        os.system(cmd)
        x_offset += new_dim
    y_offset += new_dim