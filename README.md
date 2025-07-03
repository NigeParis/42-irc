# IRC Project

We teamed up to create our own IRC (Internet Relay Chat) server. The goal was to build a chat server that supports multiple users, real-time messaging, and basic features like joining channels, setting nicknames, and sending private messages—just like classic online chat systems.

We used low-level socket programming to handle communication between clients and the server, and learned how to manage multiple users at the same time using threads or non-blocking I/O. Along the way, we also built in command parsing, user handling, and channel management.

## Bonus Features

### A Quote Bot
Instead of a general-purpose bot, we created a quote bot that randomly shares preloaded quotes when prompted. It added some personality to the chat and gave us a chance to practice handling bot-triggered events and responses without blocking other server activity.

### File Transfers
We implemented file transfers between users using direct connections. It required handling separate sockets and making sure the transfer process didn’t interrupt the chat. It was a bit of a challenge, but definitely worth it.

---

This project helped us become more confident with network programming and collaboration in a real C-based project. We hit a few bumps, but we learned a lot and walked away with something we’re proud of.


# Basic Irssi Commands

## Connection and Setup

- `/connect <server>`  
  Connect to an our IRC server (e.g., `/connect localhost <port> <password>`)

- `/disconnect`  
  Disconnect from the current server

- `/nick <nickname>`  
  Change your nickname

- `/quit [message]`  
  Leave Irssi with an optional quit message

## Channel Management

- `/join #channel`  
  Join a specific channel (e.g., `/join #myproject`)

- `/part [#channel] [message]`  
  Leave a channel with an optional parting message

- `/topic <new topic>`  
  Set or change the topic of the current channel

- `/names`  
  List users currently in the channel

## Messaging

- `/msg <nick> <message>`  
  Send a private message to a specific user

- `/me <action>`  
  Send an action (e.g., `/me laughs` → *yourname laughs*)

- `/notice <nick> <message>`  
  Send a notice (less intrusive than a regular message)

## Navigation

- `Alt + <number>`  
  Switch to a specific Irssi window (Alt+2 = Window 2)

- `Ctrl + N` / `Ctrl + P`  
  Move to the next or previous window

- `/window list`  
  Show all open windows and their activity

- `/window close` or `/wc`  
  Close the current window

