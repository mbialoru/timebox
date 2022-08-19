import sys
import matplotlib.pyplot as plt

rawDataBuffer=[]

for line in sys.stdin:
    try:
        rawDataBuffer.append(float(line.rstrip()))
    except ValueError:
        print(f"ValueError ocurred")

x = range(len(rawDataBuffer))
plt.plot(x, rawDataBuffer, color = 'b')
plt.savefig(f"time_history.png")