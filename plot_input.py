import matplotlib.pyplot as plt

# label data:
plt.xlabel("Iteracja")
plt.ylabel("Liczba kolorów")

class Limit:
    def __init__(self, action):
        self.min = 0
        self.max = 10
        self.action = action
        self.update()

    def update(self):
        self.action(self.min, self.max)

    def feed(self, val):
        if val < self.min:
            self.min = val
            self.update()
        elif val > self.max:
            self.max = val
            self.update()

xlimit = Limit(plt.xlim)
ylimit = Limit(plt.ylim)

plots = ['ro-', 'go-', 'bo-', 'yo-']

plt.draw()
plt.pause(0.01) #is necessary for the plot to update for some reason

line = input()
while line != "__begin__":
    print(line)
    line=input()

while True:
    try:
        line = input()
        if line == "__end__":
            break
        vals = [float(x) for x in line.split()]
    except EOFError as e:
        break
    t = vals[0]
    xlimit.feed(t * 1.1)
    for i, y in enumerate(vals[1:]):
        ylimit.feed(y * 1.1)
        # plot the point
        plt.plot(t, y, plots[i] if i < len(plots) else 'ko-')
		

    # draw the plot
    plt.draw()
    plt.pause(0.01) #is necessary for the plot to update for some reason

while True:
    try:
        print(input())
    except EOFError as e:
        break

plt.show()