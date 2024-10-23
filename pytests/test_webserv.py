from conftest import WebservConfig
from time import sleep

def test_webserv_instance(webserv_instance: int) -> None:
    print(f"pid: {webserv_instance.pid}")
    sleep(10)
