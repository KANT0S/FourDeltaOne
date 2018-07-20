<?php
// standard phpBB setup
if($SERVER['REMOTE_ADDR'] == "127.0.0.1" || $SERVER['REMOTE_ADDR'] == "94.23.147.71" || $SERVER['REMOTE_ADDR'] == "188.165.193.75")
{
	unset($SERVER['REMOTE_ADDR']);
	$SERVER['REMOTE_ADDR'] = $SERVER['HTTP_X_REAL_IP'];
}
if($SERVER['HTTP_X_FORWARDED_FOR'] == "127.0.0.1" || $SERVER['HTTP_X_FORWARDED_FOR'] == "94.23.147.71" || $SERVER['HTTP_X_FORWARDED_FOR'] == "188.165.193.75")
{
	unset($SERVER['HTTP_X_FORWARDED_FOR']);
}
function get_ip_address() {
    foreach (array('HTTP_CLIENT_IP', 'HTTP_X_FORWARDED_FOR', 'HTTP_X_FORWARDED', 'HTTP_X_CLUSTER_CLIENT_IP', 'HTTP_FORWARDED_FOR', 'HTTP_FORWARDED', 'REMOTE_ADDR') as $key) {
        if (array_key_exists($key, $_SERVER) === true) {
            foreach (explode(',', $_SERVER[$key]) as $ip) {
                if($SERVER[$key] == "127.0.0.1" || $SERVER[$key] == "94.23.147.71" || $SERVER[$key] == "188.165.193.75")
                {
                        unset($_SERVER[$key]);
                }
                if (valid_ip($ip) !== false) {
                    return $ip;
                }
            }
        }
    }
}
get_ip_address();
$acm_type = "memcache";
$phpbb_root_path = '/www/sites/secret.dotalex.co.uk';
define('IN_PHPBB', true);
define('IN_CHECK_BAN', 1);
define('IN_LOGIN', 1);
$phpEx = substr(strrchr(__FILE__, '.'), 1);
include($phpbb_root_path . 'common.' . $phpEx);
include_once($phpbb_root_path . 'includes/functions_profile_fields.' . $phpEx);
if ( !function_exists('group_memberships') )
{
	include_once($phpbb_root_path . 'includes/functions_user.'.$phpEx);
}
$monthsNeeded = 6;
function valid_ip($ip)
{
	return ( ! preg_match( "/^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}$/", $ip)) ? FALSE : TRUE;
}
$user->session_begin();
$auth->acl($user->data);
$canPlay = false;
function isGroup($userid)
{
    $groups = group_memberships(false,$userid);
	$return = false;
    foreach ($groups as $grouprec)
    {
        if($grouprec['group_id'] == 22 || $grouprec['group_id'] == 9 || $grouprec['group_id'] == 11 || $grouprec['group_id'] == 12 || $grouprec['group_id'] == 10 || $grouprec['group_id'] == 4)
		{
			$return = true;
		}
    }  
	return $return;
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

	$fields = array('can_has_servers', 'can_play', 'can_play_reason', 'can_play_expire');
	foreach ($fields as $used_ident)
	{
		foreach ($field_data as $user_id => $row)
		{
			$user_fields[$user_id][$used_ident]['value'] = $row['pf_' . $used_ident];
		}
	}

	return $user_fields;
}


