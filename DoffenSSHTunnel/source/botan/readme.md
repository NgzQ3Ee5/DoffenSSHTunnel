# Botan 3 – Amalgamation Build (`botan_all.h` / `botan_all.cpp`)

This document describes how to generate the `botan_all.h` and `botan_all.cpp` amalgamation files

The amalgamation build embeds Botan directly into the source tree, avoiding external runtime dependencies and simplifying cross-platform builds.

---

## 1. Download Botan 3

Download the latest stable Botan 3 release from:

https://botan.randombit.net/#botan3

Extract the archive to directory `C:\QtProjects`

---

## 2. Exexute ant build task

The following commands generate a **static, minimized amalgamation build** with only the required modules enabled.

In directory `C:\QtProjects\DoffenSSHTunnel\build`

Execute `build_botan.bat`

Updates the files in `C:\QtProjects\DoffenSSHTunnel\DoffenSSHTunnel\source\botan`
