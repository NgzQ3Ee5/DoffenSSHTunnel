# Botan 3 – Amalgamation Build (`botan_all.h` / `botan_all.cpp`)

This document describes how to generate the `botan_all.h` and `botan_all.cpp` amalgamation files used by this project.

The amalgamation build embeds Botan directly into the source tree, avoiding external runtime dependencies and simplifying cross-platform builds.

---

## 1. Download Botan 3

Download the latest stable Botan 3 release from:

https://botan.randombit.net/#botan3

Extract the archive to a local working directory.

---

## 2. Generate `botan_all.h` and `botan_all.cpp`

The following commands generate a **static, minimized amalgamation build** with only the required modules enabled.

Enabled modules:
- `cryptobox`
- `bcrypt`
- `auto_rng`
- `system_rng`
- `entropy`

### Windows (MSVC)

Copy the generated files to `./win`.

```
python configure.py --cc=msvc --amalgamation --disable-shared --minimized-build --enable-modules=cryptobox,bcrypt,auto_rng,system_rng,entropy
```

### Linux

Copy the generated files to `./linux`.

```
python configure.py --cc=gcc --amalgamation --disable-shared --minimized-build --enable-modules=cryptobox,bcrypt,auto_rng,system_rng,entropy
```

### Mac

Copy the generated files to `./mac`.

```
python configure.py --cc=clang  --amalgamation --disable-shared --minimized-build --enable-modules=cryptobox,bcrypt,auto_rng,system_rng,entropy
```
