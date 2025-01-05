from ..conftest import LogLevel, WebservInstance
import requests
import psutil

def send_chunked_post_request(port: int, uri: str, chunks: list, expected_status_code: int):
	"""
	Sends a chunked POST request to the specified URI and verifies the response status code.

	Args:
		port (int): The port number to send the request to.
		uri (str): The URI to send the request to.
		chunks (list): The list of chunks to send in the request body.
		expected_status_code (int): The expected HTTP status code of the response.
	"""
	# print(f"\nSending chunked POST request to http://localhost:{str(port)}/{uri}")

	def generate_chunks():
		for chunk in chunks:
			yield chunk

	headers = {'Transfer-Encoding': 'chunked'}
	r = requests.post(f"http://localhost:{str(port)}/{uri}", data=generate_chunks(), headers=headers)

	# print("Status code == ", r.status_code)
	# print("Expected code == ", expected_status_code)
	assert r.status_code == expected_status_code

def test_chunked_request(webserv_instance: WebservInstance) -> None:
	"""
	Tests the chunked request functionality of the web server instance.

	Args:
		webserv_instance (WebservInstance): The instance of the web server to test.
	"""
	assert webserv_instance.proc.pid != 0

	chunks = ["chunk1", "chunk2j", "chunk3jk"]

	for port in webserv_instance.config.ports:
		send_chunked_post_request(port, "/upload", chunks, 200)
