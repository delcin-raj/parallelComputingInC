import matplotlib.pyplot as plt
steps = []
time_1 = []
time_multi = []
time_red = []
time_partition = []

with open('result_single.txt') as f1:
    readings = f1.readlines()

for line in readings:
    t = line.split()
    steps.append(int(t[0]))
    time_1.append(float(t[-1]))

with open('result_multi.txt') as f1:
    readings = f1.readlines()

for line in readings:
    t = line.split()
    time_multi.append(float(t[-1]))

with open('result_partition.txt') as f1:
    readings = f1.readlines()

for line in readings:
    t = line.split()
    time_partition.append(float(t[-1]))

with open('result_omp_reduce.txt') as f1:
    readings = f1.readlines()

for line in readings:
    t = line.split()
    if len(t) > 0:
        time_red.append(float(t[-1]))

fig = plt.figure()
ax = fig.add_subplot(2,2,1)
ax.plot(steps,time_1)
ax.set_title("single thread")

ax = fig.add_subplot(2,2,2)
ax.plot(steps,time_multi)
ax.set_title("Using jumps")

ax = fig.add_subplot(2,2,3)
ax.plot(steps,time_partition)
ax.set_title("Using partition")

ax = fig.add_subplot(2,2,4)
ax.plot(steps,time_red)
ax.set_title("Using reduction")

plt.savefig("plot.jpeg")
