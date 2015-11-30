#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <error.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <math.h>

void usage(char* prog) {
    printf("Usage: %s inputFile\n", prog);
    exit(1);
}

int main(int argc, char** argv) {
    int fd;
    size_t num, randomSize, numerator;
    double min, max, normDiv, sigmaBar, sum;
    double *randomInput;
    char *buf, *curPos;
    struct stat fdStat;

    // Check the args
    if (argc != 2)
        usage(argv[0]);

    // Open the file
    if ((fd = open(argv[1], O_RDONLY)) == -1)
        error(errno, errno, "Error opening input file");
    if(fstat(fd, &fdStat) == -1)
        error(errno, errno, "Error opening input file");
    buf = reinterpret_cast<char*>(
        mmap(NULL, static_cast<size_t>(fdStat.st_size),
                PROT_READ, MAP_PRIVATE, fd, 0));
    if (buf == MAP_FAILED)
        error(errno, errno, "Error opening input file");

    // Parse the file
    randomSize = 256;
    randomInput = reinterpret_cast<double*>(
            malloc(sizeof(double) * randomSize));
    if (randomInput == NULL)
        error(errno, errno, "Error allocating memory");
    curPos = buf;
    num = 0;
    while(true) {
        double input;
        char *oldPos;
        oldPos = curPos;
        input = strtod(curPos, &curPos);
        if (errno == ERANGE)
            error(errno, errno, "Input out of range");
        if (input == 0 && curPos == oldPos && curPos - buf == fdStat.st_size) {
            if (curPos - buf == fdStat.st_size)
                break;
            else
                error(1, 0, "Input could not be read");
        }
        if (num == randomSize)
            if ((randomInput = reinterpret_cast<double*>(
                            realloc(randomInput, sizeof(double) *
                                (randomSize = randomSize * 2))))
                    == NULL)
                error(errno, errno, "Error allocating memory");
        randomInput[num++] = input;
        if (*curPos++ != '\n')
            error(1, 0, "Invalid Input");
    }
    if (num % 2 != 0)
        error(1, 0, "Input is not even");
    if ((randomInput = reinterpret_cast<double*>(
                    realloc(randomInput, sizeof(double) * num))) == NULL)
        error(errno, errno, "Error allocating memory");
    if (munmap(buf, static_cast<size_t>(fdStat.st_size)) == -1)
        error(errno, errno, "Error unmapping buffer");
    if (close(fd) == -1)
        error(errno, errno, "Error closing file");
    // Normalize the input
    min = DBL_MAX;
    max = DBL_MIN;
    for (size_t i = 0; i < num; ++i) {
        double cur = randomInput[i];
        if (cur < min)
            min = cur;
        else if (cur > max)
            max = cur;
    }
    normDiv = max - min;
    for (size_t i = 0; i < num; ++i) {
        randomInput[i] = (randomInput[i] - min) / normDiv;
    }
    // Do the estimation
    numerator = 0;
    sum = 0.0;
    for (size_t i = 0; i < num; i += 2) {
        double x, y;
        x = randomInput[i];
        y = randomInput[i + 1];
        if (sqrt(x * x + y * y) <= 1)
            ++numerator;
        sum += x + y;
    }
    // Find the error
    sigmaBar = sqrt(sum / static_cast<double>(num * (num - 1)));
    free(randomInput);
    printf("Estimate of pi: %f+%e\n",
            static_cast<double>(numerator) / static_cast<double>(num) * 8,
            sigmaBar);
    return 0;
}
