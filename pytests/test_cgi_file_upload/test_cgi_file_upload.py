import pytest
from time import sleep
from ..conftest import WebservInstance
import requests
import os.path

def upload_file_using_CGI(port : int):
    url = "http://localhost:"
    url += str(port)
    url += "/cgi-bin"


def test_webserv_instance_pid_check(webserv_instance: WebservInstance) -> None:
    assert webserv_instance.proc.pid is not 0
