import cgi
import sys
import time

print("Listing the parameters given in the URL")
print("")
print("")
print("")

arguments = cgi.FieldStorage()
for i in arguments.keys():
    print(f"{i} : {arguments[i].value}")


# time.sleep(2)

i = 0
for arg in sys.argv:
    print(f"{i} : {arg}")
    i += 1

# print(sys.argv[3])

print('')
