<?php
// standard phpBB setup
$acm_type = "null";
$phpbb_root_path = '/www/sites/secret.dotalex.co.uk/';
define('IN_PHPBB', true);
define('IN_CHECK_BAN', 1);
$phpEx = substr(strrchr(__FILE__, '.'), 1);
include($phpbb_root_path . 'common.' . $phpEx);
include_once($phpbb_root_path . 'includes/functions_profile_fields.' . $phpEx);
if ( !function_exists('group_memberships') )
{
        include_once($phpbb_root_path . 'includes/functions_user.'.$phpEx);
}
$ip = $_SERVER['REMOTE_ADDR'];
if ($ip == "173.60.29.207")
{
$monthsNeeded = 6;
$user->session_begin();
$auth->acl($user->data);
$sid = $db->sql_escape($_GET['s']);
$reason = intval($_GET['r']);
$keysql = "SELECT * FROM phpbb_sessions s, phpbb_users u WHERE session_id = '".$sid."' AND session_onlineplay = 1 AND s.session_user_id = u.user_id";
$keyresult = $db->sql_query($keysql);
$keyuser = 0;
$canPlay = false;
while ($keyrow = $db->sql_fetchrow($keyresult))
{
    $keyuser = $keyrow['session_user_id'];
    $userID = (int)$keyuser;
    $reasonString = 'Cheat detected (#' . $reason . ')';
    $sql = 'SELECT user_id FROM ' . PROFILE_FIELDS_DATA_TABLE . ' WHERE user_id = ' . $userID;
    $result = $db->sql_query($sql);
    $row = $db->sql_fetchrow($result);
    if (empty($row))
    {
        $sql = 'INSERT INTO ' . PROFILE_FIELDS_DATA_TABLE . ' (user_id) VALUES (' . $userID . ')';
        $db->sql_query($sql);
    }
    $sql = 'UPDATE ' . PROFILE_FIELDS_DATA_TABLE . ' SET pf_can_play = 2, pf_can_play_expire = ' . (time() + (24 * 60 * 60 * 5)) . ', pf_can_play_reason = \'' . $reasonString . '\' WHERE user_id = ' . $userID;
    $db->sql_query($sql);
    add_log('admin', 'LOG_USER_DETECT', $reasonString, $keyrow['username']);
    add_log('mod', 0, 0, 'LOG_USER_DETECT', $reasonString, $keyrow['username']);
    add_log('user', $userID, 'LOG_USER_DETECT', $reasonString, $keyrow['username']);
    echo 'Good riddance.';
}
}
else
{
    add_log('admin', 'LOG_ADMIN_AUTH_FAIL');
	echo 'Nope';
}
?>