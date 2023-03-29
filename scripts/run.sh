#!/bin/bash

# python3 generate_results.py --fname othello -k 12 -a 1.0 -p f -r m -t f:6
# python3 generate_results.py --fname othello -k 12 -a 2.0 -p f -r m -t f:6
# python3 generate_results.py --fname othello -k 12 -a 4.0 -p f -r m -t f:6
# python3 generate_results.py --fname othello -k 12 -a 8.0 -p f -r m -t f:6
# python3 generate_results.py --fname othello -k 12 -a 0.5 -p f -r m -t f:6
# python3 generate_results.py --fname othello -k 12 -a 0.25 -p f -r m -t f:6
# python3 generate_results.py --fname othello -k 12 -a 1.0 -p u -r m -t f:6
# python3 generate_results.py --fname othello -k 12 -a 1.0 -p f -r o -t f:6
# python3 generate_results.py --fname othello -k 12 -a 1.0 -p f -r n -t f:6
# python3 generate_results.py --fname othello -k 12 -a 1.0 -p f -r c:10 -t f:6
# python3 generate_results.py --fname othello -k 12 -a 1.0 -p f -r c:100 -t f:6
# python3 generate_results.py --fname othello -k 12 -a 1.0 -p f -r c:1000 -t f:6

# python3 csv_to_tables.py input/othello*.csv.gz -o all_res_othello.csv

# python3 csv_to_information.py input/othello*.csv.gz

python3 images_information.py \
    'othello_12_0.5_f_m_[f:6,]' \
    'othello_12_1.0_f_m_[f:6,]' \
    'othello_12_2.0_f_m_[f:6,]' \
    'othello_12_4.0_f_m_[f:6,]' \
    'othello_12_8.0_f_m_[f:6,]' \
    'othello_12_0.25_f_m_[f:6,]' \
    'othello_12_1.0_u_m_[f:6,]' \
    'othello_12_1.0_f_o_[f:6,]' \
    'othello_12_1.0_f_n_[f:6,]' \
    'othello_12_1.0_f_c:10_[f:6,]' \
    'othello_12_1.0_f_c:100_[f:6,]' \
    'othello_12_1.0_f_c:1000_[f:6,]' \
    -w 100
