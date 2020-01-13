import matplotlib.pyplot as plt
import math

# label data:
plt.xlabel("Iteracja")
plt.ylabel("Liczba kolorów")

class Limit:
    def __init__(self, action):
        self.min = 0
        self.max = 0
        self.fed = False
        self.action = action

    def update(self):
        self.action(self.min, self.max)

    def feed(self, val):
        vmin = val / 1.1
        vmax = val * 1.1 + 1
        if not self.fed:
            self.min = vmin
            self.max = vmax
            self.fed = True
        else:
            if vmin < self.min:
                self.min = vmin
                self.update()
            elif vmax > self.max:
                self.max = vmax
                self.update()

xlimit = Limit(plt.xlim)
ylimit = Limit(plt.ylim)

xlimit.feed(-30000)

plots = ['r.-', 'g.-', 'b.-', 'y.-']

vals = {}
last_vals = {}
last_i = 0
plotting = False
finished = False
def safe_read():
    global vals, last_vals, plotting, last_i, finished
    try:
        line = input()
        print(line)

        if line == "__begin__":
            plotting = True
            return False
        elif line == "__end__":
            finished = True
            plotting = False

        if plotting:
            spl = line.split()
            last_i = i = int(spl[0])
            vs = [float(x) for x in spl[1:]]

            if i in vals:
                last_vals[i] = vals[i]
                vals[i] = vs
            else:
                last_vals[i] = vs
                vals[i] = vs

    except EOFError:
        return False
    return plotting

while not finished:
    if safe_read():
        lv = last_vals[last_i]
        v = vals[last_i]
        t = (lv[0], v[0])
        xlimit.feed(t[1])
        plt.plot(t, (lv[1], v[1]), plots[last_i] if last_i < len(plots) else 'k.-')
            
        # redraw the plot
        plt.draw()
        plt.pause(0.01) #is necessary for the plot to update for some reason

while True:
    try:
        print(input())
    except EOFError as e:
        break

plt.show()