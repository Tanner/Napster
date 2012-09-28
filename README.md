Project 1
---------
Tanner Smith

902706179

tsmith84

Protocol Definition
-------------------
All messsages are begin with a command and are ended with a new line. Between the two are various arguments required by the message.

All commands must be in all uppercase letters. If a command has arguments, it must be separated from the arguments by a space. If there are no arguments, the command can simply be followed by `\n`.

## Adding a File
### Request
`ADD path\n`

The file path must not contain spaces. Any other arguments will be ignored.

## Removing a File
### Request
`REMOVE path\n`

The file path must not contain spaces. Any other arguments will be ignored.

## Listing all Files
### Request
`LIST\n`

The list message takes no arguments.

### Response
The server responds with multiple commands depending on how many files are currently on the server.

`LIST # file file file\n`

The number is the number of files in base 10 to expect. The rest of the remaining arguments are the files available on the server. The files will be separated by spaces.

If none are known, then the server replies with `LIST 0\n`.

Known Issues
------------
- Client and Server may have memory leaks.
- File paths cannot have spaces.
- Rare high CPU usage (unknown cause and not replicatable).

Resources
---------
http://cs.baylor.edu/~donahoo/practical/CSockets/textcode.html
