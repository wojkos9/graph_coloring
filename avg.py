import sys
import subprocess

n = int(sys.argv[1])
args = sys.argv[2:]
args += ["-f", "%d %d\n"]
sums = [0, 0]
for _ in range(n):
    result = subprocess.run(["./main", *args], stdout=subprocess.PIPE)
    ts, greedy = str(result.stdout.decode('utf-8')).split()
    sums[0] += int(ts)
    sums[1] += int(greedy)
    print(ts, greedy)

print("Average:")
print(" ".join([str(float(s)/n) for s in sums]))