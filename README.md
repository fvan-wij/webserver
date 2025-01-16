# websrv

A HTTP server written in ✨ *C++17* ✨.




## Building

Clone to repo
```
git clone git@github.com:fvan-wij/webserver.git
cd webserver
make
```

## Usage
Running the webserver.
```
./app [CONFIG]
```
Or via the makefile (this will load `test.conf` from the root of the project)
```
make run
```

## Dev scripts
There is a script called `monitor_children.sh` to monitor the child procs, you can run it like this
```
./tests/monitor_children.sh `pidof webserv`
```

Same script for monitoring the fd

```
./tests/monitor_fds.sh `pidof webserv`
```



## Running Tests
There are some end-to-end tests  written in Python using [pytest](https://docs.pytest.org/en/stable/) they are stored in the `pytests` directory. <br>
This directory contains some subdirectories (`test_<NAME>`), essentially acting as seperate test-environments because they all have their own `test.conf` file.



Install the required packages using pip.
```
pip install -r pytests/requirements.txt
```

Pytest will automatically look for the `pytests` [directory](https://docs.pytest.org/en/stable/explanation/goodpractices.html#conventions-for-python-test-discovery).

Running all of the tests.
```
pytest
```

To run a specific test directory/file.
```
pytest [PATH]
```

Or a specific test function.
```
pytest -k NAME 
```

### Pytest flags
| flag | description |
|---------|-----------------------------|
| [PATH] | path to python file containing the tests |
| -s | disable output capture |
| -k NAME | only run tests which match the given substring |



## Dev


### Archiving stale branches
It is recommeded to archive stale branches instead of deleting them

Archive
```
git tag archive/<branchname> <branchname>
git push origin archive/<branchname>
git branch -D <branchname>
```

Restore a branch
```
git checkout -b <branchname> archive/<branchname>
```

> [from](https://stackoverflow.com/a/4292670)
