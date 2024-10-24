from subprocess import STDOUT
import pytest
from os import chdir
from pathlib import Path

from ..conftest import WebservConfig, search_upwards_for_file


@pytest.fixture
def webserv_config() -> WebservConfig:
    executable_name = 'app'
    args=["test.conf"]
    # This should match the ports specified in the config file.
    ports=[9090]
    url = "http://localhost:"

    script_path = Path(__file__).parent.resolve()
    chdir(script_path)
    executable_path = search_upwards_for_file(executable_name)
    

    config = WebservConfig(
        path=str(executable_path),
        args=args,
        ports=ports,
        url=url,
        )
    return config
