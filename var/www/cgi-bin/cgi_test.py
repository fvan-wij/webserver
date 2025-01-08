import cgi

print("bla bla")

args = cgi.parse()
print(args)

arguments = cgi.FieldStorage()
for i in arguments.keys():
 print(arguments[i])

