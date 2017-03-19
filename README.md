# SatSolvers

## 1. Budowanie solverów

1. Glucose
```
cd solvers/glucose/simp/
make rs
```

2. Lingeling
```
cd solvers/lingeling
./configure && make
```

3. Minisat
```
cd solvers/minisat/simp/
make rs
```

## 2. Konfiguracja solverów
Linki symboliczne:

```
ln -s full_path/solvers/glucose/simp/glucose_static /usr/bin/glucose
ln -s full_path/solvers/lingeling/lingeling /usr/bin/lingeling
ln -s full_path/solvers/minisat/simp/minisat_static /usr/bin/minisat
```

