<?php

mysql_connect('localhost','aiw3','NRvFafRdHVcUzBRR') or die(mysql_error());
mysql_select_db('aiw3') or die(mysql_error());

$time = time();
$servers = 0;
$filename = "/var/www/aiw3.net/stats/dpmaster.info";
$handle = fopen($filename, 'r');
if($data = fread($handle, filesize($filename)))
{
	$data = explode("\n", $data);
	$servers = count($data);
}
fclose($handle);

$players = file_get_contents('http://server.aiw3.net:3035/users');

mysql_query("INSERT INTO stats (time, servers, players) VALUES ('".$time."', '".$servers."', '".$players."')");
?>
