import re

filePath = input("File path: ")

data = ""
with open(filePath, "r") as F:
    data = F.read()

structName = input("Struct name: ")

objName = input("Object name: ")

side = input("side: ")

verticesObj = [x for x in data.split('\n') if len(x) > 0 and x[0] == 'v' and x[1] == ' ']

indicesObj = [x for x in data.split('\n') if len(x) > 0 and x[0] == 'f']

vertices = []

for x in verticesObj:
    split = x.split(' ')
    temp = []
    if side == "":
        v1 = float(split[1])
        v2 = float(split[2])
        v3 = float(split[3])
    else:
        temp.append(("-" if float(split[1]) < 0.0 else "") + "side")
        temp.append(("-" if float(split[2]) < 0.0 else "") + "side")
        temp.append(("-" if float(split[3]) < 0.0 else "") + "side")

    vertices.append(temp)

indices = []
indlen = 0

for x in indicesObj:
    temp = []
    for y in re.findall(re.compile(r"(?<= )(.*?)(?=/)"), x):
        temp.append(y)
        indlen += 1
    indices.append(temp)

result = "" if structName == "" else ("struct " + structName + " {\n")
if side != "": result += f"static constexpr float side = {side}f;\n"
result += f"static constexpr std::pair<std::array<DirectX::XMFLOAT3, {len(vertices)}>, std::array<uint16_t, {indlen}>> {objName}" + "{\n\t{\n"
for x in vertices:
    result += f"\t\tDirectX::XMFLOAT3({x[0]}, {x[1]}, {x[2]}),\n"
result += "\t},\n\t{\n"
for x in indices:
    build = ""
    for y in x:
        build += f"{int(y) - 1}, "
    result += "\t\t" + build[:len(build) - 1] + "\n"
result += "\t}\n};\n"
if structName != "": result += "}"

with open("output.txt", "w") as F:
    F.write(result)
