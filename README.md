Project 1
---------
Tanner Smith

902706179

tsmith84

Protocol Definition
-------------------
All messsages are begin with a command and are ended with a new line. Between the two are various arguments required by the message.

## Adding a File
### Request
`ADD path\n`

The add message must begin with 'ADD' in all uppercase letters followed by space and then the desired file path to the text file. The file path must not contain spaces.

## Removing a File
### Request
`REMOVE path\n`

The remove message must begin with 'REMOVE' in all uppercase lettesr followed by a space and then the desired file path to the text file to remove. The file path must not contain spaces.

## Listing all Files
### Request
`LIST\n`

The list message takes no arguments.

### Response
The server responds with multiple commands depending on how many files are currently on the server.

`LIST # file file file\n`

The number is the number of files to expect. The rest of the remaining arguments are the files available on the server.

If none are known, then the server replies with `LIST 0\n`.

Resources
---------
http://cs.baylor.edu/~donahoo/practical/CSockets/textcode.html
