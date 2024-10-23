import pytest
from dataclasses import dataclass
from subprocess import Popen, call
from os import path
from time import sleep


@dataclass
class WebservConfig:
    path: str
    args: list[str]
    ports: list[int]


@pytest.fixture()
def webserv_instance(webserv_config: WebservConfig):
    proc = Popen(
            args=[webserv_config.path] + webserv_config.args,
            shell=False
        )
    sleep(1)
    yield proc, webserv_config
    sleep(1)
    
    proc.kill()
