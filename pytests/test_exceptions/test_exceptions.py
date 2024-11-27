from ..conftest import LogLevel, WebservInstance
import requests
import psutil
import os.path

def send_put_request(port: int, uri: str, expected_status_code: int):
    print(f"\nSending PUT request to http://localhost:{str(port)}/{uri}")
    r = requests.put(f"http://localhost:{str(port)}/{uri}")
    print("Status code == ",  r.status_code)
    print("Expected code == ",  expected_status_code)
    assert(r.status_code) == expected_status_code

def send_upload_request_wrong_content_length(port: int, file_path: str, expected_status_code: int):
    headers = {
        "Content-Length": "100"
        }  
    url = "http://localhost:"
    url += str(port)
    url += "/uploads"
    r = requests.post(url, headers=headers)
    print("Status code == ",  r.status_code)
    print("Expected code == ",  expected_status_code)
    assert(r.status_code) == expected_status_code or 400
    test_file_path = "./var/www/uploads/" + file_path
    print(r.request.headers)

def test_webserv_instance_pid_check(webserv_instance: WebservInstance) -> None:
    assert webserv_instance.proc.pid is not 0
    for port in webserv_instance.config.ports:
        send_put_request(port, "/", 405)
        send_upload_request_wrong_content_length(port, "", 400)
