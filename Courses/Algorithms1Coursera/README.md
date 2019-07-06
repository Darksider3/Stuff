# Algorithms 1 Stuff

https://www.coursera.org/learn/algorithms-part1/

Following problem:

Given is a set of elements, some are connected, some not:
(1) ---> (2)    (3)---->(4)
                 ^
                 |
(6)              |
                (5)

1 is connected to 2, 3 is connected to 5 and 4 to 3. 
Now we want a function that can tell us, if the given two objects
are somehow connected. The function to call here is, to connect, connect(x, y).
To check it's (bool) union(x,y)
