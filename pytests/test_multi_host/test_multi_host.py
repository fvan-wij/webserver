import pytest
from time import sleep
from ..conftest import WebservInstance
import requests

def send_to_host(host: str, url: str):
    headers = {"Host": host}
    response = requests.get(url, headers=headers)
    print(f"Response for {host}: {response.text}")
    assert response.status_code == 200 and response.headers.get("Virtual-Host") == host

def test_webserv_instance_pid_check(webserv_instance: WebservInstance) -> None:
    assert webserv_instance.proc.pid is not 0
    url = "http://localhost:" + str(webserv_instance.config.ports[0])
    send_to_host("a.org", url)
    send_to_host("b.org", url)
    send_to_host("localhost", url)
