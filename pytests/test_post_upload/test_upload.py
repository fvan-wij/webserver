import pytest
from time import sleep
from ..conftest import WebservInstance
import requests
import os.path

def send_upload_request(port: int, file_path: str, expected_status_code: int):
    url = "http://localhost:"
    url += str(port)
    url += "/uploads"
    absolute_path = "./pytests/test_post_upload/" + file_path
    files={"upload_file": open(absolute_path, "rb")}
    r = requests.post(url, files=files)
    print("Status code == ",  r.status_code)
    print("Expected code == ",  expected_status_code)
    assert(r.status_code) == expected_status_code or 413
    test_file_path = "./var/www/uploads/" + file_path
    if (r.status_code == expected_status_code):
        assert(os.path.isfile(test_file_path)) == True
        size_test_file = os.path.getsize(test_file_path)
        size_og_file = os.path.getsize(absolute_path)
        print(f"Size of test file: {size_test_file}, size of og file {size_og_file}")
        assert(size_test_file) == size_og_file
        os.remove(test_file_path)

def test_webserv_instance_pid_check(webserv_instance: WebservInstance) -> None:
    assert webserv_instance.proc.pid is not 0
    send_upload_request(webserv_instance.config.ports[0], "test_image1.png", 200)
    send_upload_request(webserv_instance.config.ports[0], "test_audio1.mp3", 200)
    send_upload_request(webserv_instance.config.ports[0], "test_audio2.mp3", 200)

