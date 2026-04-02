# Finance Tracker

Desktop finance tracker written in **C++** with a **wxWidgets GUI** and **MySQL** persistence.

## Tech stack

- **Language:** C++17
- **GUI framework:** wxWidgets
- **Database:** MySQL / MariaDB (via `libmysqlclient`)
- **Password hashing:** libsodium (`crypto_pwhash`)
- **Build tool:** Make + clang++
- **Schema setup:** SQL scripts in `queries/`

## Project structure

- `src/` – application source files
- `include/` – headers
- `queries/` – SQL scripts for creating database tables
- `makefile` – build instructions
- `bin/` – compiled binary output (`bin/comp` after build)

## Prerequisites

Install the following:

1. **clang++** (with C++17 support)
2. **make**
3. **wxWidgets development package** (must provide `wx-config` *or* `wxwidgets` pkg-config metadata)
4. **MySQL client development library** (`libmysqlclient`)
5. **libsodium development library**
6. A running **MySQL/MariaDB server**

> The build tries `wx-config`, `wx-config-gtk3`, and versioned `wx-config-*` binaries first, then falls back to `pkg-config` (`wxwidgets` / `wxgtk3.2`).

## Configuration

The app reads database configuration from a `.env` file in the project root.

Create `.env`:

```env
DB_HOST=127.0.0.1
DB_USER=root
DB_PASSWORD=your_password
DB_NAME=manage_spendings
DB_PORT=3306
```

### Notes

- On startup, the app loads `.env` and tries to connect to MySQL.
- It will create the database `manage_spendings` if it does not exist.
- It then executes all SQL scripts from `queries/` to ensure required tables are created.

## Build and run

From the repository root:

```bash
make
```

This creates the executable at:

```bash
bin/comp
```

Run it:

```bash
./bin/comp
```

Clean build artifacts:

```bash
make clean
```

## Typical first run

1. Start your MySQL/MariaDB server.
2. Create/update `.env` with valid DB credentials.
3. Build with `make`.
4. Launch `./bin/comp`.
5. Register a new user in the GUI, then log in.

## Troubleshooting

- **`wx-config: command not found`**
  - Install wxWidgets development packages and ensure one of these is available:
    - `wx-config`/`wx-config-gtk3`/`wx-config-3.2`
    - `pkg-config` metadata for `wxwidgets` or `wxgtk3.2`
- **MySQL connection error on startup**
  - Verify host/user/password/port in `.env`.
  - Confirm DB server is running and accessible.
- **Linker errors for mysql/sodium**
  - Install `libmysqlclient` and `libsodium` development libraries.
