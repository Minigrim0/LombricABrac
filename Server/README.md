# SERVER

## _Make commands_

#### **Simple compilation**
Type the following :
```
    make
    ./server <port>
```

#### **Compile + run**
Type the following :
```
    make run
```

#### **Compile + run with valgrind**
Type the following :
```
    make valrun
```

#### **Initialize data base**

Without filling it :
```
    make init_db
    ./init_db
```

With some random useless data in it :
```
    make init_db
    ./init_db true
```

#### **Clean**
```
    make clean
```

#### **Fresh start**
```
    make clean all
```


## Database methods
### ```int send_message(int sender_id, int receiver_id, std::string message);```
Sends a message to the user with the receiver_id, from the user with sender_id

### ```int get_received_messages(int user_id, Chat_r* chat_r);```
Returns the list of the messages received by the user with user_id

### ```int get_sent_messages(int user_id, Chat_r* chat_r);```
Returns the list of the messages sent by the user with user_id


### ```int get_all_messages(int user_id, Chat_r* chat_r);```
Returns the list of all the sent or received messages of the user with user_id

### TODO
* method to gather the messages sent to a specific user
* method to gather the messages received from a specific user
* method to gather the messages exchanged with a specific user (received or sent)