#!/usr/bin/env python3

import os
import sys
import mimetypes

def generate_http_response(file_path):
    if not os.path.isfile(file_path) or not os.access(file_path, os.R_OK):
        print("Status: 404 Not Found")
        print("Content-Type: text/plain")
        print()
        print("Error: File not found or cannot be accessed.")
        return

    mime_type, _ = mimetypes.guess_type(file_path)
    if mime_type is None:
        mime_type = "application/octet-stream"

    file_size = os.path.getsize(file_path)

    print("Status: 200 OK")
    print(f"Content-Type: {mime_type}")
    print(f"Content-Length: {file_size}")
    print("Content-Disposition: attachment; filename=\"{}\"".format(os.path.basename(file_path)))
    print()

    with open(file_path, "rb") as f:
        sys.stdout.buffer.write(f.read())

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Status: 400 Bad Request")
        print("Content-Type: text/plain")
        print()
        print("Error: Missing file path argument.")
        sys.exit(1)

    file_path = sys.argv[1]

    generate_http_response(file_path)
