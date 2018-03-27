2018s-onion-team4
=====

Onion Messenger
-----
Onion messenger is the messenger service that inspired by onion routing.

It use PGP for encrypt or decrypt messages.

Also, our messenger provides ascii art feature! You can send your favorite picture to other guy.

Build & Run
-----
```sh
$ make
$ ./OnionMessenger --help  # to show options
```

Command line
-----
- Send msg
`/send [Github_ID] [msg]`
- Geturl to asciiart(png file)
`/image [Github_ID] [url]`

Architecture
-----
- Thread #1: handle UI.
- Thread #2: handle packet in, out.
- "std::thread" used for Enc & Dec.
- Distribute System -> No main server.
- Onion routing: Safe from sniffing.

Protocol
-----
There are three types of packet.

##### Handshake
`| 0(uint8_t) | GithubID Length(uint32_t) | GithubID(GithubIDLength) |
PubkeyLength(uint32_t) | Pubkey(PubkeyLength)| # of connected node(uint32_t)
| (# of connected node) entries of Node IP(uint32_t) |`

##### Msg
`| 1(uint8_t) | Length(uint32_t) | ENC of MsgBody(Length) |`

##### MsgBody
OnionLayer: `| 0 (uint8_t) | idLength(uint32_t) | id (idLength) | Data |`

Message   : `| 1 (uint8_t) | Data |`

Image     : `| 2 (uint8_t) | Url |`

Team Member & Role
-----
| Name        | Role                    |
|-------------|-------------------------|
| Hyunki Kim  | PGP & Features          |
| Hongsik Kim | PGP & Features          |
| Heeseok Kim | Protocol                |
| Minkyu Jung | Client & Protocol && UI |
