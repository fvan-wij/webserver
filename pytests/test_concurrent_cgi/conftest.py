import pytest
from os import chdir
from pathlib import Path

from ..conftest import LogLevel, WebservConfig, search_upwards_for_file


@pytest.fixture
def webserv_config(child_stdout_level: LogLevel) -> WebservConfig:
    executable_name = 'app'
    args=["test.conf"]
    # This should match the ports specified in the config file.
    ports=[8080, 8081, 9090, 9091]
    url = "http://localhost:"

    script_path = Path(__file__).parent.resolve()
    chdir(script_path)
    executable_path = search_upwards_for_file(executable_name)
    

    config = WebservConfig(
        path=str(executable_path),
        args=args,
        ports=ports,
        url=url,
        stdout_level=child_stdout_level,
        )
    return config
