"""mapper.py"""
import sys

for line in sys.stdin:
    if line.startswith('id'):
        continue
    line = line.strip()
    line = line.split('\t')
    print(f'{1}\t{line[9]}')
