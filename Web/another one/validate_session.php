<?php
	$phpbb_root_path = '';
	$acm_type = "memcache";
	define('IN_PHPBB', true);
	define('IN_LOGIN', true);
	$phpEx = substr(strrchr(__FILE__, '.'), 1);
	include($phpbb_root_path . 'common.' . $phpEx);
	
	function userInput($string)
	{
		if (get_magic_quotes_gpc())
		{
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
		$fields = array('can_play');
		
		foreach ($fields as $used_ident)
		{
			foreach ($field_data as $user_id => $row)
			{
				$user_fields[$user_id][$used_ident]['value'] = $row['pf_' . $used_ident];
			}
		}

		return $user_fields;
	}
	
	$data = file_get_contents('php://input');
	$data = explode('&&', $data);
		
	$sid = trim(htmlspecialchars(str_replace(array("\r\n", "\r", "\0"), array("\n", "\n", ''), $data[0]), ENT_COMPAT, 'UTF-8'));
	$ipsource = trim(htmlspecialchars(str_replace(array("\r\n", "\r", "\0"), array("\n", "\n", ''), $data[1]), ENT_COMPAT, 'UTF-8'));
	
	//$sid = userInput($_GET['sid']);
	//$ipsource = userInput($_GET['ip']);
	
	if ($sid == '' || $ipsource == '')
	{
		echo "Invalid request";
		exit();
	}
	
	$keysql = "SELECT session_user_id FROM phpbb_sessions WHERE session_id = '".$sid."'";
	$keyresult = $db->sql_query($keysql);
	$keyuser = 0;
	$canPlay = false;
	
	while ($keyrow = $db->sql_fetchrow($keyresult))
	{
		$keyuser = $keyrow['session_user_id'];
		$userID = (int)$keyuser;
		$user_id = array($userID);
		$canhave = get_profile_fields($user_id);
			
		if ($canhave[$userID]['can_play']['value'] == 1 || $canhave[$userID]['can_play']['value'] == 0 || $canhave[$userID]['can_play']['value'] == "")
		{
			$canPlay = true;
		}
		
		if (isset($_GET['ip']) && ($keyrow['session_ip'] != $ipsource && $keyrow['session_realip'] != $ipsource))
		{
			$canPlay = false;
		}
	}
	
	// TODO: $canPlay foreach
	echo 'sessionResult' . '#';
	echo (($keyuser > 0 && $keysql) ? $sid : '0') . '#';
	echo (($keyuser > 0 && $keysql) ? '1' : '0') . '#';
	echo (($keyuser > 0 && $keysql) ? $userID : '0') . '#';
	echo (($keyuser > 0 && $keysql) ? '0' : '0') . '#';
?>
