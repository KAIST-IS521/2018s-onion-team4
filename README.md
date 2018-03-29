 2018s-onion-team4
=====

Onion Messenger
-----
Onion messenger is the messenger service that is inspired by onion routing.

It uses PGP to encrypt or decrypt messages.

Also, our messenger provides ascii art feature! You can send your favorite picture to others.

Build & Run
-----
```sh
$ make
$ ./OnionMessenger --help  # to show options
Usage: ./OnionMessenger [Options]

Required arguments
  --priv, -s : Private key file
  --pub,  -b : Public key file
  --port, -p : Server listen port

Optional arguments
  --notui    : Not use TUI
  --node, -n : Another client's IP
  --help, -h : Show this message

```
Command line
-----
- Send msg
`/send [Github_ID] [msg]`
- Geturl to asciiart
`/image [Github_ID] [url]`

TUI Overview
-----
todo

CUI Overview
-----
todo

Architecture
-----
- We use two threads + on-demand thread.
  * Thread #1: UI handler.
  * Thread #2: Packet handler.
  * On demand thread for PGP encrypt and decrypt.
- Distributed System: No `single point of failure`.
- IO multiplexing with `select`: Can handle lots of clients.
- Onion routing with PGP: All of your messages cannot be sniffed. Also, no one knows who is the sender and who is the receiver except you and receiver.

Protocol
-----
There are two types of packet.

##### Handshake
`| 0(uint8_t) | port(uint16_t) | GithubID Length(uint32_t) | GithubID(GithubIDLength) |
PubkeyLength(uint32_t) | Pubkey(PubkeyLength)| # of connected node(uint32_t)
| (# of connected node) entries of Node IP(uint32_t) |`

##### Msg
`| 1(uint8_t) | Length(uint32_t) | Cipher text(Length) |`

Message type
-----
Three types of message is embeded to msg packet.

OnionLayer: `| 0 (uint8_t) | idLength(uint32_t) | id (idLength) | Data |`

MessageLayer: `| 1 (uint8_t) | Data |`

ImageLayer: `| 2 (uint8_t) | Url |`

Team Member & Role
-----
| Name        | Role                    |
|-------------|-------------------------|
| Hyunki Kim  | PGP & Features          |
| Hongsik Kim | PGP & Features          |
| Heeseok Kim | Protocol                |
| Minkyu Jung | Client & Protocol && UI |
