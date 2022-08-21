import sys
import matplotlib.pyplot as plt

rawDataBuffer=[]

for line in sys.stdin:
    try:
        rawDataBuffer.append(float(line.rstrip()))
    except ValueError:
        print(f"ValueError ocurred")

x = range(len(rawDataBuffer))
plt.plot(x, rawDataBuffer, color = 'bk')
plt.legend()
plt.xlabel("Tick [n]")
plt.ylabel("Delta [ms]")
plt.savefig(f"time_history.png")