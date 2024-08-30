https://stackoverflow.com/a/3233022
https://www.baeldung.com/cs/tcp-fin-vs-rst
https://stackoverflow.com/a/12925016
https://thelinuxcode.com/setsockopt-function-c/



webserv -> server(s) ->

webserv
- loads config file
- poll loop which iterates over all the `server` instances
- for server in servers;
    for fds in server.getfds:
        poll (fds)
        if (event)
        if (event == dataIn)
        {
            server.handleInEvent()
        }
        else if (event == dataOut)
        {
            server.handleOutEvent()
        }


server 
- runs on specific port(s) (so has its own socket)
- can have multiple `location` routings
- list of client/listening fds


route







plan
1. fixed response
2. read body content from file
3. try to upload file
