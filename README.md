# LinM

## Introduction

LinM is a visual file manager for the terminal.

It's a feature-rich full-screen text mode application that allows you to copy, move and
delete files and whole directory trees, search for files and run commands in the sub-shell.

LinM supports FTP, SFTP, and Samba remote connections, and can view tar, zip, alz, rpm, deb,
rar, and iso archives directly.

It's a clone of Mdir, the famous file manager from the MS-DOS age.
LinM inherits the keyboard shortcuts and the screen layout from Mdir to maximize user-friendliness.

For bug reports, comments and questions, please email to la9527@daum.net

## System Requirements

* **cmake** (version 3.10 or higher)
* **ncurses / ncursesw** (version 5.3 or higher) - ncursesw preferred for UTF-8 support
* **openssl** (version 1.0 or higher) - for SFTP support
* **libssh2** - for SFTP support
* **libsmbclient** (samba) - for SMB support (optional)

### Ubuntu / Debian

```bash
sudo apt install build-essential cmake libncursesw5-dev libssl-dev libssh2-1-dev libsmbclient-dev
```

### Fedora / RHEL

```bash
sudo dnf install gcc-c++ cmake ncurses-devel openssl-devel libssh2-devel libsmbclient-devel
```

## Building LinM

```bash
mkdir build
cd build
cmake ..
make -j$(nproc)
```

### CMake Options

| Option | Description |
|--------|-------------|
| `-DCMAKE_INSTALL_PREFIX=PATH` | Change the installation prefix (default: `/usr/local`) |
| `-DPTHREAD_ENABLE=OFF` | Build without pthread support |
| `-DCMAKE_BUILD_TYPE=Debug` | Enable debug build |

### Install

```bash
sudo make install
```

To have the shell prompt return to the directory where you ended LinM,
re-login after installation.

## Supported Platforms

* Linux (Ubuntu, Debian, Fedora, RHEL, etc.)
* macOS
* FreeBSD / OpenBSD

## Supported Archive Formats

tar, tar.gz, gz, bz2, tar.bz2, rpm, zip, deb, alz, jar, rar, iso

## Keyboard Shortcuts

### File Panel

| Key | Action |
|-----|--------|
| `/` | Shell command |
| `ESC` | Show command screen |
| `\|` | Go to home directory |
| `\` | Go to root directory |
| `BS` | Go to parent directory |
| `Alt+C` | Copy files |
| `Alt+D` | Delete files |
| `Alt+K` | Make directory |
| `Alt+R` | Rename |
| `Alt+V` | View file |
| `Alt+S` | Sort files |
| `Alt+Z` | Toggle hidden files |
| `Alt+H` | Chmod |
| `Alt+X` / `Ctrl+Q` | Exit LinM |
| `Ctrl+A` | Select all |
| `Ctrl+W` | Toggle split screen |
| `TAB` | Next window |
| `Ctrl+R` | Remote connect (SFTP/FTP/Samba) |
| `Ctrl+O` | Subshell (MC style) |
| `Ctrl+F` | File find |
| `F1`-`F12` | Help, Rename, View, Editor, Refresh, Move, Mkdir, Remove, Compress, MCD, QCD, Menu |

### Editor

| Key | Action |
|-----|--------|
| `Ctrl+N` | New document |
| `Ctrl+S` | Save |
| `Ctrl+C` / `Ctrl+X` / `Ctrl+V` | Copy / Cut / Paste |
| `Ctrl+Z` | Undo |
| `Ctrl+F` | Find |
| `Ctrl+G` | Go to line |
| `Alt+F` / `Alt+E` | Go to first / last line |

## Homepage

https://github.com/onionmixer/linm-neo

## License

LinM is distributed under the GNU General Public License Version 3.
See [LICENSE](LICENSE) for details.

## Changes

### v0.9.1

Modernized for Ubuntu 22.04+ and GCC 12+ environments.
Comprehensive code quality improvements (60+ bug fixes):

- **Memory safety**: Fixed NULL dereferences, buffer overflows, uninitialized variables, use-after-free, and memory leaks across the entire codebase.
- **Threading**: Fixed `pthread_cond_timedwait` to use absolute time, added proper mutex lock/unlock pairing, added `va_copy` for `va_list` reuse.
- **Logic errors**: Fixed `operator<` self-comparison, `&&` vs `||` range checks, `dup2` argument order, switch fall-through bugs.
- **Security**: Replaced `system()` shell injection with `fork`+`exec` and safe C APIs. Replaced `inet_ntoa` with `inet_ntop`.
- **Type safety**: Changed `ullong` from `long long` to `unsigned long long`. Used `ssize_t` for error detection from `libssh2_sftp_read`.
- **Build fixes**: Fixed `sizeof(pointer)` bug after VLA-to-vector conversion. Replaced deprecated `libssh2_session_startup` with `libssh2_session_handshake`.
- **Dead code**: Fixed semicolons that truncated tar extract `-C` directory option. Fixed format string argument count mismatches.

### v0.9.0 - 2017-08-24

- Build packaging changed from automake to cmake.
- Fixed compile warnings.
- Fixed syntax highlighter in internal editor.
