<?php
require_once('PHPUnit/Framework.php');

function check($length, $obj)
{
  $v = msgpack_pack($obj);
  if (strlen($v) != $length) {
    return false;
  }
  if (msgpack_unpack($v) !== $obj) {
    return false;
  }
  return true;
}

function check_array($overhead, $num)
{
  $a = array();
  for ($i = 0; $i < $num; $i++) {
    $a []= NULL;
  }
  /* php array is assoc list */
  return check(($num * 2 + $overhead), $a);
}
    
function check_raw ($overhead, $num)
{
  $str = "";
  for ($i = 0; $i < $num; $i++) {
    $str .= " ";
  }
  return check($num + $overhead, $str);
}

function match($obj, $buf)
{
  if (msgpack_pack($obj) != $buf) {
    return false;
  }
  if (msgpack_unpack($buf) != $obj) {
    return false;
  }
  return true;
}

class TestCase extends PHPUnit_Framework_TestCase
{
  function test_1()
  {
    $a = array(NULL, True, False, 0, 1,
	       (1 << 6), (1 << 7) - 1, -1, -((1<<5)-1), -(1<<5));
    foreach ($a as $o) {
      $this->assertTrue(check(1, $o));
    }
  }

  function test_2()
  {
    $a = array(1 << 7, (1 << 8) - 1,
	       -((1<<5)+1), -(1<<7));
    foreach ($a as $o) {
      $this->assertTrue(check(2, $o));
    }
  }

  function test_3()
  {
    $a = array(1 << 8, (1 << 16) - 1,
	       -((1<<7)+1), -(1<<15));
    foreach ($a as $o) {
      $this->assertTrue(check(3, $o));
    }
  }

  function test_5()
  {
    $a = array(1 << 16, (1 << 32) - 1,
	       -((1<<15)+1), -(1<<31));
    foreach ($a as $o) {
      $this->assertTrue(check(5, $o));
    }
  }

  function test_9()
  {
    $a = array(1 << 32, -((1<<31)+1),
	       1.0, 0.1, -0.1, -1.0);
    foreach ($a as $k => $o) {
      $this->assertTrue(check(9, $o), $msg);
    }
  }

  function test_fixraw()
  {
    $this->assertTrue(check_raw(1, 0));
    $this->assertTrue(check_raw(1, (1<<5) - 1));
  }

  function test_raw16()
  {
    $this->assertTrue(check_raw(3, (1 << 5)));
    $this->assertTrue(check_raw(3, (1<<16) - 1));
  }

  function test_raw32()
  {
    $this->assertTrue(check_raw(5, 1<<16));
  }

  function test_fixarray()
  {
    $this->assertTrue(check_array(1, 0));
    $this->assertTrue(check_array(1, (1 << 4) - 1));
  }

  function test_array16()
  {
    $this->assertTrue(check_array(3, 1 << 4));
    /* max array elements is 1310077 */
    //    $this->assertTrue(check_array(3, (1<<16)-1));
  }

  function test_array32()
  {
    //    $this->assertTrue(check_array(5, (1<<16)));
  }

  function test_match()
  {
    $cases = array(
		   array(NULL, pack("c", 0xc0)),
		   array(False, pack("c", 0xc2)),
		   array(True, pack("c", 0xc3)),
		   array(0, pack("c", 0x00)),
		   array(127, pack("c", 0x7f)),
		   array(128, pack("c*", 0xcc, 0x80)),
		   array(256, pack("c*", 0xcd, 0x01, 0x00)),
		   array(-1, pack("c", 0xff)),
		   array(-33, pack("c*",  0xd0, 0xdf)),
		   array(-129, pack("c*",  0xd1, 0xff, 0x7f)),
		   array(1.0, pack("c*", 0xcb, 0x3f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00)),
		   array(array(), pack("c", 0x80)),
		   array(range(0, 14), pack("c*", 0x8f, 0x00, 0x00, 0x01, 0x01,
					    0x02, 0x02, 0x03, 0x03, 0x04, 0x04,
					    0x05, 0x05, 0x06, 0x06, 0x07, 0x07,
					    0x08, 0x08, 0x09, 0x09, 0x0a, 0x0a,
					    0x0b, 0x0b, 0x0c, 0x0c, 0x0d, 0x0d,
					    0x0e, 0x0e)),
		   array(range(0, 15), pack("c*", 0xde, 0x00, 0x10, 0x00, 0x00,
					    0x01, 0x01, 0x02, 0x02, 0x03, 0x03,
					    0x04, 0x04, 0x05, 0x05, 0x06, 0x06,
					    0x07, 0x07, 0x08, 0x08, 0x09, 0x09,
					    0x0a, 0x0a, 0x0b, 0x0b, 0x0c, 0x0c,
					    0x0d, 0x0d, 0x0e, 0x0e, 0x0f, 0x0f))
		   );
    foreach ($cases as $k => $x) {
      $this->assertTrue(match($x[0], $x[1]), "$k");
    }
  }
}
