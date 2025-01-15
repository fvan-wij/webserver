import cgi
import sys

print("Listing the parameters given in the URL")
print("")
print("")
print("")


i = 0
for arg in sys.argv:
    print(f"{i} : {arg}")
    i += 1

print('')
print('')


arguments = cgi.FieldStorage()
for i in arguments.keys():
    print(f"{i} : {arguments[i].value}")



# print(sys.argv[3])

