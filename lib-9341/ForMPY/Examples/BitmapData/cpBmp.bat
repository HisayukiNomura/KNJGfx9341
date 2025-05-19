rem This batch file uses rshell to copy a Python program to a Raspberry Pi Pico, which defines bitmap images.
rem It works the same as saving a file in Thonny, but Thonny can hang when saving large files, so rshell is used instead.
rem For more details on rshell, please visit: https://github.com/dhylands/rshell.

rshell -p COM17 --buffer-size=8192 cp <python bitmap files directory>/*.py /pyboard/