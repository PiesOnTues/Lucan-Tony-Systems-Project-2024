# Prints 3, 6, 12, 12, 6, 0, 20
var <- 3
vartwo <- var
vartwo <- vartwo *2 
varthree <- vartwo*2
print var
print  vartwo
print varthree

function mult a b
	return a * b

function pmult a b
	print a * b

pmult (mult (1, mult(1,mult(1,1))), varthree)
print pmult(2,3) + a
pmult(4, 5)