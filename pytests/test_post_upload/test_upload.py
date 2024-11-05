import pytest
from time import sleep
from ..conftest import WebservInstance
import requests
import os.path
from requests_toolbelt.multipart.encoder import MultipartEncoder # Download this

def send_webkitformboundary_post_request(port: int, file: str, format: str, expected_status_code: int):
    url = f"http://localhost:{port}/uploads"
    dest_file_path = "./var/www/uploads/" + file
    m = MultipartEncoder(
        fields = {
            'file':(file, open(file, 'rb'), format),
        }
    )
    headers = {'Content-Type': m.content_type}
    r = requests.post(url, data=m, headers=headers)
    print("Status code == ",  r.status_code)
    print("Expected code == ",  expected_status_code)
    assert(r.status_code) == expected_status_code or 413
    if (r.status_code == expected_status_code):
        assert(os.path.isfile(dest_file_path)) == True
        size_test_file = os.path.getsize(dest_file_path)
        size_og_file = os.path.getsize(file)
        print(f"Size of test file: {size_test_file}, size of og file {size_og_file}")
        assert(size_test_file) == size_og_file
        os.remove(dest_file_path)


def send_basic_upload_request(port: int, file_path: str, expected_status_code: int):
    headers = {
        "Content-Type": "multipart/form-data; boundary=----WebKitFormBoundaryq0TjWbmsRaFX6PG9"
        }  
    body = (
            "------WebKitFormBoundaryq0TjWbmsRaFX6PG9\r\n"
            f"Content-Disposition: form-data; name=\"file\"; filename=\"{file_path}\"\r\n"
            "Content-Type: text/plain\r\n"
            "\r\n"
            "hoi\r\n"
            "------WebKitFormBoundaryq0TjWbmsRaFX6PG9--\r\n"
            )
    url = "http://localhost:"
    url += str(port)
    url += "/uploads"
    files={"upload_file": open(file_path, "rb")}
    r = requests.post(url, headers=headers, data=body)
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
        os.remove(test_file_path)

def send_upload_request(port: int, file_path: str, expected_status_code: int):
    url = "http://localhost:"
    url += str(port)
    url += "/uploads"
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
        os.remove(test_file_path)

def test_webserv_instance_pid_check(webserv_instance: WebservInstance) -> None:
    assert webserv_instance.proc.pid is not 0
    # send_webkitformboundary_post_request(webserv_instance.config.ports[0], 'test_image1.png', 'image/png', 200)
    # send_basic_upload_request(webserv_instance.config.ports[0], "test_file.txt", 200)
    # send_upload_request(webserv_instance.config.ports[0], "test_file.txt", 200)
    send_upload_request(webserv_instance.config.ports[0], "test_image1.png", 200)
    # send_upload_request(webserv_instance.config.ports[0], "test_audio1.mp3", 200)
    # send_upload_request(webserv_instance.config.ports[0], "test_audio2.mp3", 200)

