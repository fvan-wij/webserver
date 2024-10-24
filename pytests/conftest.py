# Main config file that all tests will inherit from
from pathlib import Path
import pytest
from dataclasses import dataclass
from typing import Type
from subprocess import Popen
from time import sleep



@dataclass
class WebservConfig:
    path: str
    args: list[str]
    ports: list[int]
    url: str

@dataclass
class WebservInstance:
    config: WebservConfig
    proc: Type[Popen]

def search_upwards_for_file(filename):
    d = Path.cwd()
    root = Path(d.root)

    while d != root:
        attempt = d / filename
        if attempt.exists():
            return attempt
        d = d.parent
    return None


@pytest.fixture()
def webserv_instance(webserv_config: WebservConfig) -> WebservInstance:
    proc = Popen(
            args=[webserv_config.path] + webserv_config.args,
            shell=False
        )
    sleep(1)
    yield WebservInstance(
            config=webserv_config,
            proc=proc
            )
    sleep(1)
    
    proc.kill()


# TODO Add cmd parameter to set child proc loglevel
def pytest_addoption(parser):
    parser.addoption("--name", action="store", default="default name")



