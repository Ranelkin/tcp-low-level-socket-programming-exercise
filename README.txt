Exercise: Modify the programs as follows:
The server does not automatically send a time stamp when a new incoming connection is established, but first waits for a command of the form GET\n to be received (\n denotes a line feed in C notation). Only when this command has been read is the timestamp created and sent via the same connection.
Accordingly, the client should first write the GET\n command after the connection is established and then wait for the response from the server, read it out and then output it as text.
The server should not close the connection automatically (except in the event of an error), but wait for further commands. The client should close the connection once the time stamp has been read.

Running the programm: Just run 'make' in your terminal 