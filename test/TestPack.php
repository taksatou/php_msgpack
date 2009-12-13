<?php
require_once('PHPUnit/Framework.php');

function check($data)
{
  if ($data === msgpack_unpack(msgpack_pack($data)))
    return true;
  else
    return false;
}

class TestPack extends PHPUnit_Framework_TestCase
{
  function test_pack()
  {
    $test_data = array(0, 1, 127, 128, 255, 256, 65535, 65536,
		       -1, -32, -33, -128, -129, -32768, -32769,
		       1.0,
		       "", "a", "a"*31, "a"*32,
		       None, True, False,
		       array(), array(array()), array(array(), NULL),
		       array(10 => 0),
		       (1<<23)
		       );
    foreach ($test_data as $td) {
      $this->assertTrue(check($td));
    }
  }
}
