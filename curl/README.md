This is just a simple setup to run the POST/GET requests over cURL instead through the browser.

The following command will read the `.http` files and make a request.
```
curl -H @curl/post_header.http --data "@curl/post_body.http" http://localhost:9090/cgi-bin/hello_world.py -v
```
