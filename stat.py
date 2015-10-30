
statMatrix = [[] for j in xrange(10)]
for i in xrange(10):

    f = open("./state"+str(i)+".txt", 'r')
    fcontent = f.readlines()

    d = {}

    for line in fcontent:
        l = line.strip()
        if l not in d:
            d[l] = 1
        else:
            d[l] = d[l] + 1

    total = 0

    for ele in d:
        total = total + d[ele]


    for ele in d:
        d[ele] = float(d[ele]) / total


    d1 = {}
    for key in sorted(d.iterkeys()):
        statMatrix[i].append(d[key])
    #print sorted(d)
    #d = dict(sorted(d))
    f.close()

#newMatrix = [list(i) for i in zip(*statMatrix)]
#print newMatrix
print d
import numpy as np
labels = ["S", "S1", "S11", "S111", "S2", "S21", "S211"]

x = np.array(statMatrix)

import matplotlib.pyplot as plt
plt.boxplot(x, labels=labels)
plt.title("Statistics of States: 10 trials of 10,000 dispaches")
plt.xlabel("States")
plt.ylabel("Probability Distribution")
plt.show()
