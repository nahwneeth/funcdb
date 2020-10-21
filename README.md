# funcdb

`IT 203 Project : Nov 2020 : NITK`

Building a simple database using B+trees.

## How to build?

```
python3 build.py
```

## Build successfully? How to run?

```
cd out
./funcdb.exe test.db

[db]: .help
```

The path of the file to store data has to be specified as argument when running the program. Here `test.db` is the filename which will be created in `out` directory if it doesn't exist. If it exists then data in the file will be read and changes wil be written when `commited`.

## Data Types
  - #### `i32`  
    Signed Integer of 32-bits.
  - #### `c100`
    String of atmost 100 chars. 

## Commands

- #### `create <ColName1>(i32) <ColName2>(<DataType2>) ...`
  The first column is always a unique key of type `i32`. This is the key used to store data in the B+tree. The rest of the columm can have different types and names. Column names cannot be repeated. A table can be created only once for a file.

  ```
  [db]: create RegNo(i32) Name(c100)
  ```

- #### `insert <key> <value> ...`  
  If the key doesn't already exist, the values are inserted into the table with the given key.

  ```
  [db]: insert 191639 Tharun.K

  [db]: insert 191750 K.Prajwal

  [db]: insert 191665 SomeITGuy
  ```
- #### `select [key]`  
  If key is specified and the key exists, the corresponding values are printed. If key is not specified then all the keys and values in the table are printed.

  ```
  [db]: select
  RegNo	: 191639
  Name	: Tharun.K

  RegNo	: 191665
  Name	: SomeITGuy

  RegNo	: 191750
  Name	: K.Prajwal

  [db]: select 191665
  RegNo	: 191665
  Name	: SomeITGuy
  ```

- #### `replace <key> <value> ...`  
  If the key exists, the values of the corresponding key will be replaced with the new ones.

  ```
  [db]: replace 191665 Navaneeth.P

  [db]: select
  RegNo	: 191639
  Name	: Tharun.K

  RegNo	: 191665
  Name	: Navaneeth.P

  RegNo	: 191750
  Name	: K.Prajwal
  ```

- #### `commit`  
  Save changes to file.
  ```
  [db]: commit
  ```

- #### `remove <key>`  
  If the key exists, it and it's corresponding values are removed from the table.

  ```
  [db]: remove 191665

  [db]: select
  RegNo	: 191639
  Name	: Tharun.K

  RegNo	: 191750
  Name	: K.Prajwal
  ```

- #### `rollback`  
  Discard changes made after the last commit.

  ```
  [db]: rollback

  [db]: select
  RegNo	: 191639
  Name	: Tharun.K

  RegNo	: 191665
  Name	: Navaneeth.P

  RegNo	: 191750
  Name	: K.Prajwal
  ```

- #### `.exit`  
  Quit the program discarding any unsaved changes.

  ```
  [db]: .exit
  ```

## Authors
  - K.Prajwal
  - Navaneeth.P
  - Tharun.K