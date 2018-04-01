 2018s-onion-team4
=====

Onion Messenger
-----
Onion messenger is the messenger service that is inspired by onion routing.

It uses PGP to encrypt or decrypt messages.

Also, our messenger provides ascii art feature! You can send your favorite picture to others.

Build & Run
-----
You can run our messenger in your own environment.

```sh
$ sudo apt install libncurses-dev libcurl4-openssl-dev g++ \
  golang-go apt-utils apt-utils
$ go get github.com/stdupp/goasciiart
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

or use [docker](Docker/)!

Command line
-----
- Send msg
`/msg [Github_ID] [msg]` or `/m [Github_ID] [msg]`
- Geturl to asciiart
`/img [Github_ID] [url]` or `/i [Github_ID] [url]`
- Help message
`/help` or `/h`

UI Overview
-----
- Login UI
<img src="https://user-images.githubusercontent.com/12270593/38128144-56c27c36-3434-11e8-9e75-25cb297a0212.PNG" height="500">

- Chat UI
<img src="https://user-images.githubusercontent.com/12270593/38128267-d8055a5c-3434-11e8-8ca7-8f1aa7cc795a.PNG" height="300">

- Chat UI with Asciiart
<img src="https://user-images.githubusercontent.com/12270593/38128207-95919afa-3434-11e8-8665-3b9811aa1531.PNG" height="500">

Architecture
-----
- We use two threads + on-demand thread.
  * Thread #1: UI handler.
  * Thread #2: Packet handler.
  * On demand thread for PGP encrypt and decrypt.
- Distributed System: No `single point of failure`.
- IO multiplexing with `select`: Can handle lots of clients.
- Onion routing with PGP: All of your messages cannot be sniffed.
Also, no one knows who is the sender and who is the receiver except you and receiver.

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
| Name        | Role                     |
|-------------|--------------------------|
| Hyunki Kim  | Features & docs & Docker |
| Hongsik Kim | PGP & Features           |
| Heeseok Kim | CUI                      |
| Minkyu Jung | All components           |
