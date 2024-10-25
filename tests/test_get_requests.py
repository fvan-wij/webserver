import pytest
import requests

ports = ["8080", "8081", "9090", "9091"]

def send_get_request(port, uri, expected_status_code):
    url = "http://localhost:"
    url += port
    url += uri
    print("\nSending GET request to url", url)
    r = requests.get(url)
    print("Status code == ",  r.status_code)
    print("Expected code == ",  expected_status_code)
    assert(r.status_code) == expected_status_code

def test_get_request_root():
    for i in ports:
        send_get_request(i, "/", 200)

def test_get_request_index_html():
    for i in ports:
        send_get_request(i, "/index.html", 200)
