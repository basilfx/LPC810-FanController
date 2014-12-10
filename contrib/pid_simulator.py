import argparse
import cpid
import sys

class Model(object):

    def __init__(self, start, target):
        self.current = start
        self.target = target
        self.control = 0

        self.heating = 0.07

    def step(self):
        delta = self.heating - (1.7**(self.control / 10.0) - 1.0)
        #delta = (self.weight / self.heating) - (self.cooling + self.control)
        self.current = self.current + delta


def parse_arguments():
    """
    Parse command line arguments.
    """

    parser = argparse.ArgumentParser()

    # Add options
    parser.add_argument("--kp", type=int, help="serial port", required=True)
    parser.add_argument("--ki", type=int, help="integral term", required=True)
    parser.add_argument("--kd", type=int, help="derivative term", required=True)

    parser.add_argument("--steps", type=int, help="number of steps", default=1000)

    # Parse command line
    return parser.parse_args(), parser

def main():

    # Parse arguments
    arguments, parser = parse_arguments()

    # Create PID controller and model
    pid = cpid.PIDController(arguments.kp, arguments.ki, arguments.kd)

    # Create model
    model = Model(start=25.0, target=30.0)

    for i in xrange(arguments.steps):
        # Update model
        model.step()

        # Calculate PID output
        output = -1 * pid.update(model.current, model.target)

        # Limit output
        if output > 0:
            control = output * 100 / 128
        elif output < 10:
            control = 0

        # Feed into model
        model.control = control

        # Output
        sys.stdout.write("%d,0,%d,%d,%d,%d,%d\n" %
            (i, model.current, model.target, model.control, output, model.control))

if __name__ == "__main__":
    sys.exit(main())