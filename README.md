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

### Options

- `-o` (required) : Output filename.
- `-main_only` : When set to `1`, logs instructions only in main binary. Otherwise, logs all instructions.

## ToDo

- [ ] Support AT&T syntax.
