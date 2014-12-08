import argparse
import time
import sys

import matplotlib.pyplot as plot

def main():
    """
    Monitor stdin for incoming data and parse it. When the stream closes, the
    graph will be plotted.

    Format of incoming data: time,current,target,control.
    """

    # Gather data
    start = time.time()
    data = []

    try:
        while True:
            # Read data
            line = sys.stdin.readline()

            if not line:
                break

            # Split data
            line = line.strip().split(",")

            if len(line) < 5:
                continue

            # Parse the data
            data.append(map(int, line[:5]))
    except KeyboardInterrupt:
        pass

    # Check for data
    if not data:
        return

    # Generate graph
    figure = plot.figure()
    axis = figure.add_subplot(1, 1, 1)
    axis2 = axis.twinx()

    times, values, targets, controls, pids = zip(*data)
    axis.plot(times, targets, color="grey", marker=".")
    axis.plot(times, values, color="black")
    axis2.plot(times, controls, color="red")
    #axis2.plot(times, pids, color="green")

    axis.set_xlabel("Time (s)")
    axis.set_ylabel("Temperature (C)")
    axis2.set_ylabel("Control (%)")

    axis.grid()

    # Show
    plot.show()

if __name__ == "__main__":
    sys.exit(main())