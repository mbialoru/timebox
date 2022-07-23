import sys
import matplotlib.pyplot as plt
from decimal import Decimal

def fexp(number):
    (_, digits, exponent) = Decimal(number).as_tuple()
    return len(digits) + exponent - 1

def fman(number):
    return Decimal(number).scaleb(-fexp(number)).normalize()

rawDataBuffer = []

for line in sys.stdin:
    try:
        rawDataBuffer.append(float(line.rstrip()))
    except ValueError:
        print(f"ValueError ocurred")

min_t = float("{:.3e}".format(min(x for x in rawDataBuffer)))
max_t = float("{:.3e}".format(max(x for x in rawDataBuffer)))
avg_t = float("{:.3e}".format(sum(rawDataBuffer) / len(rawDataBuffer)))
# rec_r = float(f"1.000e{fexp(max_t) + 1}")
rec_r = 2 * max_t

print(f"AVG clock resoution was: {avg_t}")
print(f"MIN clock resoution was: {max_t}")
print(f"MAX clock resoution was: {min_t}")
print(f"MIN to AVG ratio: {max_t / avg_t:.3f}")

print(f"\nNote: MAX clock ratio should not be 0.0")
print(f"Recommended safe clock resolution: {rec_r}")

# Plotting
name = 'plot' if len(sys.argv) < 2 else sys.argv[1]
x = range(len(rawDataBuffer))

plt.plot(x, rawDataBuffer, color = 'b')
plt.axhline(y = max_t, label = "MIN", color = 'r', linestyle = '--')
plt.axhline(y = min_t, label = "MAX", color = 'g', linestyle = '--')
plt.axhline(y = avg_t, label = "AVG", color = 'y', linestyle = '--')
plt.axhline(y = rec_r, label = "REC", color = 'k', linestyle = '-.')

plt.legend()
plt.xlabel("N")
plt.ylabel("Delta")
plt.savefig(f"{name}.png")

print(f"Run finished, check {name}.png")