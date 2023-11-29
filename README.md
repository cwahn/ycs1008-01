# YCS1008-01 WebSocket Server

http://arsvivendi.io/ycs1008

## Topology
Messages from each clients will be broadcasted to the other clients connected to the same server end point.

## Protocol
```
start: "start"
torque: "t<int>"
pos: "p<pos>"
end: "stop"
``` 