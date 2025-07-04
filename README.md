## IRC Project

We teamed up to create our own IRC (Internet Relay Chat) server. The goal was to build a chat server that supports multiple users, real-time messaging, and basic features like joining channels, setting nicknames, and sending private messages—just like classic online chat systems.

We used low-level socket programming to handle communication between clients and the server, and learned how to manage multiple users at the same time using threads or non-blocking I/O. Along the way, we also built in command parsing, user handling, and channel management.

### Bonus Features

### A Quote Bot
Instead of a general-purpose bot, we created a quote bot that randomly shares preloaded quotes when prompted. It added some personality to the chat and gave us a chance to practice handling bot-triggered events and responses without blocking other server activity.

### File Transfers
We implemented file transfers between users using direct connections. It required handling separate sockets and making sure the transfer process didn’t interrupt the chat. It was a bit of a challenge, but definitely worth it.

---

This project helped us become more confident with network programming and collaboration in a real C-based project. We hit a few bumps, but we learned a lot and walked away with something we’re proud of.



### What Is the `irssi` Command?

We used the 'irssi' command a powerful and lightweight terminal-based IRC (Internet Relay Chat) client. It runs entirely in the terminal and is often favored for its speed, efficiency, and scriptability.

### What It Does

When you enter `irssi` in the terminal:

- It starts the Irssi interface inside your terminal window.
- You can begin entering IRC commands like `/connect` or `/join`.
- It allows real-time chatting in channels or via private messages.

### Basic Usage connecting using irssi client command 

```perl
irssi

/connect localhost <6100> <password>
```

### Connecting to Our IRC Server Using `nc`

During development and testing, we used the `nc` (Netcat) command-line tool to connect to our IRC server from the terminal. It provided a quick and lightweight way to manually interact with the server and send IRC commands without the need for a full client.

### Example connecting using nc Command

```perl
// -C optiion = Send CRLF('\r\n') as line-ending.

nc -C localhost 6100
PASS <password>
NICK <nickname>
USER <nige> <nigehost> <localhost> :<Niel Robertson>
```
### Breakdown of the `USER` Command Components

```perl
| Part               | Description                                                                 |
|--------------------|-----------------------------------------------------------------------------|
| `USER`             | IRC command used to register the user after connecting to the server        |
| `nige`             | Username (also called ident)                                                 |
| `nigehost`         | Hostname, which may be a placeholder or ignored by some IRC servers          |
| `localhost`        | Server name—often unused or set to `*` in practice                          |
| `:Niel Robertson`  | Real name (GECOS field): the colon is required to allow spaces in the text  |

```


### Difference in Commands Between `irssi` and `nc`

Both `irssi` and `nc` (Netcat) can be used to interact with IRC servers, but they operate at very different levels of abstraction. Here's a comparison of how their commands differ:

### 1. Interface Style

```perl

| Tool    | Command Style            | Description                                               |
|---------|--------------------------|-----------------------------------------------------------|
| `irssi` | Slash-prefixed commands  | Commands start with `/` (e.g., `/join`, `/msg`)           |
| `nc`    | Raw IRC protocol lines   | You type raw IRC commands manually (e.g., `NICK`, `USER`) |

---
```
### 2. Connecting to a Server

```perl
| Task              | `irssi` Command                          | `nc` Command Example                      |
|-------------------|------------------------------------------|-------------------------------------------|
| Connect to server | `/connect irc.example.com`               | `nc -C irc.example.com 6667`              |
| Set nickname      | `/nick myname`                           | `NICK myname` (typed manually)            |
| Register user     | `/user myname myhost myserver :Real Name`| `USER myname myhost myserver :Real Name`  |

---
```

### 3. Joining and Messaging

```perl
| Task                                  | `irssi` Command                  | `nc` Raw Command                |
|---------------------------------------|----------------------------------|---------------------------------|
| Join a channel                        | `/join #channel`                 | `JOIN #channel`                 |
| Send a message                        | `/msg nick Hello`                | `PRIVMSG nick :Hello`           |
| Leave a channel                       | `/part #channel`                 | `PART #channel`                 |
| Quit IRC                              | `/quit`                          | `QUIT :Goodbye`                 |
| Invite a user to a channel            | `/invite alice #devchat`         | `INVITE alice #devchat`         |
| View current topic of a channel       | `/topic #project`                | `TOPIC #project`                |
| Set a new topic (if operator)         | `/topic #project :Meeting at 5pm`| `TOPIC #project :Meeting at 5pm`|
| Set channel to invite-only (`+i`)     | `/mode #team +i`                 | `MODE #team +i`                 |
| Remove invite-only mode (`-i`)        | `/mode #team -i`                 | `MODE #team -i`                 |
| Restrict topic changes to operators   | `/mode #team +t`                 | `MODE #team +t`                 |
| Allow anyone to change topic (`-t`)   | `/mode #team -t`                 | `MODE #team -t`                 |
| Set a channel password (`+k`)         | `/mode #vault +k secret123`      | `MODE #vault +k secret123`      |
| Remove channel password (`-k`)        | `/mode #vault -k`                | `MODE #vault -k`                |
| Give operator status to a user (`+o`) | `/mode #chatroom +o bob`         | `MODE #chatroom +o bob`         |
| Remove operator status (`-o`)         | `/mode #chatroom -o bob`         | `MODE #chatroom -o bob`         |
| Set user limit (`+l`)                 | `/mode #team +l 15`              | `MODE #team +l 15`              |
| Remove user limit (`-l`)              | `/mode #team -l`                 | `MODE #team -l`                 |


---
```
### 4. Navigation and Extras

```perl
| Feature            | `irssi`                  | `nc`                              |
|--------------------|--------------------------|-----------------------------------|
| Switch windows     | `Alt + number`           | Not applicable                    |
| View user list     | `/names`                 | `NAMES #channel`                  |
| Help               | `/help`                  | Not available                     |

---
```

### Summary

- **`irssi`** is a full-featured IRC client with built-in command parsing, window management, and scripting support.
- **`nc`** is a low-level tool that sends raw text over TCP. You must manually type every IRC protocol command.

Use `irssi` for convenience and productivity. Use `nc` when you want to debug or test your server at the protocol level.


### Quote Bot  (bonus) — Command Examples

### → "Displays a random joke from the bot’s collection.
```perl
/quote bot
```
### → Adds a new joke to the collection.
```perl
/quote bot set Pourquoi les canards n’aiment-ils pas les grenouilles ? Parce qu’elles les cuassent !
```
### → Removes the joke at index 1 (indexes start at 0).
```perl
/quote bot remove :2
```


## File Transfer Between irssi Clients — /dcc Command (bonus)

As part of the bonus feature, we implemented file transfers using the `/dcc` command. This allows clients to send and receive files directly, bypassing the IRC server.
### Sending a File
```perl

/dcc send <nickname> <filename>

/dcc send alice project_notes.txt

```

### receiving a file

```perl
/dcc get <nickname> <filename>

/dcc get bob project_notes.txt

```



### connecting on my server from another computer

```perl
//find the ip address of the server in a terminal perl
hostname -I
10.16.4.9  192.168.122.1  172.17.0.1 
```

```perl
irssi
/connect 10.16.4.9 <port nbr> <password>
```

#### link IRC Docs - https://modern.ircdocs.horse/

#### project made by mkane and nrobinso : 42.paris
