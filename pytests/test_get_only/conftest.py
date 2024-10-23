import pytest
from dataclasses import dataclass
from subprocess import Popen, call
from os import path

from ..conftest import WebservConfig

@pytest.fixture
def webserv_config() -> WebservConfig:
    executable_name = "app"
    base_path = path.dirname(path.realpath('__file__'))

    # Set the this files location as the cwd.
    # Run the webserver from here using relative path.
    # Add the current test.conf to args.


    config = WebservConfig(
        path=base_path + "/app",
        args=["test.conf"], 
        # This should match the ports specified in the config file.
        ports=[9090],
        )
    return config
