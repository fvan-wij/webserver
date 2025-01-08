import cgi

print("Listing the parameters given in the URL")
print("")
print("")
print("")

arguments = cgi.FieldStorage()
for i in arguments.keys():
    print(f"{i} : {arguments[i].value}")

