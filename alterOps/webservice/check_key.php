<?php
$acm_type = "memcache";
$phpbb_root_path = '/www/sites/secret.dotalex.co.uk';
define('IN_PHPBB', true);
$phpEx = substr(strrchr(__FILE__, '.'), 1);
include($phpbb_root_path . 'common.' . $phpEx);
/*
$key = file_get_contents('php://input');
$key = base64_decode($key);
$key = bin2hex($key);
*/
function userInput($string){
  if(get_magic_quotes_gpc()){
    $string = stripslashes($string);
  }
  $string = addslashes($string);
  return $string;
}
function get_profile_fields($user_id)
{
	global $db;

	$sql = 'SELECT *
		FROM ' . PROFILE_FIELDS_DATA_TABLE . '
		WHERE ' . $db->sql_in_set('user_id', array_map('intval', $user_id));
	$result = $db->sql_query($sql);

	$field_data = array();
	while ($row = $db->sql_fetchrow($result))
	{
		$field_data[$row['user_id']] = $row;
	}
	$db->sql_freeresult($result);

	$user_fields = array();

	$fields = array('can_has_servers', 'can_play', 'can_play_expire');
	foreach ($fields as $used_ident)
	{
		foreach ($field_data as $user_id => $row)
		{
			$user_fields[$user_id][$used_ident]['value'] = $row['pf_' . $used_ident];
		}
	}

	return $user_fields;
}
$key = userInput($_GET['key']);
$keysql = "SELECT * FROM server_keys WHERE first8 = '".$key."'";
$keyresult = $db->sql_query($keysql);
$keyexists = 0;
$fullkey = "";
$keyuser = 0;
$type = 0;
$canHaveServers = false;
$canPlay = false;
while ($keyrow = $db->sql_fetchrow($keyresult))
{
    $keyexists = 1;
    $type = $keyrow['type'];
    $fullkey = $keyrow['tigerkey'];
    $keyuser = $keyrow['user_id'];
    $userID = (int)$keyuser;
    $user_id = array($userID);
    $canhave = get_profile_fields($user_id);
    if ($canhave[$userID]['can_play_expire']['value'] <= time())
    {
       $canhave[$userID]['can_play']['value'] = 1;
    }
    if($canhave[$userID]['can_has_servers']['value'] == 1 || $canhave[$userID]['can_has_servers']['value'] == 0 || $canhave[$userID]['can_has_servers']['value'] == "")
    {
        $canHaveServers = true;
    }
    if($canhave[$userID]['can_play']['value'] == 1 || $canhave[$userID]['can_play']['value'] == 0 || $canhave[$userID]['can_play']['value'] == "")
    {
        $canPlay = true;
    }
}
if($keyexists == 1 && $canHaveServers && $canPlay)
{
    echo "1 ".$type." ".$fullkey." ".$keyuser;
}
else
{
    echo "0";
}
?>
