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

Architecture
-----
- Main thread: Packet handling
- Other threads: Enc & Dec

Protocol
-----
There are two types of packet.

##### Handshake
`| 0(uint8_t) | GithubID Length(uint32_t) | GithubID(GithubIDLength) |
PubkeyLength(uint32_t) | Pubkey(PubkeyLength)| # of connected node(uint32_t)
| Node IP((uint32_t) * # of connected node) |`

##### Msg
```| 1(uint8_t) | TimeStamp(uint32_t) | Length(uint32_t) | Data(Length) |```

Team Member & Role
-----
| Name        | Role              |
|-------------|-------------------|
| Hyunki Kim  | PGP & Features    |
| Hongsik Kim | PGP & Features    |
| Heeseok Kim | Client & Protocol |
| Minkyu Jung | Client & Protocol |
