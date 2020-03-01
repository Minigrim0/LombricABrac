# SERVER

## _Dependencies_
The server part of the project uses X libraries :
* Google's Protocol Buffers
* Sqlite3
* Bcrypt
* ZeroMQ

**Google's Protocol Buffers** : this library is used with the client part, to easily exchange data through the network.
It uses .proto files, that contains the definitions of the structures you want to send through the network.
The makefile then compiles this .proto file into a cpp source file, that is then compiled as an object file, and then 
mixed with the rest of the source code, as an executable. The compilation into a cpp file provides definitions and declarations of classes
that have a method allowing us to serialize the state of the instantiated object into a string or some other types of data (We only use
strings in this project). The plus is that on the other side (for example the client) we are able, from the string received, to parse
this last one in a newly created object of the same class, to recover data.

**Sqlite3** : This library is simply used to store and retreive long lasting data easily (The users profiles, the messages sent/received, ...)

**Bcrypt** : This library is used to hash users' passwords, to avoid (in case of a database leak) the gathering of the user's passwords.

**ZeroMQ** : This lib is used to communicate between the different threads on the server. It is used as follow :
* The lib allows us to create "channels" on the server, to send or receive messages, via the publisher/subscriber scheme
* There is a thread on the server called Broker_Thread. that is subscribed to the channel "all". It receives all the message
a user could send to communicate with an other thread (sending a message to another user, add a friend, ...), and all the messages
that the user could send to interact with the server itself (create a room mainly). If the message is destined to someone else, the
thread simply redirects it to the corresponding channel, otherwise, it readsit and interpret the message.
* Every client is, as soon as he's authenticated, susbcribed to his own broker related channel ("users/<client_id>/broker"), and his own
room related channel ("users/<client_id>/room"). He can then receive messages from the broker (e.g. : if another client wants to send
him a message), and from the room if he's in one (e.g. : when the game is starting).
* The room threads are also connected to their own channels, where the clients can publish to interract with them.


## _Building the server_

#### **1) Initialize data base**

The first step is to initialize the database, to create the tables (and the file if not existing).
```
    make init_db
    ./init_db
```

#### **2) Building the server**

To build the server, you just need to type :
```
    make
```

Three out of the four libraries are already included in the project, but the fourth (ZeroMQ) will be downloaded
and installed on your computer (/!\ **IT MAY ASK FOR YOUR PASSWORD IN ORDER TO INSTALL THE LIB** /!\).


#### **3) Running the server**
To run the server no simply need to execute the following :
```
    ./server <port>
```
Providing the port you want to use as the only parameter.