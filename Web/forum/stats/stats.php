<?php

define('IN_PHPBB', true);
$phpbb_root_path = (defined('PHPBB_ROOT_PATH')) ? PHPBB_ROOT_PATH : '../';
$phpEx = substr(strrchr(__FILE__, '.'), 1);
include($phpbb_root_path . 'common.' . $phpEx);
include($phpbb_root_path . 'includes/functions_display.' . $phpEx);
// others
include($phpbb_root_path . 'stats/nicejson.' . $phpEx);

// Start session management
$user->session_begin();
$auth->acl($user->data);
$user->setup('viewforum');

global $cache;

$data = "";

if (($data = $cache->get('_dediserverlist')) === false) {
	$filename = "/var/www/aiw3.net/stats/dpmaster.info";
	$handle = fopen($filename, 'r');
	if($data = fread($handle, filesize($filename)))
	{
		if($data != "")
		{
			$cache->put('_dediserverlist', $data, 30);
		}
	}
	fclose($handle);
}

$data = explode("\n", trim($data));

foreach ($data as $d)
{
	list($ip, $status, $game, $type, $map, $hostname, $update) = explode(',', $d);

	$serverList = array(
			"ip" => $ip,
			"status" => $status,
			"game" => $game,
			"type" => $type,
			"map" => $map,
			"hostname" => utf8_encode(htmlspecialchars($hostname)),
		);

	echo json_format($serverList);
	echo "\n";
}

?>
