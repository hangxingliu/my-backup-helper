# My Backup Helper

My backup helper program. (Load Configurations then compress using **7za**)

It has been tested on **Ubuntu 17.04** and ****. 

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

## Configuration File Description

``` json
{
	"// this is a valid comment": "comment",
	"target_dir": [
		"// you can also write comments like this line",
		"/path/to/backup_files_save_to_dir"
	],
	"configurations": {
		"config_name": {
			"// invoke 7z with sudo when sudo option is true": "comment",
			"sudo": false,

			"// set this option if your backup target archived file need encrypted": "comment",
			"// you need input password before program create archived file": "comment",
			"// and its sha1sum should be same with the value of this option": "comment",
			"password_sha1": "sha1sum. you can get it from command `backup-helper sha1sum`",

			"prefix": "prefix of backup file name",

			"type": "type of backup file, for example: tar, 7z, zip",

			"// compression level is from 0 to 9. greater level, smaller filesize of archived file.": "comment",
			"// default value is auto decided by 7z": "comment",
			"compression_level": 0,

			"description": "you can write description in here",

			"files": [
				"/path/to/backup_dir1",
				"/path/to/backup_dir2",
				"$HOME/Videos", "// System variables could be embedded into"
				"// ... more directories"
			],

			"exclude": [ "backup_dir1/pornhub", "// ..." ],

			"exclude_recursive": [ "CMakeFiles", "*.avi", "// ..." ],
		},
		"config_name2": {
			"// ...": "comment"
		}
	}
}
```

## Author

[Liu Yue @hangxingliu](https://github.com/hangxingliu)

## Third Party Libraries

- [vog/sha1](https://github.com/vog/sha1) for sha1 encoding
- [vivkin/gason](https://github.com/vivkin/gason) for parsering JSON [**MIT**]

## License

[GPL-3.0](LICENSE)
