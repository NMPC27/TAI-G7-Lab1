import os.path
import subprocess
from argparse import ArgumentParser
from typing import List
from common import parse_processed_name


"""
Take the final results from the .csv files and output them into a CSV table.
"""


def get_results_block(input_file_name: str) -> List[str]:
    completed_process = subprocess.run(['gzip', '-dc', input_file_name, '|', 'tail', '-n', '7'], capture_output=True)
    return completed_process.stdout.decode().strip().split('\n')


if __name__ == '__main__':

    parser = ArgumentParser(__file__)
    parser.add_argument('file', nargs='+', type=str)
    parser.add_argument('-o', '--output', type=str, required=True)

    args = parser.parse_args()
    separator = '\t'

    with open(args.output, 'wt') as f:
        f.write('k\talpha\tdistribution\tmanager\tthresholds\tentropy\n')
        for fname in args.file:
            fname_base, _ = os.path.splitext(os.path.basename(fname))
            _, k, alpha, distribution, manager, thresholds = parse_processed_name(fname_base)
            
            results_block = get_results_block(fname)
            prefix = 'Mean amount of information of a symbol: '
            suffix = ' bits'
            entropy = float(results_block[-2][len(prefix):-len(suffix)])
            
            f.write(f'{k}\t{alpha}\t{distribution}\t{manager}\t{thresholds}\t{entropy}')
