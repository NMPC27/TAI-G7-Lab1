import numpy as np
import os.path
import gzip
from os import stat
from math import log2
from argparse import ArgumentParser


"""
Calculate information and sliding entropy at each step, as stored in .csv files,
and output it into the 'processed' folder.
"""


def get_approximate_byte_count(file_name: str) -> int:
    file_stat = stat(file_name)
    return file_stat.st_size

def get_information(file_name: str, max_size: int) -> np.ndarray:
    information_steps: np.ndarray = np.empty((max_size, 1), dtype=np.float32)

    with gzip.open(file_name, 'rt') as f:
        f.readline()    # skip header
        for i, line in enumerate(f):
            line = line[:-1]    # remove newline
            
            spl = line.split(',')
            # If this happens, then we are likely at the end of the file, quit
            if len(spl) != 4:
                break

            _, actual, _, distribution_whole = spl

            distribution_strings = distribution_whole.split(':')
            symbol_probability = float(distribution_strings[distribution_strings.index(actual) + 1])
            information_steps[i] = -log2(symbol_probability)
            
            print(f'Progress: {i / max_size:%}', end='\r')
        print()
        
        actual_size = i + 1
        information_steps.resize((actual_size, 1))
    
    return information_steps

def process_and_save_information_and_entropy(main_file_name: str):
    main_file_parameters = os.path.basename(main_file_name).split('_')
    source_file_name = '_'.join(main_file_parameters[:len(main_file_parameters) - 5])

    source_file_path = f'../example/{source_file_name}.txt'
    results_file_path = main_file_name
    
    assert os.path.exists(source_file_path), f'Source file \'{source_file_path}\' doesn\'t exist!'
    assert os.path.exists(results_file_path), f'Source file \'{results_file_path}\' doesn\'t exist!'
    
    results_file_path_without_extension = os.path.splitext(os.path.splitext(os.path.basename(results_file_path))[0])[0]
    processed_information_file_path = f'./processed/{results_file_path_without_extension}_information.npy'
    processed_entropy_file_path = f'./processed/{results_file_path_without_extension}_entropy.npy'

    max_number_of_characters = get_approximate_byte_count(source_file_path)

    print('Calculating...')
    information_steps = get_information(results_file_path, max_number_of_characters)
    entropy_at_step = np.cumsum(information_steps) / (np.arange(information_steps.size) + 1)

    print('Saving...')
    np.save(processed_information_file_path, information_steps)
    np.save(processed_entropy_file_path, entropy_at_step)


if __name__ == '__main__':

    parser = ArgumentParser(__file__)
    parser.add_argument('file', nargs='+', type=str)

    args = parser.parse_args()

    for fname in args.file:
        process_and_save_information_and_entropy(fname)