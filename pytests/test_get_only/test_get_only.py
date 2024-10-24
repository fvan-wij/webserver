from ..conftest import LogLevel, WebservInstance
import requests
import psutil

def send_get_request(port: int, uri: str, expected_status_code: int):
    url = "http://localhost:"
    url += str(port)
    url += uri
    print("\nSending GET request to url", url)
    r = requests.get(url)
    print("Status code == ",  r.status_code)
    print("Expected code == ",  expected_status_code)
    assert(r.status_code) == expected_status_code


def test_webserv_instance_pid_check(webserv_instance: WebservInstance) -> None:
    assert webserv_instance.proc.pid is not 0


def test_webserv_instance_child_startup(webserv_instance: WebservInstance) -> None:
    proc = psutil.Process(webserv_instance.proc.pid)
    children = proc.children(recursive=True)

    # send_get_request(webserv_instance.config.ports[0], "", 200)
    port: int = webserv_instance.config.ports[0]
    uri: str = "cgi-bin"
    r = requests.post(f"http://localhost:{port}/{uri}")
    print(f"running request on http://localhost:{port}/{uri}")
    assert r.status_code == 200, r.content

    for child in children:
        print('Child pid is {}'.format(child.pid))
    print(f"pid: {webserv_instance.proc.pid}")


def test_webserv_pass(webserv_instance: WebservInstance) -> None:

    names = [entry.name.lower() for entry in LogLevel]
    print(f"names: {names}")

    pass
