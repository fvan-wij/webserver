 #!/bin/bash
# curl -X POST localhost:8080/cgi-bin
# curl -X POST localhost:8081/cgi-bin
# curl -X POST localhost:9090/cgi-bin
# curl -X POST localhost:9091/cgi-bin

# seq 1 200 | xargs -n1 -P10  curl -X POST "localhost:9091/cgi-bin"
seq 1 200 | xargs -Iname -P10  curl -X POST "localhost:9091/cgi-bin"
# curl -X POST localhost:8080/cgi-bin
# sleep 0.5
# curl -X POST localhost:8081/cgi-bin
# sleep 0.5
# curl -X POST localhost:9090/cgi-bin
# sleep 0.5
# curl -X POST localhost:9091/cgi-bin
# sleep 0.5
