import pytest
import requests
import os.path

ports = ["8080", "8081", "9090", "9091"]

def send_upload_request(port, file_path, expected_status_code):
    url = "http://localhost:"
    url += port
    url += "/uploads"
    files={"upload_file": open(file_path, "rb")}
    r = requests.post(url, files=files)
    print("Status code == ",  r.status_code)
    print("Expected code == ",  expected_status_code)
    assert(r.status_code) == expected_status_code or 413
    test_file_path = "../var/www/uploads/" + file_path
    if (r.status_code == expected_status_code):
        assert(os.path.isfile(test_file_path)) == True

def test_upload_file():
    send_upload_request(ports[0], "test_image1.png", 200)
    send_upload_request(ports[1], "test_image2.png", 200)
    send_upload_request(ports[2], "test_image3.png", 200)
    send_upload_request(ports[3], "test_image4.png", 200)

