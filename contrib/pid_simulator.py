import argparse
import cpid
import sys

class Model(object):

    def __init__(self, start, target):
        self.current = start
        self.target = target
        self.control = 0

        self.heating = 2.25
        self.steps = 0

    def step(self):
        if self.control > 0:
            self.steps += 1
        else:
            self.steps = 0

        delta = self.heating - 2 ** (1 + max(0, self.control / 256.0))
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
    print arguments.kp
    pid = cpid.PIDController(arguments.kp, arguments.ki, arguments.kd)

    # Create model
    model = Model(start=250, target=300)

    for i in xrange(arguments.steps):
        # Update model
        model.step()

        # Calculate PID output
        output = -1 * pid.update(model.current, model.target)

        # Limit output
        if output > 0:
            control = output * 100 / 256

            if control < 10:
                control = 0
        else:
            control = 0

        # Feed into model
        model.control = control

        # Output
        sys.stdout.write("%d,0,%d,%d,%d,%d,%d\n" %
            (i, model.current, model.target, model.control, output, model.control))

if __name__ == "__main__":
    sys.exit(main())