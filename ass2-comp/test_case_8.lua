
function test(a,b,c)
	print(a,b, c)
end


function add(a,b,c,d)
	return a+b+c+d
end

test(1,2,3)

test(8 ,5,add(1,2,3,4))
