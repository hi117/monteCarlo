import random
import sys

with open('testInput.txt', 'w') as f:
    for i in range(int(sys.argv[1])):
        f.write(str(random.random()) + '\n')
