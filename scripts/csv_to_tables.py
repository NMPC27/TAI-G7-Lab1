import os.path
import subprocess
from argparse import ArgumentParser
from typing import List
from common import parse_processed_name


"""
Take the final results from the .csv files and output them into a CSV table.
"""


def get_results_block(input_file_name: str) -> List[str]:
    gzip_output = subprocess.run(['gzip', '-dc', input_file_name], check=True, capture_output=True)
    last_results_lines = subprocess.run(['tail', '-n', '3'], input=gzip_output.stdout, capture_output=True)
    return last_results_lines.stdout.decode().strip().split('\n')


if __name__ == '__main__':

    parser = ArgumentParser(__file__)
    parser.add_argument('file', nargs='+', type=str)
    parser.add_argument('-o', '--output', type=str, required=True)

    args = parser.parse_args()
    separator = '\t'

    with open(args.output, 'wt') as f:
        f.write('k\talpha\tdistribution\tmanager\tthresholds\tentropy\ttime\n')
        for fname in args.file:
            fname_base = os.path.splitext(os.path.splitext(os.path.basename(fname))[0])[0]
            _, k, alpha, distribution, manager, thresholds = parse_processed_name(fname_base)
            
            results_block = get_results_block(fname)

            prefix = 'Mean amount of information of a symbol: '
            suffix = ' bits'
            entropy = float(results_block[-3][len(prefix):-len(suffix)])

            prefix = 'Time elapsed: '
            suffix = ' seconds'
            time = float(results_block[-1][len(prefix):-len(suffix)])
            
            f.write(f'{k}\t{alpha}\t{distribution}\t{manager}\t{thresholds}\t{entropy}\t{time}\n')