function check_ban($user_id = false, $user_ips = false, $user_email = false, $return = false)
	{
		global $config, $db;

		$banned = false;
		$cache_ttl = 3600;
		$where_sql = array();

		$sql = 'SELECT ban_ip, ban_userid, ban_email, ban_exclude, ban_give_reason, ban_end
			FROM ' . BANLIST_TABLE . '
			WHERE ';

		// Determine which entries to check, only return those
		if ($user_email === false)
		{
			$where_sql[] = "ban_email = ''";
		}

		if ($user_ips === false)
		{
			$where_sql[] = "(ban_ip = '' OR ban_exclude = 1)";
		}

		if ($user_id === false)
		{
			$where_sql[] = '(ban_userid = 0 OR ban_exclude = 1)';
		}
		else
		{
			$cache_ttl = ($user_id == ANONYMOUS) ? 3600 : 0;
			$_sql = '(ban_userid = ' . $user_id;

			if ($user_email !== false)
			{
				$_sql .= " OR ban_email <> ''";
			}

			if ($user_ips !== false)
			{
				$_sql .= " OR ban_ip <> ''";
			}

			$_sql .= ')';

			$where_sql[] = $_sql;
		}

		$sql .= (sizeof($where_sql)) ? implode(' AND ', $where_sql) : '';
		$result = $db->sql_query($sql, $cache_ttl);

		$ban_triggered_by = 'user';
		while ($row = $db->sql_fetchrow($result))
		{
			if ($row['ban_end'] && $row['ban_end'] < time())
			{
				continue;
			}

			$ip_banned = false;
			if (!empty($row['ban_ip']))
			{
				if (!is_array($user_ips))
				{
					$ip_banned = preg_match('#^' . str_replace('\*', '.*?', preg_quote($row['ban_ip'], '#')) . '$#i', $user_ips);
				}
				else
				{
					foreach ($user_ips as $user_ip)
					{
						if (preg_match('#^' . str_replace('\*', '.*?', preg_quote($row['ban_ip'], '#')) . '$#i', $user_ip))
						{
							$ip_banned = true;
							break;
						}
					}
				}
			}

			if ((!empty($row['ban_userid']) && intval($row['ban_userid']) == $user_id) ||
				$ip_banned ||
				(!empty($row['ban_email']) && preg_match('#^' . str_replace('\*', '.*?', preg_quote($row['ban_email'], '#')) . '$#i', $user_email)))
			{
				if (!empty($row['ban_exclude']))
				{
					$banned = false;
					break;
				}
				else
				{
					$banned = true;
					$ban_row = $row;

					if (!empty($row['ban_userid']) && intval($row['ban_userid']) == $user_id)
					{
						$ban_triggered_by = 'user';
					}
					else if ($ip_banned)
					{
						$ban_triggered_by = 'ip';
					}
					else
					{
						$ban_triggered_by = 'email';
					}

					// Don't break. Check if there is an exclude rule for this user
				}
			}
		}
		$db->sql_freeresult($result);

		if ($banned && !$return)
		{
			global $template;

			// If the session is empty we need to create a valid one...
			if (empty($this->session_id))
			{
				// This seems to be no longer needed? - #14971
//				$this->session_create(ANONYMOUS);
			}

			// Initiate environment ... since it won't be set at this stage
			$this->setup();

			// Logout the user, banned users are unable to use the normal 'logout' link
			if ($this->data['user_id'] != ANONYMOUS)
			{
				$this->session_kill();
			}

			// We show a login box here to allow founders accessing the board if banned by IP
			if (defined('IN_LOGIN') && $this->data['user_id'] == ANONYMOUS)
			{
				global $phpEx;

				$this->setup('ucp');
				$this->data['is_registered'] = $this->data['is_bot'] = false;

				// Set as a precaution to allow login_box() handling this case correctly as well as this function not being executed again.
				define('IN_CHECK_BAN', 1);

				login_box("index.$phpEx");

				// The false here is needed, else the user is able to circumvent the ban.
				$this->session_kill(false);
			}

			// Ok, we catch the case of an empty session id for the anonymous user...
			// This can happen if the user is logging in, banned by username and the login_box() being called "again".
			if (empty($this->session_id) && defined('IN_CHECK_BAN'))
			{
				$this->session_create(ANONYMOUS);
			}


			// Determine which message to output
			$till_date = ($ban_row['ban_end']) ? $this->format_date($ban_row['ban_end']) : '';
			$message = ($ban_row['ban_end']) ? 'BOARD_BAN_TIME' : 'BOARD_BAN_PERM';

			$message = sprintf($this->lang[$message], $till_date, '<a href="mailto:' . $config['board_contact'] . '">', '</a>');
			$message .= ($ban_row['ban_give_reason']) ? '<br /><br />' . sprintf($this->lang['BOARD_BAN_REASON'], $ban_row['ban_give_reason']) : '';
			$message .= '<br /><br /><em>' . $this->lang['BAN_TRIGGERED_BY_' . strtoupper($ban_triggered_by)] . '</em>';

			// To circumvent session_begin returning a valid value and the check_ban() not called on second page view, we kill the session again
			$this->session_kill(false);

			// A very special case... we are within the cron script which is not supposed to print out the ban message... show blank page
			if (defined('IN_CRON'))
			{
				garbage_collection();
				exit_handler();
				exit;
			}

			trigger_error($message);
		}

		return ($banned && $ban_row['ban_give_reason']) ? $ban_row['ban_give_reason'] : $banned;
	}

 
// session stuff will not be needed as this occurs from a non-client session, but we need $user->setup it seems
if (empty($user->lang))
{
	$user->setup();
}

$user->add_lang('ucp');

