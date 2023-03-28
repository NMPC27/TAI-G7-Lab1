import subprocess
import gzip
from argparse import ArgumentParser

"""
Wrapper script over CPM that writes the results into a file in the 'input' folder,
which is identified by the parameters that were provided to the program.
"""


if __name__ == '__main__':

    parser = ArgumentParser(__file__)
    parser.add_argument('--fname', type=str, required=True, help='the basename of the source file (the one CPM will run on), without the file extension')
    parser.add_argument('-k', type=int, required=True)
    parser.add_argument('-a', type=float, required=True)
    parser.add_argument('-p', type=str, required=True)
    parser.add_argument('-r', type=str, required=True)
    parser.add_argument('-t', type=str, action='append', required=True)

    args = parser.parse_args()

    source_path = f'../example/{args.fname}.txt'
    results_fname = f'{args.fname}_{args.k}_{args.a}_{args.p}_{args.r}_['
    for t in args.t:
        results_fname += t + ','
    results_fname += ']'

    cmd = ['../bin/cpm', '-v', 'm', '-k', str(args.k), '-a', str(args.a), '-p', args.p, '-r', args.r]
    for t in args.t:
        cmd += ['-t', t]
    cmd += [source_path]

    with gzip.open(f'./input/{results_fname}.csv', 'w') as f:
        subprocess.run(cmd, stdout=f)
