# BasicallyEpollingHTTPServer

Currently, it's just(despite the name) an TCP-Echo server, because i didn't yet come to implement the HTTP functionality
on top of it.

To start it, just do a quick

```shell
mkdir build && cd build
cmake .. && make
./BaiscallyEpollingHTTP
```

And to reach it, open a(or multiple) terminal windows with netcat!

```shell
netcat 127.0.0.1 8080
```

... and interact with it! :)