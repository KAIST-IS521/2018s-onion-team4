2018s-onion-team4
=====

Features(& Additional)
-----
- Send msg
`/send [Github_ID] [msg]`
- Geturl to asciiart
`/image [Github_ID] [url]`

Build & Run
-----
`
$ git submodule update
$ make
`

Architecture
-----
- Main thread: handle UI.
- Another thread: handle packet in, out.
- "std::async" used for Enc & Dec.
- Distribute System -> No main server.

Protocol
-----
There are three types of packet.

##### Handshake
```| 0(uint8_t) | GithubID Length(uint32_t) | GithubID(GithubIDLength) |
PubkeyLength(uint32_t) | Pubkey(PubkeyLength)| # of connected node(uint32_t)
| (# of connected node) entries of Node IP(uint32_t) |```

##### Msg
```| 1(uint8_t) | Length(uint32_t) | Data(Length) |```

##### Image

Team Member & Role
-----
| Name        | Role                    |
|-------------|-------------------------|
| Hyunki Kim  | PGP & Features          |
| Hongsik Kim | PGP & Features          |
| Heeseok Kim | Client & Protocol       |
| Minkyu Jung | Client & Protocol && UI |
