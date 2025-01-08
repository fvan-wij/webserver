import requests

def send_chunked_post_request(port: int, uri: str, chunks: list, expected_status_code: int):
	def generate_chunks():
		for chunk in chunks:
			yield chunk
	headers = {'Transfer-Encoding': 'chunked'}
	r = requests.post(f"http://localhost:{str(port)}/{uri}", data=generate_chunks(), headers=headers)
	assert r.status_code == expected_status_code



large_chunk = "A" * 10**6  # 1 MB of data
chunks = [large_chunk for _ in range(10)]  # 10 MB of data in total
send_chunked_post_request(9090, "/uploads", chunks, 200)
