from types import SimpleNamespace
import cgi
import sys

print("Listing the parameters given in the URL")
print("")
print("")
print("")


i = 0
for arg in sys.argv:
    print(f"argv[{i}] : {arg}")
    i += 1

print('')
print('')


if sys.argv[1]:
    print("get parameters")
    arguments = cgi.FieldStorage()
    for i in arguments.keys():
        print(f"{i} : {arguments[i].value}")




if sys.argv[2]:
    print("post body")
    print(f"{sys.argv[2]}")
