import socket

# Replace with your server's host and port
host = "localhost"
port = 9090

# Open the image file
# image_path = "big_image.png"
image_path = "YoinkTest.png"
with open(image_path, "rb") as f:
    image_data = f.read()

# Chunk size
chunk_size = 1024  # Send in 1KB chunks

# Connect to the server
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((host, port))

    # Send the HTTP headers
    headers = (
        "POST /uploads HTTP/1.1\r\n"
        f"Host: {host}\r\n"
        "Transfer-Encoding: chunked\r\n"
        # "Content-Type: image/png\r\n"
        "Content-Type: audio/mpeg\r\n"
        "\r\n"
    )
    s.sendall(headers.encode())

    # Send the image data in chunks
    for i in range(0, len(image_data), chunk_size):
        chunk = image_data[i:i + chunk_size]
        s.sendall(f"{len(chunk):x}\r\n".encode())  # Send chunk size in hex
        s.sendall(chunk)  # Send the chunk data
        s.sendall(b"\r\n")  # End of chunk

    # Send the final chunk
    s.sendall(b"0\r\n\r\n")  # Indicates the end of the data

    # Receive and print the server's response
    response = s.recv(4096)
    print(response.decode())
