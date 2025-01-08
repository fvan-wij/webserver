import cgi
import time

print("Listing the parameters given in the URL")
print("")
print("")
print("")

arguments = cgi.FieldStorage()
for i in arguments.keys():
    print(f"{i} : {arguments[i].value}")


time.sleep(5)

# with open("../cgi_test.html") as f:
#     i: int = 0;
#     for line in f:
#         print(f"{i} : {line}")
#         i += 1
