output = open("output.txt", "w")

output.write("int stationsToVisit[1000] = {")
with open("C:\\CoSpaceRobot Studio\\ADL-2023\\ADL\\Bin\\path.txt") as f:
    lines = f.readlines()
    for i, line in enumerate(lines):
        line = line.replace("\n", "")
        output.write(f'{line}, ')
    f.close()

output.write("};")
output.close()
