import pytest
from dataclasses import dataclass
from subprocess import Popen, call
from os import path


@dataclass
class WebservConfig:
    path: str
    args: list[str]
    ports: list[int]


@pytest.fixture
def webserv_config() -> WebservConfig:
    base_path = path.dirname(path.realpath('__file__'))

    config = WebservConfig(
        path=base_path + "/app",
        args=["test.conf"], 
        # This should match the ports specified in the config file.
        ports=[9090, 9091]
        )
    return config




@pytest.fixture()
def webserv_instance(webserv_config: WebservConfig):
    proc = Popen(
            args=[webserv_config.path] + webserv_config.args,
            shell=False
        )
    yield proc
    
    proc.kill()
