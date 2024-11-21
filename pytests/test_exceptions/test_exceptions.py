from ..conftest import LogLevel, WebservInstance
import requests
import psutil

def send_put_request(port: int, uri: str, expected_status_code: int):
    print(f"\nSending GET request to http://localhost:{str(port)}/{uri}")
    r = requests.put(f"http://localhost:{str(port)}/{uri}")
    print("Status code == ",  r.status_code)
    print("Expected code == ",  expected_status_code)
    assert(r.status_code) == expected_status_code


def test_webserv_instance_pid_check(webserv_instance: WebservInstance) -> None:
    assert webserv_instance.proc.pid is not 0
    for port in webserv_instance.config.ports:
        send_put_request(port, "/", 200)
