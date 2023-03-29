import gc
import numpy as np
import matplotlib.pyplot as plt
from os import stat, listdir
from math import log2
from dataclasses import dataclass
from typing import Dict, List, Tuple
from argparse import ArgumentParser
from scipy.ndimage import uniform_filter1d

def parse_processed_name(name: str) -> Tuple[str, int, float, str, str, int]:
    parameters = name.split('_')
    fname_parameter_span = len(parameters) - 5

    fname = '_'.join(parameters[:fname_parameter_span])
    k = int(parameters[fname_parameter_span])
    alpha = float(parameters[fname_parameter_span + 1])
    probability_distribution = parameters[fname_parameter_span + 2]
    pointer_manager = parameters[fname_parameter_span + 3]
    thresholds = parameters[fname_parameter_span + 4]

    return fname, k, alpha, probability_distribution, pointer_manager, thresholds

def plot_information(information_steps: np.ndarray, sliding_entropy: np.ndarray, processed_file_name: str):
    source_file_name, k, alpha, probability_distribution, pointer_manager, thresholds = parse_processed_name(processed_file_name)
    
    line_is, = plt.plot(information_steps, label='instant information')
    line_se, = plt.plot(sliding_entropy, label='cummulative information (entropy)')
    plt.legend(handles=[line_is, line_se], loc='upper left')
    plt.title(f'Information throughout the message coding steps ({source_file_name})\n(k={k}, alpha={alpha}, p={probability_distribution}, r={pointer_manager}, t={thresholds})')
    plt.xlabel('step')
    plt.ylabel('bits')
    plt.savefig(f'./images/{processed_file_name}.png')
    plt.close()

if __name__ == '__main__':

    parser = ArgumentParser(__file__)
    parser.add_argument('file', nargs='+', type=str, help='base file names (without extension and \'_{information,entropy}\')')
    parser.add_argument('-w', '--window', type=int, default=1, help='apply moving average with window of size W (default: 1)')

    args = parser.parse_args()

    for processed_run in args.file:
        print('Loading processed files...')
        information_steps = np.load(f'./processed/{processed_run}_information.npy')
        sliding_entropy = np.load(f'./processed/{processed_run}_entropy.npy').reshape(-1, 1)

        print('Applying moving average...')
        N = args.window
        information_steps = uniform_filter1d(information_steps.flatten(), N, mode='nearest')
        sliding_entropy = uniform_filter1d(sliding_entropy.flatten(), N, mode='nearest')

        print('Plotting data...')
        plot_information(information_steps, sliding_entropy, processed_run)
        print('Done for', processed_run)

        del information_steps
        del sliding_entropy
        gc.collect()
