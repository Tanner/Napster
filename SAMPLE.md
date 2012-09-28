Client
------
````
$ client 127.0.0.1 4006
(napster) list
Server File List:
No files.
(napster) add kitty.txt
(napster) list
Server File List:
1) kitty.txt

(napster) remove kitty.txt
(napster) list
Server File List:
No files.
(napster) quit
````

Server
------
````
$ server 4006
Server started...
Got request from 127.0.0.1 – LIST
Got request from 127.0.0.1 – ADD
	Creating new source...
Updated file source list:
	127.0.0.1:
		kitty.txt
Got request from 127.0.0.1 – LIST
Sending response back to 127.0.0.1 – LIST
Got request from 127.0.0.1 – REMOVE
	Removed 1 files.
Updated file source list:
	127.0.0.1:
Got request from 127.0.0.1 – LIST
Sending response back to 127.0.0.1 – LIST
````