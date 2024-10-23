import pytest
import requests
import subprocess

def count_process_fds(result):
    socket_count    = 0;
    pipe_count      = 0;
    for line in result.stdout.splitlines():
        if 'socket' in line:
            socket_count += 1
        elif 'pipe' in line:
            pipe_count += 1
    result = (socket_count, pipe_count)
    return result


def test_fd_leaks():
    result = subprocess.run(['bash', 'list_all_open_fds.sh'], capture_output=True, text=True)
    fd_count = count_process_fds(result) 
    print(f"Socket fds: {fd_count[0]}, Pipe fds: {fd_count[1]}")


    
