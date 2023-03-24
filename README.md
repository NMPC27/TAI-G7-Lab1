# TAI-G7-Lab1

## Requirements

- CMake >= 3.10

## Building

In order to build the project, first create a `build` directory and generate a CMake native build environment.
If using Linux, run:

```
mkdir build
cd build
cmake ..
```

After that, build the binaries, within the `build` directory:

```
cmake --build .
```

The generated binaries will be present in the `bin` folder.

## Usage

### `cpm`

To use the copy model, run:

```
./bin/cpm <k> <file>
```
#### Example runs

Best run
```
./run.sh -k 12 -x -a 1 -r m -t f:6 ./example/chry.txt

Total amount of information: 4.2215e+07 bits | media: 1.8623

Time Taken: 2m30,314s
```

Default run for testing

```
./run.sh -k 12 -x -a 1 -r m -t f:3 ./example/chry.txt

Total amount of information: 4.23187e+07 bits | media: 1.86687
```


Changing -r
```
./run.sh -k 12 -x -a 1 -r o -t f:3 ./example/chry.txt

Total amount of information: 4.23537e+07 bits | media: 1.86842
```
```
./run.sh -k 12 -x -a 1 -r n -t f:3 ./example/chry.txt

Total amount of information: 4.24362e+07 bits | media: 1.87206
```


Changing -t f:x
```
./run.sh -k 12 -x -a 1 -r m -t f:2 ./example/chry.txt

Total amount of information: 4.23444e+07 bits | media: 1.86801
```
```
./run.sh -k 12 -x -a 1 -r m -t f:4 ./example/chry.txt

Total amount of information: 4.22495e+07 bits | media: 1.86382
```
```
./run.sh -k 12 -x -a 1 -r m -t f:5 ./example/chry.txt

Total amount of information: 4.23222e+07 bits | media: 1.86703
```
```
./run.sh -k 12 -x -a 1 -r m -t f:6 ./example/chry.txt

Total amount of information: 4.2215e+07 bits | media: 1.8623
```

Changing -t n:x
```
./run.sh -k 12 -x -a 1 -r m -t n:0.5 ./example/chry.txt

Total amount of information: 4.35485e+07 bits | media: 1.92112
```
```
./run.sh -k 12 -x -a 1 -r m -t n:0.4 ./example/chry.txt

Total amount of information: 4.47532e+07 bits | media: 1.97427
```
```
./run.sh -k 12 -x -a 1 -r m -t n:0.3 ./example/chry.txt

Total amount of information: 4.48986e+07 bits | media: 1.98068
```

Changing -t c:x (verificar, demora 10 vezes mais os outros thresholds que os outros)
```
./run.sh -k 12 -x -a 1 -r m -t c:0.001 ./example/chry.txt

Total amount of information: 4.05122e+07 bits | media: 1.78718
```
```
./run.sh -k 12 -x -a 1 -r m -t c:0.01 ./example/chry.txt

Total amount of information: 4.05122e+07 bits | media: 1.78718

time taken: 19m29,093s
```

Changing -a
```
time ./run.sh -k 12 -x -a 10 -r m -t f:6 ./example/chry.txt

Total amount of information: 4.31398e+07 bits | media: 1.90309

time taken: 2m23,550s
```
```
time ./run.sh -k 12 -x -a 100 -r m -t f:6 ./example/chry.txt

Total amount of information: 4.35363e+07 bits | media: 1.92059

time taken: 2m23,352s
```
```
time ./run.sh -k 12 -x -a 0.1 -r m -t f:6 ./example/chry.txt

Total amount of information: 4.47932e+07 bits | media: 1.97604

time taken: 2m0,562s
```
```
time ./run.sh -k 12 -x -a 2 -r m -t f:6 ./example/chry.txt

Total amount of information: 4.23265e+07 bits | media: 1.86722

time taken: 2m20,142s
```


...

### `cpm-gen`

...
