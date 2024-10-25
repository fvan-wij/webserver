import pytest
import requests
from concurrent.futures import ThreadPoolExecutor

urls = ["http://localhost:8080/cgi-bin", "http://localhost:8081/cgi-bin", "http://localhost:9090/cgi-bin", "http://localhost:9091/cgi-bin"]

#Function that concurrently runs all the given urls
def trigger_cgi_on_urls(urls):
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

def test_cgi_calls():
    print("\n[TEST3]: Sending POST requests to ", urls)
    trigger_cgi_on_urls(urls)

