# Finance Tracker

Desktop finance tracker written in **C++** with a **wxWidgets GUI** and **MySQL/MariaDB** persistence.

## Tech stack

- **Language:** C++17
- **GUI framework:** wxWidgets
- **Database:** MySQL/MariaDB (`libmysqlclient`/`libmariadb`)
- **Password hashing:** libsodium (`crypto_pwhash`)
- **Build tool:** Make + clang++

## Project structure

- `src/` - application source files
- `include/` - headers
- `queries/` - SQL scripts for schema creation
- `Makefile` - build instructions
- `bin/` - compiled output (`bin/comp`)

## Quick start (Ubuntu)

From project root:

```bash
sudo apt update
sudo apt install -y clang make libwxgtk3.2-dev libmariadb-dev libsodium-dev mariadb-server mariadb-client
sudo systemctl enable --now mariadb
```

Create database and an app user:

```bash
sudo mariadb -e "CREATE DATABASE IF NOT EXISTS manage_spendings;"
sudo mariadb -e "CREATE USER IF NOT EXISTS 'finance_user'@'127.0.0.1' IDENTIFIED BY 'finance_pass';"
sudo mariadb -e "GRANT ALL PRIVILEGES ON manage_spendings.* TO 'finance_user'@'127.0.0.1'; FLUSH PRIVILEGES;"
```

Create `.env` in project root:

```env
DB_HOST=127.0.0.1
DB_USER=finance_user
DB_PASSWORD=finance_pass
DB_NAME=manage_spendings
DB_PORT=3306
```

Build and run:

```bash
make
./bin/comp
```

## Configuration

The app reads DB settings from `.env` in project root.

If `.env` is missing, fallback defaults are:

- `DB_HOST=127.0.0.1`
- `DB_USER=root`
- `DB_PASSWORD=` (empty)
- `DB_NAME=manage_spendings`
- `DB_PORT=3306`

On startup, the app:

1. Loads `.env` (if present)
2. Connects to MySQL/MariaDB
3. Creates database `manage_spendings` if it does not exist
4. Executes SQL files in `queries/`

## Build

```bash
make
```

Output binary:

```bash
bin/comp
```

Clean artifacts:

```bash
make clean
```

## Troubleshooting

- **`wx-config not found`**
  - Install wxWidgets dev package:
  - `sudo apt install -y libwxgtk3.2-dev`

- **Startup error: `Can't connect to server on '127.0.0.1'`**
  - Server is not running or not installed.
  - Install/start MariaDB:
    - `sudo apt install -y mariadb-server mariadb-client`
    - `sudo systemctl enable --now mariadb`

- **Startup error: `Access denied for user ...`**
  - `.env` credentials are wrong or user lacks privileges.
  - Recreate/grant user as shown in Quick start.

- **Linker/headers errors for mysql/sodium**
  - Install dev libs:
  - `sudo apt install -y libmariadb-dev libsodium-dev`

- **Verify DB credentials manually**

```bash
mariadb -h 127.0.0.1 -P 3306 -u finance_user -pfinance_pass -e "SELECT 1;"
```

## Notes

- `.env` is ignored by git (`.gitignore`) so secrets are not committed.
- If you prefer MySQL server instead of MariaDB, equivalent setup works as long as client/server are reachable from `.env`.
