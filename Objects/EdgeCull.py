#%% Parse the file
filename = 'Objects/Mine_ORIG.obj'
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

#%% Remove edges, tracking the removals
vertMap = ['1-INDEX']
skipped = 0
for i, vertex in enumerate(vertices):
    if all(abs(v) == 6.4 for v in vertex):
        skipped += 1
        vertMap.append(None)
    else:
        vertMap.append(i - skipped + 1)
# print(vertMap)

i = len(vertMap) - 1
while i >= 0:
    if vertMap[i] is None:
        vertices.pop(i-1)
    i -= 1
print(f"Removed {len(vertMap)-1 - len(vertices)} / {len(vertMap)-1} vertices")

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