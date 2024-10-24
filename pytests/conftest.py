# Main config file that all tests will inherit from
from pathlib import Path
import pytest
from dataclasses import dataclass
from typing import Type
from subprocess import Popen
from time import sleep
from enum import Enum


class LogLevel(Enum):
    NONE = 1
    ERROR = 2
    ALL = 3

@dataclass
class WebservConfig:
    path: str
    args: list[str]
    ports: list[int]
    url: str
    stdout_level: LogLevel

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


#`webserv_config` gets passed in from the child directories their `conftest.py`
@pytest.fixture()
def webserv_instance(webserv_config: WebservConfig) -> None:
    proc = Popen(
            args=[webserv_config.path] + webserv_config.args,
            shell=False
        )
    # TODO Make this smaller?
    sleep(1)
    yield WebservInstance(
            config=webserv_config,
            proc=proc,
            )
    sleep(1)
    
    proc.kill()


# Add `--child-stdout-level` parameter to set child `LogLevel`
def pytest_addoption(parser):
    parser.addoption(
            "--child-stdout-level",
            action="store",
            default=LogLevel.NONE.name,
            help="forward child's STDOUT",
            # Tering vieze python shit hiero.
            choices=([entry.name.lower() for entry in LogLevel]),
        )


def pytest_generate_tests(metafunc):
    # This is called for every test. Only get/set command line arguments
    # if the argument is specified in the list of test "fixturenames".
    option_value = metafunc.config.option.child_stdout_level
    if 'child_stdout_level' in metafunc.fixturenames and option_value is not None:
        # Take users `option_value` and make it an `LogLevel` type again
        metafunc.parametrize("child_stdout_level", [LogLevel[option_value.upper()]])

