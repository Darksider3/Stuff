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

In case the Server get's unresponsive to a CTRL+C, you will have to kill it with `kill`:

```shell
kill $(pgrep -f *Basically*)
```

### Scope

Currently, the only thing i will try with this, is running a functional webserver. **Nothing more**, nothing less. This
means, im not going to handle deflate(for now), HTTP2(or 3, for that matter) and am not going to do anything possible to
support the littlest of the little of edge cases. What is planned to get working

* Simple Requests(GET, PUT... $Verb here)
* Query Parsing
* En&decoding of percent-values
* Uploads