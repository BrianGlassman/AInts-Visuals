#%% Parse the file
filename = 'Objects/Mine4-4_ORIG.obj'

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
faces = foo ; del foo, bar

#%% Function def
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

#%% Remove edges, tracking the removals
outer_edge = 0
for vertex in vertices:
    mx = max(abs(v) for v in vertex)
    if mx > outer_edge:
        outer_edge = mx
print(f"Outer edge = {outer_edge}")

# Remove MV box corners
edgeMap = ['1-INDEX']
skipped = 0
for i, vertex in enumerate(vertices):
    if all(abs(v) == outer_edge for v in vertex):
        skipped += 1
        edgeMap.append(None)
    else:
        edgeMap.append(i - skipped + 1)
# print(edgeMap)

removeVertMap(edgeMap, vertices, faces)

#%% Patch the cut from core to outside
cut_verts = set()
cut_faces = set()
for f, face in enumerate(faces):
    if any(abs(vertices[v['v']-1][0]) == outer_edge for v in face):
        # Face of the cut
        cut_faces.add(f)
        cut_verts.update([v['v']-1 for v in face])
cut_inner = [v for v in cut_verts if vertices[v][0] < outer_edge]
cut_outer = [v for v in cut_verts if vertices[v][0] == outer_edge]
del cut_verts

# Get the middle of the cut
avg_v = [0, 0, 0]
for v in cut_inner:
    v = vertices[v]
    avg_v[0] += v[0]
    avg_v[1] += v[1]
    avg_v[2] += v[2]
n = len(cut_inner)
avg_v[0] /= n
avg_v[1] /= n
avg_v[2] /= n

# Pull outer vertices to the middle
for v in cut_outer:
    vertices[v] = avg_v

# Update normals
for f in cut_faces:
    face = faces[f]
    for v in face:
        v['n'] = 1

print(f"Patched cut to ({avg_v[0]:0.2f}, {avg_v[1]:0.2f}, {avg_v[2]:0.2f})" +
    f" with normal = {normals[1]}")

del cut_inner, cut_outer, avg_v, cut_faces

#%% Remove the core for now so I can see what I'm doing
if False:
    coreMap = ['1-INDEX']
    skipped = 0
    for i, vertex in enumerate(vertices):
        if all(abs(v) <= 4.5 for v in vertex):
            skipped += 1
            coreMap.append(None)
        else:
            if any(abs(v) > 6.35 for v in vertex):
                print(vertex)
            coreMap.append(i - skipped + 1)

    removeVertMap(coreMap, vertices, faces)

#%% Convert back to text
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

#%% Write the updated geometry
with open(filename.replace("_ORIG", ''), 'w') as f:
    f.writelines(lines)