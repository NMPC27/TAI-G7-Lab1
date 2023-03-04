# TAI-G7-Lab1

## Requirements

- CMake >= 3.10

## Building

In order to build the project, first create a `build` directory and generate a CMake native build environment.
If using Unix, run:

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

...

### `cpm-gen`

...

## TODO

Tasks within each category are ordered in terms of urgency of development (simpler approaches first).

- Assign different distributions to the symbols other than the one being predicted:
  **- Uniform distribution**
  - Distribution using static probabilities of each symbol (probability of symbol appearing in the _whole_ file)
- Use different thresholds for changing the copy pointer:
  **- Static value (probability of hit being lower than a value)**
  - Given number of successive fails
  - Rate of change of the probability of hit
- Choice of next pointer when copy pointer is changed:
  **- Next pointer closest to the changed copy pointer**
  - Most recent
- Data structures:
  - Circular array for the copy context (no append(end) -> remove(first) needed)
- File reading strategy:
  **- Keep file contents in memory**
  - Read file in binary
- Possible bugs:
  - Not working with UTF-8
  - Check how the first few characters should be handled (for k=4, what are the probabilities for the first 3 characters?)
