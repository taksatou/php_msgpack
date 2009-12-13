<?php
$data = array(1, -1, true, false, null, "key" => "value");

// simple procedural usage
$serialized = msgpack_pack($data);
var_dump(msgpack_unpack($serialized));

// OO usage
$obj = new Msgpack;
$serialized = $obj->packer($data);
var_dump($obj->unpacker($serialized));

// stream deserialize
$buf = pack("c*", 0x00, 0x40, 0x7f, 0xe0, 0xf0, 0xff);
$obj->feed($buf);
while ($obj->remain()) {
  var_dump($obj->unpacker(NULL));
}
