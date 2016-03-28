loadi @0, 0 
loadi @1, 1
loadi @2, 49
loadi @3, 5 
loadi @6, 0
add @4, @2, @0
add @5, @3, @0
and @7, @4, @1
jz @7, 10
add @6, @6, @5
srl @4, @4, @1
sll @5, @5, @1
jnz @4, 7
add @5, @5, @0
add @6, @6, @0
add @4, @3, @0
add @5, @2, @0
and @7, @4, @1
jz @7, 20
sub @6, @6, @5
srl @4, @4, @1
sll @5, @5, @1
jnz @4, 17
add @5, @5, @0
add @6, @6, @0
stop
