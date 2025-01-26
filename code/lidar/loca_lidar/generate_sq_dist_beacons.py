from math import sqrt, pow

beacons = [
    [3094, 50],
    [-94, 1000],
    [3094, 1950],
]

calculated_dist = [0] * len(beacons)
for i in range(len(beacons)):
    calculated_dist[i] = [0] * len(beacons)


for i in range(len(beacons)):
    for j in range(len(beacons)):
        if j <= i:
            continue
        sq_dist = pow(beacons[i][0] - beacons[j][0], 2) + pow(beacons[i][1] - beacons[j][1],2)
        print(i, j, sq_dist)
        calculated_dist[i][j] = sq_dist

print(calculated_dist)