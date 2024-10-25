import pytest
from dataclasses import dataclass
from subprocess import Popen, call
from os import path

from ..conftest import WebservConfig

@pytest.fixture
def webserv_config() -> WebservConfig:
    executable_name = "app"
    base_path = path.dirname(path.realpath('__file__'))


    # TODO
    # Set the this files location as the cwd for webserv.
    # Run the webserver from here using relative path `../../webserv`.
    


    config = WebservConfig(
        path=base_path + "/app",
        args=[], 
        # This should match the ports specified in the config file.
        ports=[9090],
        )
    return config
