# Hachcode2017
Hashcode competition at FMF

## Scoring
You can score your solution by writing

```bash
python scoring.py example.in example.out
```

## Simple fill solution
```bash
python fill.py < example.in > example.out
```

## CPP greedy solution
There are two greedy solutions: greedy1 and greedy2, we will mark greedyX
```bash
cd solvers/greedy
mkdir build
cd build
cmake ..
make greedyX
./greedyX < example.in > example.out
```
