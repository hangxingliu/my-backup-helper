# My Backup Helper

My backup helper program. (Load Configurations then compress using **7za**)

It has only been tested on **Ubuntu 17.04**. 

## Build

``` bash
./check.sh # check dependencies, install packages follow its output
./build.sh # build it by CMake
sudo ./install.sh # install it to /usr/local/bin/backup-helper
```

## Usage

1. Create configuration file in any one of :
	- `$HOME/.backup-config.json`
	- `/etc/backup-config.json`
2. Make backup rules refer to:
	- JSON schema file: `./config-schema/schema.json`
	- Example files: `./config-schema/test-config.json` or `./backup-config.debug.json`
3. Backup by executing `backup-helper $CONFIG_NAME ...`.
4. More help by executing `backup-helper --help`

## Author

Liu Yue

## Third Party Libraries

- [vog/sha1](https://github.com/vog/sha1) for sha1 encoding
- [vivkin/gason](https://github.com/vivkin/gason) for parsering JSON [**MIT**]

## License

[GPL-3.0](LICENSE)
