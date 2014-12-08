import argparse
import serial
import time
import sys

def parse_arguments():
    """
    Parse command line arguments.
    """

    parser = argparse.ArgumentParser()

    # Add options
    parser.add_argument("port", help="serial port")
    parser.add_argument("baud", help="baud rate")

    # Parse command line
    return parser.parse_args(), parser

def main():
    """
    Monitor serial port and write it to stdout.
    """

    # Parse arguments
    arguments, parser = parse_arguments()

    # Open serial port
    port = serial.Serial(arguments.port, arguments.baud)

    # Read and write until CTRL + C
    start = time.time()

    try:
        while True:
            line = port.readline()

            if not line:
                break

            sys.stdout.write("%d,%s" % (time.time() - start, line))
            sys.stdout.flush()
    except KeyboardInterrupt:
        pass

if __name__ == "__main__":
    sys.exit(main())