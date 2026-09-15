# mod-twow-bot-gear

Generated from `modules/templates/basic`.

## Layout

- `src/` contains C++ scripts.
- `conf/mod-twow-bot-gear.conf.dist` contains module config defaults. Copy or move it to `mod-twow-bot-gear.conf` in the installed module config directory before enabling the module.
- `data/sql/auth`, `data/sql/character`, and `data/sql/world` are optional module migration folders.
- `mod-twow-bot-gear.cmake` is optional and only needed for custom CMake hooks.

## Build

Use one of:

```sh
cmake -S . -B build -DMODULES=static
cmake -S . -B build -DMODULES=dynamic
cmake -S . -B build -DMODULE_MOD_TWOW_BOT_GEAR=static
```

The exact per-module cache variable is printed by CMake during configuration.
