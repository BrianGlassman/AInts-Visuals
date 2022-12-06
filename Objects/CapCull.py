import os, sys
import shutil

filenames = [
    'Objects/Mine4-1.obj',
    'Objects/Mine4-2.obj',
    'Objects/Mine4-3.obj',
    'Objects/Mine4-4.obj',
    'Objects/Mine4-5.obj',
    'Objects/Mine4-6.obj',
]

def parse(filename):
    with open(filename, 'r') as f:
        lines = [x.strip() for x in f.readlines()]

    header = []
    i = 0
    while not lines[i+1].startswith("# normals"):
        header.append(lines[i])
        i += 1

    # Remove comments (would be nice to keep, but not necessary)
    normals = [line.split(' #')[0] for line in lines if line.startswith('vn ')]
    colors = [line.split(' #')[0] for line in lines if line.startswith('vt ')]
    vertices = [line.split(' #')[0] for line in lines if line.startswith('v ')]
    faces = [line.split(' #')[0] for line in lines if line.startswith('f ')]

    normals = [[float(v) for v in line.split('vn ')[1].split(' ')] for line in normals]
    colors = [[float(v) for v in line.split('vt ')[1].split(' ')] for line in colors]
    vertices = [[float(v) for v in line.split('v ')[1].split(' ')] for line in vertices]

    foo = []
    for line in faces:
        bar = []
        line = line.split('f ')[1]
        points = line.split(' ')
        for point in points:
            v, c, n = point.split('/')
            bar.append({'v': int(v), 'c': int(c), 'n': int(n)})
        foo.append(bar)
    faces = foo

    return header, normals, colors, vertices, faces

def removeVertMap(vertMap: list, vertices: list[list[float]], faces: list[list[dict]]) -> None:
    i = len(vertMap) - 1
    while i >= 0:
        if vertMap[i] is None:
            vertices.pop(i-1)
        i -= 1
    print(f"Removed {len(vertMap)-1 - len(vertices)} ({len(vertMap)-1} -> {len(vertices)}) vertices")

    # Update faces to match the removals
    i = 0
    while i < len(faces):
        face = faces[i]

        # Update the vertex numbering to account for skipped vertices
        brk = False
        for point in face:
            v = point['v']
            if vertMap[v] is None:
                # Cull face if using a skipped vertex
                brk = True
                faces.pop(i)
                break
            else:
                point['v'] = vertMap[v]
        if (brk): continue

        i += 1

def cullCap(vertices: list[list[float]], faces: list[list[dict]]):
    oldCount = len(faces)
    outer_edge = 0
    for vertex in vertices:
        mx = max(abs(v) for v in vertex)
        if mx > outer_edge:
            outer_edge = mx
    print(f"Outer edge = {outer_edge}")

    # Remove faces that are entirely on the outer edge
    remove = []
    for f, face in enumerate(faces):
        drop = True
        for point in face:
            if all(abs(v) != outer_edge for v in vertices[point['v']-1]):
                # Not an edge vertex, therefore not an edge face
                drop = False
                break
        if drop:
            remove.append(f)
    
    for f in reversed(remove):
        faces.pop(f)

    print(f"Removed {len(remove)} faces ({oldCount} --> {len(faces)})")

    return vertices, faces

def unparse(header, normals, colors, vertices, faces):
    normals = ['# normals'] + ["vn " + " ".join(str(v) for v in line) for line in normals]
    colors = ['# texcoords'] + ["vt " + " ".join(str(v) for v in line) for line in colors]
    vertices = ['# verts'] + ["v " + " ".join(str(v) for v in line) for line in vertices]
    foo = []
    for face in faces:
        point = [f"{p['v']}/{p['c']}/{p['n']}" for p in face]
        point = ' '.join(point)
        foo.append('f ' + point)
    faces = ["# faces"] + foo ; del foo, point

    def addNL(lines):
        return [line+'\n' for line in lines]
    header = addNL(header)
    normals = addNL(normals)
    colors = addNL(colors)
    vertices = addNL(vertices)
    faces = addNL(faces)

    sep = ["\n"]
    lines = header + sep + normals + sep + colors + sep + vertices + sep + faces
    lines[-1] = lines[-1][:-1] # Remove trailing newline at end of file

    return lines

def write(filename, lines):
    with open(filename.replace("_ORIG", ''), 'w') as f:
        f.writelines(lines)

for filename in filenames:
    out = filename
    orig = filename.replace('.obj', '_ORIG.obj')

    # Make sure there's an original copy
    if not os.path.exists(orig):
        print(f"No backup found for {filename}, saving as {orig}")
        shutil.move(filename, orig)

    print(filename)
    header, normals, colors, vertices, faces = parse(orig)
    vertices, faces = cullCap(vertices, faces)
    lines = unparse(header, normals, colors, vertices, faces)
    write(out, lines)
    print()