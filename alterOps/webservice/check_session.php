<?php
$phpbb_root_path = '/www/sites/secret.dotalex.co.uk';
$acm_type = "memcache";
define('IN_PHPBB', true);
define('IN_LOGIN', true);
$phpEx = substr(strrchr(__FILE__, '.'), 1);
include($phpbb_root_path . 'common.' . $phpEx);
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
$sid = userInput($_GET['sid']);
$ipsource = userInput($_GET['ip']);
$keysql = "SELECT * FROM phpbb_sessions WHERE session_id = '".$sid."' AND session_onlineplay = 1";
$keyresult = $db->sql_query($keysql);
$keyuser = 0;
$canPlay = false;
while ($keyrow = $db->sql_fetchrow($keyresult))
{
    $keyuser = $keyrow['session_user_id'];
    $userID = (int)$keyuser;
    $user_id = array($userID);
    $canhave = get_profile_fields($user_id);
        if ($canhave[$userID]['can_play_expire']['value'] <= time())
        {
                $canhave[$userID]['can_play']['value'] = 1;
        }
    if($canhave[$userID]['can_play']['value'] == 1 || $canhave[$userID]['can_play']['value'] == 0 || $canhave[$userID]['can_play']['value'] == "")
    {
        $canPlay = true;
    }
	if(isset($_GET['ip']) && ($keyrow['session_ip'] != $ipsource && $keyrow['session_realip'] != $ipsource))
	{
		$canPlay = false;
	}
}
if($keyuser > 0 && $canPlay)
{
    echo "1 ". $keyuser;
}
else
{
    echo "0";
}
?>