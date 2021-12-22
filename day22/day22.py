# This is Blender python
# For part 1, make sure to delete all input lines not under 50
# import bpy, re

# context = bpy.context

# bpy.ops.mesh.primitive_cube_add(scale=(2,2,2), location=(0, 0, 0))
# original_cube = context.object

# bpy.ops.mesh.primitive_cube_add(scale=(1,1,1), location=(1, 1, 1))
# sub_cube = context.object

# mod = original_cube.modifiers.new("Boolean", type='BOOLEAN')
# mod.operation = 'DIFFERENCE'
# mod.object = sub_cube

# bpy.context.view_layer.objects.active= original_cube
# bpy.ops.object.modifier_apply(modifier=mod.name)

# bpy.data.objects.remove(sub_cube)

import bpy, re

lines = []
context = bpy.context

with open("C:\\Users\\beake\\Desktop\\Advent of Code 2021\\day22\\input.txt", "r") as f:
    lines = f.readlines()

# def divBy100(t):
#     return (t[0]/100.0, t[1]/100.0, t[2]/100.0)

def divByD(t, d):
    return (t[0]/d, t[1]/d, t[2]/d)

def divBy2(t):
    return divByD(t, 2.0)

scale = 0.2

def divByScale(t):
    return divByD(t, scale)

groups = re.search(r"(on|off) x=([\d-]+)\.\.([\d-]+),y=([\d-]+)\.\.([\d-]+),z=([\d-]+)\.\.([\d-]+)", lines[0]).groups()
coords = [(float(groups[1]), float(groups[3]), float(groups[5])), (float(groups[2]), float(groups[4]), float(groups[6]))]
coords[0] = divByScale(coords[0])
coords[1] = divByScale(coords[1])
print(divByD(coords[0], 1/scale), divByD(coords[1], 1/scale))
size = (coords[1][0]-coords[0][0]+(1.0/scale),coords[1][1]-coords[0][1]+(1.0/scale),coords[1][2]-coords[0][2]+(1.0/scale))
print(size)
bpy.ops.mesh.primitive_cube_add(scale=divBy2(size), location=(coords[0][0]+(size[0]/2.0), coords[0][1]+(size[1]/2.0), coords[0][2]+(size[2]/2.0)))
original_cube = context.object

def report_info(title="", message=""):
    # show in console
    print(title, message)
    # show in popup
    def draw_cb(self, context):
        self.layout.label(text=message)
    context.window_manager.popup_menu(draw_func=draw_cb, title=title, icon='INFO')


def is_face_skip(f):
    """Ignore faces that pass this test!"""
    return f.hide is False
    # You may want to filter based on material.
    # return f.material_index == 0


def bmesh_from_object_final(ob):
    import bmesh
    matrix = ob.matrix_world
    me = ob.to_mesh()
    me.transform(matrix)
    bm = bmesh.new()
    bm.from_mesh(me)
    # if USE_FILTER_FACES:
    #     faces_remove = [f for f in bm.faces if not is_face_skip(f)]
    #     for f in faces_remove:
    #         bm.faces.remove(f)
    return (bm, matrix.is_negative)


def volume_and_area_from_object(ob):
    bm, is_negative = bmesh_from_object_final(ob)
    volume = bm.calc_volume(signed=True)
    area = sum(f.calc_area() for f in bm.faces)
    bm.free()
    if is_negative:
        volume = -volume
    return volume, area

for line in lines[1:]:
    groups = re.search(r"(on|off) x=([\d-]+)\.\.([\d-]+),y=([\d-]+)\.\.([\d-]+),z=([\d-]+)\.\.([\d-]+)", line).groups()
    coords = [(float(groups[1]), float(groups[3]), float(groups[5])), (float(groups[2]), float(groups[4]), float(groups[6]))]
    coords[0] = divByScale(coords[0])
    coords[1] = divByScale(coords[1])
    print(divByD(coords[0], 1/scale), divByD(coords[1], 1/scale))
    size = (coords[1][0]-coords[0][0]+(1.0/scale),coords[1][1]-coords[0][1]+(1.0/scale),coords[1][2]-coords[0][2]+(1.0/scale))
    print(size)
    bpy.ops.mesh.primitive_cube_add(scale=divBy2(size), location=(coords[0][0]+(size[0]/2.0), coords[0][1]+(size[1]/2.0), coords[0][2]+(size[2]/2.0)))
    sub_cube = context.object
    mod = original_cube.modifiers.new("Boolean", type='BOOLEAN')
    mod.operation = 'DIFFERENCE' if groups[0] == "off" else "UNION"
    print(mod.operation)
    mod.object = sub_cube

    bpy.context.view_layer.objects.active= original_cube
    bpy.ops.object.modifier_apply(modifier=mod.name)

    bpy.data.objects.remove(sub_cube)
    volume, area = volume_and_area_from_object(original_cube)
    print(volume*0.008, "=", volume, "*", 0.008, "plug into calculator to check for floating point errors") # scale = 0.2, scale^3 = 0.008