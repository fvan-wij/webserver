import pytest
from time import sleep
from ..conftest import WebservInstance
import requests
import os.path

def send_upload_request(port: int, file_path: str, expected_status_code: int):
    url = "http://localhost:" + str(port) + "/uploads"
    files={"upload_file": open(file_path, "rb")}
    r = requests.post(url, files=files)
    print("Status code == ",  r.status_code)
    print("Expected code == ",  expected_status_code)
    assert(r.status_code) == expected_status_code or 413
    test_file_path = "./var/www/uploads/" + file_path
    if (r.status_code == expected_status_code):
        assert(os.path.isfile(test_file_path)) == True
        size_test_file = os.path.getsize(test_file_path)
        size_og_file = os.path.getsize(file_path)
        print(f"Size of test file: {size_test_file}, size of og file {size_og_file}")
        assert(size_test_file) == size_og_file

def send_delete_request(port: int, file_name: str, expected_status_code: int):
    url = "http://localhost:" + str(port) + "/uploads/" + file_name
    r = requests.delete(url)
    print("Status code == ",  r.status_code)
    print("Expected code == ",  expected_status_code)
    assert(r.status_code) == expected_status_code or 413
    test_file_path = "./var/www/uploads/" + file_name
    if (r.status_code == expected_status_code):
        assert(os.path.isfile(test_file_path)) == False

def send_get_request(port: int, uri: str, expected_status_code: int):
    print(f"\nSending GET request to http://localhost:{str(port)}/{uri}")
    r = requests.get(f"http://localhost:{str(port)}/{uri}")
    print("Status code == ",  r.status_code)
    print("Expected code == ",  expected_status_code)
    assert(r.status_code) == expected_status_code

def trigger_cgi(port: int, file_name: str, expected_status_code: int):
    url = "http://localhost:" + str(port) + "/cgi-bin/" + file_name
    r = requests.post(url)
    print("Status code == ",  r.status_code)
    print("Expected code == ",  expected_status_code)
    assert(r.status_code) == expected_status_code

def test_webserv_get_upload_delete(webserv_instance: WebservInstance) -> None:
    assert webserv_instance.proc.pid is not 0
    server_port = webserv_instance.config.ports[0]
    # get requests
    send_get_request(server_port, "", 200)
    send_get_request(server_port, "/uploads", 200)
    send_get_request(server_port, "/cgi-bin", 405)

    # uploading files
    send_upload_request(server_port, "test_image1.png", 200)
    send_upload_request(server_port, "test_audio1.mp3", 200)
    send_upload_request(server_port, "test_audio2.mp3", 200)

    # deleting files
    send_delete_request(server_port, "test_image1.png", 200)
    send_delete_request(server_port, "test_audio1.mp3", 200)
    send_delete_request(server_port, "test_audio2.mp3", 200)

    # trigger cgi
    trigger_cgi(server_port, "hello_world.py", 200)

