# Buffer Overflow Demostration

A demonstration of a vulnerable web server that can be attacked using a buffer overflow.

## Simple server crash (strcpy overflow)

The demostration idea is to be minimal, the server copies a user-supplied value into a fixed 32-byte stack buffer using `strcpy`. Sending a long `X-User` header will overflow the buffer and crash the process (often with stack protector abort).

Build:

```sh
make -C server/src clean && make -C server/src Server.out
make -C client/src clean && make -C client/src client.out
```

Run the server (in one terminal):

```sh
cd server/src
./Server.out
```

Crash it from the client (in another terminal):

```sh
cd client/src
# Defaults to 127.0.0.1 and a 1000-byte payload
./client.out --crash

# You can also specify IP and payload length:
./client.out --crash --len 2048 127.0.0.1
```

If the server crashed, the listening socket on port 8080 will disappear and the client will typically receive no response.
