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
```

The path of the file to store data has to be specified as argument when running the program. Here `test.db` is the filename which will be created in `out` directory if it doesn't exist. If it exists then data in the file will be read and changes wil be written when `commited`.

## Commands

- #### `insert <key> <value>`  
  If the key doesn't already exist, the value is inserted into the table with the given key.

  ```
  [db]: insert 191639 Tharun.K

  [db]: insert 191750 K.Prajwal

  [db]: insert 191665 SomeITGuy
  ```
- #### `select [key]`  
  If key is specified and the key exists, the corresponding value is printed. If key is not specified then all the keys and values in the table are printed.

  ```
  [db]: select
  191639  : Tharun.K
  191665  : SomeITGuy
  191750  : K.Prajwal

  [db]: select 191665
  191665  : SomeITGuy
  ```

- #### `replace <key> <value>`  
  If the key exists, the value of the corresponding key will be replaced with the new one.

  ```
  [db]: replace 191665 Navaneeth.P

  [db]: select
  191639  : Tharun.K
  191665  : Navaneeth.P
  191750  : K.Prajwal
  ```

- #### `commit`  
  Save changes to file.

  ```
  [db]: commit
  ```

- #### `remove <key>`  
  If the key exists, it and it's corresponding key is removed from the table.

  ```
  [db]: remove 191665

  [db]: select
  191639  : Tharun.K
  191750  : K.Prajwal
  ```

- #### `rollback`  
  Discard changes made after the last commit.

  ```
  [db]: rollback

  [db]: select
  191639  : Tharun.K
  191665  : Navaneeth.P
  191750  : K.Prajwal
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