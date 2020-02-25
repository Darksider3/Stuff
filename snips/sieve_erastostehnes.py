import math

N = 100000000
gestrichen = [False] * N
i = 2
print(math.sqrt(N));
while i <= math.sqrt(N):
	if not gestrichen[i]:
		print(str(i)+", ", end='')
		j = 0
		while j < math.sqrt(N):
			gestrichen[i*j] = True
			j+=1;
	i+=1;

print()
