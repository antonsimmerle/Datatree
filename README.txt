Datatree

Anton Simmerle
28/12/2023

The Datatree repository is a simple tree-structure automator written in C.

Usage:

$ gcc -o main main.c
$ ./main input.txt

Input (input.txt):

* Add a whitespace before the node name to go up a level

```
title
 heading
 heading
  body
  body
 heading
 heading
  body
title
 heading
  body
 heading
```

Output (console):

* Output will appear colored (yellow, cyan, white)

```
Datatree

├─ title
│   ├─ heading
│   ├─ heading
│   │   ├─ body
│   │   └─ body
│   ├─ heading
│   └─ heading
│       └─ body
└─ title
    ├─ heading
    │   └─ body
    └─ heading

Total Nodes: 12
```