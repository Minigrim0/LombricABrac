# SERVER

## _Make commands_

#### **Simple compilation**
Type the following :
```
    make
    ./server <port>
```

#### **Compile + run**
Type the following :
```
    make run
```

#### **Compile + run with valgrind**
Type the following :
```
    make valrun
```

#### **Initialize data base**

Without filling it :
```
    make init_db
    ./init_db
```

With some random useless data in it :
```
    make init_db
    ./init_db true
```

#### **Clean**
```
    make clean
```

#### **Fresh start**
```
    make clean all
```
