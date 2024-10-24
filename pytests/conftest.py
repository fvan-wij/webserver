# Main config file that all tests will inherit from
from pathlib import Path
import pytest
from dataclasses import dataclass
from typing import Type
from subprocess import Popen
from time import sleep
from enum import Enum



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

class LogLevel(Enum):
    NONE = 1
    ERROR = 2
    ALL = 3

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
            proc=proc,
            )
    sleep(1)
    
    proc.kill()


# TODO Add cmd parameter to set child proc loglevel
def pytest_addoption(parser):
    parser.addoption(
            "--child-stdout-level",
            action="store",
            default=LogLevel.NONE.name,
            help="child stdout level: [none|error]",
            choices=("none", "error", "all"),
            )


def pytest_generate_tests(metafunc):
    # This is called for every test. Only get/set command line arguments
    # if the argument is specified in the list of test "fixturenames".
    option_value = metafunc.config.option.child_stdout_level
    if 'child_stdout_level' in metafunc.fixturenames and option_value is not None:
        metafunc.parametrize("child_stdout_level", [option_value])

