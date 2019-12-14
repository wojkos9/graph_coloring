import matplotlib.pyplot as plt

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

plots = ['r-', 'g.-', 'b.-', 'y.-']

vals = []
last_vals = []
plotting = False
def safe_read():
    global vals, last_vals, plotting
    try:
        line = input()
        print(line)

        if line == "__begin__":
            plotting = True
            line = input()
            vals = [float(x) for x in line.split()]
            ylimit.feed(vals[1])
            ylimit.update()
            print(line)
            return False
        elif line == "__end__":
            plotting = False

        if plotting:
            last_vals = vals
            vals = [float(x) for x in line.split()]
    except EOFError:
        return False
    return plotting


while True:
    if safe_read():
        t = (last_vals[0], vals[0])
        xlimit.feed(t[1])
        for i, ys in enumerate(zip(last_vals[1:], vals[1:])):
            ylimit.feed(ys[1])
            # plot the point
            plt.plot(t, ys, plots[i] if i < len(plots) else 'k.-')
            
        # redraw the plot
        plt.draw()
        plt.pause(0.01) #is necessary for the plot to update for some reason

while True:
    try:
        print(input())
    except EOFError as e:
        break

plt.show()