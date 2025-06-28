https://www.sigbus.info/compilerbook

## Self-hosting 9cc

First build the compiler using the system toolchain:

```bash
make
```

This produces an executable named `9cc`. You can then compile the
compiler with itself using the `selfhost.sh` script:

```bash
./selfhost.sh
```

The script invokes `./9cc` for every `*.c` file to generate assembly,
assembles each file with `cc -c` and links the resulting objects into
`9cc-self`. The resulting `9cc-self` is a self-hosted build of the
compiler.
