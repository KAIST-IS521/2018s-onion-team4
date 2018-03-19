2018s-onion-team4
=====

Features(& Additional)
-----
- Send msg
<pre><code>/send [Github_ID] [Msg]</code></pre>
- Geturl to asciiart
<pre><code>/sendimg [Github_ID] [url]</code></pre>

Build & Run
-----

Architecture
-----
- MainTread > Server
- Enc & Dec Thread

Protocol
-----
MSG[1byte]
- 0: Handshake <pre><code>[GithubID Length(4byte), GithubID, PubkeyLength, Pubkey, # of connected node & Node IPS]</pre></code>
- 1: MSG <pre><code>[TimeStamp(4byte), Length(uint32_t), Data(Length)]</pre></code>

Team Member & Role
-----
- Node & Protocol : Minkyu Jung, Heeseok Kim
- (Enc & Dec) & Additional feature : Hyunki Kim, Hongsik Kim
