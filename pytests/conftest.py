import pytest
from dataclasses import dataclass
from typing import Type
from subprocess import Popen, call
from os import path
from time import sleep


@dataclass
class WebservConfig:
    path: str
    args: list[str]
    ports: list[int]

@dataclass
class WebservInstance:
    config: WebservConfig
    proc: Type[Popen]


@pytest.fixture()
def webserv_instance(webserv_config: WebservConfig) -> WebservInstance:
    proc = Popen(
            args=[webserv_config.path] + webserv_config.args,
            shell=False
        )
    sleep(1)
    yield WebservInstance(config=webserv_config, proc=proc)
    sleep(1)
    
    proc.kill()
