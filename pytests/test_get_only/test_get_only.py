import pytest
import httpx
from time import sleep
from ..conftest import WebservInstance
import requests

def send_get_request(port: int, uri: str, expected_status_code: int):
    url = "http://localhost:"
    url += str(port)
    url += uri
    print("\nSending GET request to url", url)
    r = requests.get(url)
    print("Status code == ",  r.status_code)
    print("Expected code == ",  expected_status_code)
    assert(r.status_code) == expected_status_code


def test_webserv_instance_pid_check(webserv_instance: WebservInstance) -> None:
    assert webserv_instance.proc.pid is not 0

    # for child in children:
    #     print('Child pid is {}'.format(child.pid))
    # print(f"pid: {webserv_instance.proc.pid}")
    # send_get_request(webserv_instance.config.ports[0], "", 200)

