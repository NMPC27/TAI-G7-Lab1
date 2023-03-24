import numpy as np
import matplotlib.pyplot as plt
from argparse import ArgumentParser
from math import log2
from dataclasses import dataclass
from typing import Dict, List, Tuple


@dataclass
class PredictionStep:
    prediction:         str
    actual:             str
    hit_probability:    float
    distribution:       Dict[str, float]


parser = ArgumentParser('Plot information')
parser.add_argument('-f', '--file', type=str, default='input/test1.csv')
parser.add_argument('-n', '--aggregate-n', type=int, default=1)

args = parser.parse_args()
file_name = args.file
aggregate_n = args.aggregate_n

information_steps = []
with open(file_name, 'rt') as f:
    f.readline()    # skip header
    information_steps_aggregate = []
    n = 0
    for i, line in enumerate(f):
        line = line.strip()
        prediction, actual, hit_probability, distribution_whole = line.split(',')

        distribution_strings = [s for s in distribution_whole.split(':') if s != '']
        distribution = {k:float(v) for k, v in zip(distribution_strings[::2], distribution_strings[1::2])}
        prediction_step = PredictionStep(prediction, actual, hit_probability, distribution)

        n += 1
        information_steps_aggregate.append(-log2(prediction_step.distribution[prediction_step.actual]))
        if n == aggregate_n:
            information_steps.append(sum(information_steps_aggregate) / aggregate_n)
            information_steps_aggregate.clear()
            n = 0
        
print('Finished reading')

entropy_at_step = [cs/(i+1) for i, cs in enumerate(np.cumsum(information_steps))]

plt.plot(information_steps)
plt.plot(entropy_at_step)
plt.legend(['instant information', 'cummulative information (entropy)'])
plt.title(f'Information throughout the message coding steps ({file_name})')
plt.xlabel('step')
plt.ylabel('bits')
plt.show()
