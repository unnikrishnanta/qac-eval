""" Generate random strings from a collection file
with the given length distribution uniformly sampled
from an alphabet set """

import sys
if sys.version_info[0] < 3:
    raise Exception("Must be using Python 3")
import random

SIGMA = '0123456789abcdefghijklmnopqrstuvwxyz'
SPACE = '_'

def random_str(sl, wl_list):
    """Generate a random string with the given word length distribution
    using the alphabets from Sigma.
    wl_list: list of word lengths in the original string
    sl : total length (including trailing whitespace) of the original string.
    """
    rw_list = list()
    for wl in wl_list:
        rw = ''.join(random.choice(SIGMA) for x in range(wl))
        rw_list.append(rw)

    random_str = SPACE.join(rw_list)
    if len(random_str) < sl: # Trailing whitespace
        random_str += SPACE

    return random_str


def main():
    skip_header = True
    str_list = list()
    slen_list = list()
    scores_list = list()
    for line in sys.stdin:
        if skip_header:
            skip_header = False
            continue
        stripped = line.strip()
        if not stripped:
            continue
        line_split = stripped.split()
        scores_list.append(int(line_split[1]))
        string = line_split[0]
        str_list.append(string.split(SPACE))
        slen_list.append(len(string))

    i = 0
    print('title\tscore')
    for wl_list in map(lambda x: map(len, x), str_list):
        rs = random_str(slen_list[i], wl_list)
        print(rs,'\t',scores_list[i])
        i += 1


if __name__ == "__main__":
    main()
