import argparse
import time
import sys

import matplotlib.pyplot as plot

def main():
    """
    Monitor stdin for incoming data and parse it. When the stream closes, the
    graph will be plotted.

    Format of data: i,temperature_i,target_i,speed_i,pid_i,...,max_speed
    """

    # Gather data
    start = time.time()
    sensors = None
    data = []

    try:
        while True:
            # Read data
            line = sys.stdin.readline()

            if not line:
                break

            # Split data
            line = line.strip().split(",")

            # Determine number of sensors from first line of data
            if (len(line) - 2) % 5 == 0:
                if sensors is None:
                    sensors = (len(line) - 2) / 5
            else:
                continue

            # Check if all data is available
            if len(line) != sensors * 5 + 2:
                continue

            # Parse the data
            data.append(map(int, line))
    except KeyboardInterrupt:
        pass

    # Check for data
    if not data:
        return

    # Generate graph
    figure = plot.figure()
    axis = None

    times = zip(*data)[0]
    max_speeds = zip(*data)[-1]

    for i in xrange(sensors):
        _, temperatures, targets, speeds, pids = zip(*data)[i*5 + 1:i*5 + 6]

        axis = figure.add_subplot(1, sensors, i + 1, sharey=axis)
        axis2 = axis.twinx()

        axis.plot(times, targets, color="b", label="Target")
        axis.plot(times, temperatures, color="c", label="Temperature")
        axis2.plot(times, speeds, color="y", label="Speed")
        axis2.plot(times, max_speeds, color="r", label="Max Speed")

        axis.set_xlabel("Time (s)")
        axis.set_ylabel("Temperature (C)")
        axis2.set_ylabel("Control (%)")

        axis.set_ylim(bottom=0, top=1000)
        axis2.set_ylim(bottom=0, top=100)

        axis.grid()

    # Show
    plot.show()

if __name__ == "__main__":
    sys.exit(main())