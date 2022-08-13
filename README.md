# inst_logger

This pintool logs instructions in a dynamic manner.

## Setup Pin

```bash
./install_pin.sh
```

## Build

```bash
make
```

## Run

```bash
./pin-3.20-98437-gf02b61307-gcc-linux/pin -t src/obj-intel64/inst_logger.so -o ls.csv -main_only 1 -- /bin/ls
```

Result would be like:

```
...
0x55555555a7e1,push rax
0x55555555a7e2,push rsp
0x55555555a7e3,lea r8, ptr [rip+0x10d66]
0x55555555a7ea,lea rcx, ptr [rip+0x10cef]
0x55555555a7f1,lea rdi, ptr [rip-0x1a08]
0x55555555a7f8,call qword ptr [rip+0x1c7d2]
...
```

### Options

- `-o` (required) : Output filename.
- `-main_only` : When set to `1`, logs instructions only in main binary. Otherwise, logs all instructions.
  - default : `1`, optional
- `-rel_addr` : When set to `1`, logs offset from image top as instruction address. `-main_only` must be set to `1` to enable this option.
  - default : `0`, optional

## ToDo

- [x] Add option for relative address.
- [ ] Support AT&T syntax.
