#Tests 200 CGI-bin triggers, 10 at a time
seq 1 200 | xargs -Iname -P10  curl -X POST "localhost:9091/cgi-bin"