// get variables
//$data = request_var('data', '', true);
//if(!isset($_GET['username']))
//{
    $data = file_get_contents('php://input');
    $data = explode('  ', $data);
    
    $username = trim(htmlspecialchars(str_replace(array("\r\n", "\r", "\0"), array("\n", "\n", ''), $data[0]), ENT_COMPAT, 'UTF-8'));
    $password = trim(htmlspecialchars(str_replace(array("\r\n", "\r", "\0"), array("\n", "\n", ''), $data[1]), ENT_COMPAT, 'UTF-8'));
    
    

/*
}
else
{
    $username = $_GET['username'];
    $password = $_GET['password'];    
}*/
// perform login from $auth. we don't want autologon, viewonline nor admin access for the session
    $result = $auth->login($username, $password, false, false, false);
    if($result['status'] == LOGIN_SUCCESS)
    {
        $userID = $user->data['user_id'];
        $user_id = array($userID);
        $canhave = get_profile_fields($user_id);

        if ($canhave[$userID]['can_play_expire']['value'] <= time())
	{
		$canhave[$userID]['can_play']['value'] = 1;
	}

        if($canhave[$userID]['can_play']['value'] == 1 || $canhave[$userID]['can_play']['value'] == 0 || $canhave[$userID]['can_play']['value'] == "")
        {
                $canPlay = true;
                
                $regDate = $user->data['user_regdate'];
            $sixMonthsAgo = strtotime('-'.$monthsNeeded.' months');
            if (true || isGroup($user->data['user_id'])) 
            {
                $canPlay = true; 
                $keysql = "UPDATE phpbb_sessions SET session_onlineplay = 1, session_realip = '".htmlspecialchars(get_ip_address(), ENT_QUOTES)."' WHERE session_id = '".$user->session_id."';";
                $keyresult = $db->sql_query($keysql); 
                $db->sql_freeresult($keyresult);
            }
            else
            {
                     $result['status'] = 'nope';  
                    $result['error_msg'] = 'interOps is currently down for maintenance.';
            }
            
        }
        else
        {
            $result['status'] = 'nope';  
            //$result['error_msg'] = 'User is not allowed to play'; 
            $result['error_msg'] = 'Online playing privileges revoked';

	    if ($canhave[$userID]['can_play_reason']['value'])
	    {
		$result['error_msg'] .= ' - ' . str_replace('#', '@', $canhave[$userID]['can_play_reason']['value']);
            }

	    if ($canhave[$userID]['can_play_expire']['value'])
	    {
                $result['error_msg'] .= ' (will expire in ' . duration($canhave[$userID]['can_play_expire']['value'] - time()) . ')';
            }
        }
            $banReason = check_ban($userID, '', '', 1);
            if($banReason != "")
            {
                    $result['status'] = 'nope';  
                    $result['error_msg'] = 'User is banned';   
                    $canPlay = false;
            }
        
    }
// start buffering (to allow kill)
ob_start();

// output the results
echo (($result['status'] == LOGIN_SUCCESS) ? 'ok' : 'fail') . '#';
echo (($result['error_msg']) ? ((isset($user->lang[$result['error_msg']])) ? $user->lang[$result['error_msg']] : $result['error_msg']) : 'Success.') . '#';
echo (($result['status'] == LOGIN_SUCCESS) ? $user->data['user_id'] : '1') . '#';
echo (($result['status'] == LOGIN_SUCCESS) ? $user->data['username'] : 'Anonymous') . '#';
echo (($result['status'] == LOGIN_SUCCESS) ? $user->data['user_email'] : 'anonymous@example.com') . '#';
echo (($result['status'] == LOGIN_SUCCESS) ? $user->session_id : '0') . '#';

//Deleted, used for login verify now
// kill the session
if(!$canPlay)
{
    $user->session_kill(false);
}

// and flush the contents
ob_end_flush();
exit;

function format_duration($seconds) {

    $periods = array(
        'centuries' => 3155692600,
        'decades' => 315569260,
        'years' => 31556926,
        'months' => 2629743,
        'weeks' => 604800,
        'days' => 86400,
        'hours' => 3600,
        'minutes' => 60,
        'seconds' => 1
    );

    $durations = array();

    foreach ($periods as $period => $seconds_in_period) {
        if ($seconds >= $seconds_in_period) {
            $durations[$period] = floor($seconds / $seconds_in_period);
            $seconds -= $durations[$period] * $seconds_in_period;
        }
    }
    
    return $durations;

}

function duration($seconds)
{
	$data = format_duration($seconds);
	$data2 = array();

	foreach ($data as $unit => $amount)
	{
		$data2[] = $amount . ' ' . $unit;
	}

	return implode(', ', $data2);
}
