# MQTT vs WebSockets

At first glance, MQTT might look a bit like WebSockets. In both cases, once the connection is established, you can send anything you want between client and server. Traffic is full duplex, meaning that both client and server are listening and speaking at the same time. However, there are some differences. Overall, MQTT is a lighter weight protocol, and is message-based as opposed to session-based.

## WebSockets are Session-Based
The connection process between a WebSocket client and server always begins with a HTTP request from the client that looks like this:

````
HTTP/1.1
GET /websocket
Host:
Upgrade: websocket
Connection: Upgrade
Sec-WebSocket-Key: 
````

The server responds like this:

````
HTTP/1.1 101 Switching Protocols
Upgrade: websocket
Connection: Upgrade
Sec-WebSocket-Accept:
````

Once the initial exchange is done, the client and server establish a socket conection. In OSI terms, this is a [session](https://en.wikipedia.org/wiki/Session_layer), meaning both sides formally opem communications, then maintain information on the state of the connection. The connection is synchronous: once the request is sent, the client has to wait for the server to respond. The server never initiates the connection, it always starts with the client. 

WebSockets were designed as an extension of HTTP, so the session is maintained until one side or the other explicitly closes it. WebSockets work great if you have a client that's native to HTTP (for example, a browser), but if you're making a client on a microcontroller or other memory-limited device, the extra complexity of establishing an HTTP connection to get the WebSocket can be a burden.

## MQTT is Message-Based

MQTT, unlike WebSockets, is message- and topic-based. Client and broker don't establish a session. Instead, they continually exchanges messages. The pattern is called **publish and subscribe**. MQTT host programs are called brokers instead of servers, since they broker traffic between clients. It works like this:

First, the client sends a connection message, and the broker acknowledges it.  

Next, the client either sends a message to a particular topic, or subcribes to a topic. In either case, the broker sends an acknowledgement. 

Messages can be sent at any time. Client or broker do not have to wait for the other besore sending. 

The broker saves the message as the topic's current value, and if other clients subscribe to the same topic, it sends them the value. If another client publishes to the topic, the value is updated, and all clients who've subscribed to the topic are updated with the latest value. 

Client and broker don't maintain a session, they just exchange messages. A broker doesn't save the history of a topic or who sent the latest message, it just maintains the latest message and when it was sent. It's up to the clients to save the history if they need it.  

## One-to-One vs. Many-to-Many

The synchronous, one-way nature of WebSockets makes them great for an ongoing one-to-one relationship between client and server, but it's a problem if you want multiple clients all communicating at the same time.  Similarly, the fact that every exchange starts with an exchange of complex headers makes HTTP and WebSockets a heavier method of exchange than MQTT. MQTT is really designed for a network of sensors that come online from time to time, send an update, and then go back offline. It makes it easy for sensor devices to be ephemeral. WebSockets aren't designed for this kind of on-and-off pattern. They're great if you have a client that needs a dedicated connection to a server for a set period of time, for example a game controller, but it comes at the expense of a stricter, more formal communications protocol.