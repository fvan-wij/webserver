import pytest
from ..conftest import WebservInstance
import requests
from concurrent.futures import ThreadPoolExecutor

def trigger_cgi_concurrently_on_all_servers(urls):
    def send_concurrent_post_request(url):
        try:
            r = requests.post(url)
            return f"POST request {url}: Status Code {r.status_code}"
        except requests.exceptions.RequestException as e:
            return f"POST request {url}: Failed with error {e}"

    with ThreadPoolExecutor(max_workers=len(urls)) as executor:
        results = executor.map(send_concurrent_post_request, urls)

    for result in results:
        print(result)

def test_webserv_instance_pid_check(webserv_instance: WebservInstance) -> None:
    assert webserv_instance.proc.pid is not 0
    urls = []
    for i in range(4):
        urls.append(webserv_instance.config.url + str(webserv_instance.config.ports[0]) + "/cgi-bin")
    trigger_cgi_concurrently_on_all_servers(urls)
