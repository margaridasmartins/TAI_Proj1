# TAI_Proj1

## Installing

This library is required to plot the graphs with `sciplot`.
```
sudo apt-get install gnuplot
```

## Compiling

Execute the following line on the **first time**, to create the `Makefile`. 
This will fetch `sciplot`, meaning there is no need for additional installations.
```
cmake src/
```

Afterwards, to compile there is only need to execute this line.
```
cd src/ && make
```

## Running

#### Finite-Context Model
```
cd bin/ && ./fcm <file-from-example-folder> <context-size> <alpha> 
# example
./fcm example 2 2 
```

#### Generator
```
cd bin/ && ./generator <file-from-example-folder> <context-size> <alpha> <prior>
# example
./generator example 2 2 aa
```
