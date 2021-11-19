### Summary

The current implementation of PMACCT is encoding the six TCP's Control bits into decimal format. It's the responsibility
of an external entity to decode them into the human-readable format  [URG, ACK, PSH, RST, SYN, FIN](Reference https://datatracker.ietf.org/doc/html/rfc793#section-3.1).

The main aim of the new implementation is to embed the decoding action within PMACCT.

The TCP-Flags can be, for example, serialized to KAFKA (via the kafka-plugin) using either JSON or AVRO. The new implementation 
is taking care of loading the active TCP-Flags into an array before proceeding with the actual serialization.

### Main 'Decoding' Steps (Per TCP Flow):

- PMACCT is returning the decimal representation of the TCP's Control bits
- An intermediate step is translating from decimal to binary
- Finally, only the active TCP-Flags are loaded into an array data structure ready to be serialized  
```
4 ---> [0, 0, 0, 1, 0, 0] ---> ['RST']
```

### JSON output
- Current
```JSON
{                                                                                                                                                              │
  "tcp_flags": "decimal_value",                                                                                                                                               │
  "other1": "other1",
  "other2": "other2",
  "other3": "other3"
} 
```

- Future
```JSON
{                                                                                                                                                              │
  "tcp_flags": [                                                                                                                                               │
    "URG",                                                                                                                                                     │
    "ACK",                                                                                                                                                     │
    "PSH",
    "RST",                                                                                                                                                     │
    "SYN",                                                                                                                                                     │
    "FIN"                                                                                                                                                      │
  ],                                                                                                                                                            │
  "other1": "other1",
  "other2": "other2",
  "other3": "other3"
} 
```

### AVRO schema
- Current
```JSON
{                                                                                                                                                                                                                                                                                                                             
  "type": "record",                                                                                                                                                                                                                                                                                                           
  "name": "acct_data",                                                                                                                                                                                                                                                                                                        
  "fields": [                                                                                                                                                                                                                                                                                                                 
    {                                                                                                                                                                                                                                                                                                                         
      "name": "tcp_flags",                                                                                                                                                                                                                                                                                                        
      "type": {                                                                                                                                                                                                                                                                                                               
        "type": "string"                                                                                                                                                                                                                                                                                                      
      }                                                                                                                                                                                                                                                                                                                       
    }
  ]
}
```

- Future
```JSON
{                                                                                                                                                              │
  "type": "record",                                                                                                                                            │    PID USER      PRI  NI  VIRT   RES   SHR S CPU%▽MEM%   TIME+  Command                                                                                      
  "name": "acct_data",                                                                                                                                         │
  "fields": [                                                                                                                                                  │
    {                                                                                                                                                          │
      "name": "tcp_flags",                                                                                                                                     │
      "type": {                                                                                                                                                │
        "type": "array",                                                                                                                                       │
        "items": {                                                                                                                                             │
          "type": "string"                                                                                                                                     │
        }                                                                                                                                                      │
      }                                                                                                                                                        │
    }                                                                                                                                                          │
  ]                                                                                                                                                            │
}       
```
