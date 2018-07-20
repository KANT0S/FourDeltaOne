<?php
$phpbb_root_path = '/www/sites/secret.dotalex.co.uk';
$currentLocation = '/keys/';
$isLoggedIn = 0;
$loginError = 0;
$monthsNeeded = 6;
$canHaveServers = false;
$canPlay = false;
$manageKeys = false;
define('IN_PHPBB', true);
define('IN_CHECK_BAN', 1);
define('IN_LOGIN', 1);
$phpEx = substr(strrchr(__FILE__, '.'), 1);
include($phpbb_root_path . 'common.' . $phpEx);
include_once($phpbb_root_path . 'includes/functions_profile_fields.' . $phpEx);
include_once($phpbb_root_path . 'includes/functions_profile_fields.' . $phpEx);
if ( !function_exists('group_memberships') )
{
	include_once($phpbb_root_path . 'includes/functions_user.'.$phpEx);
}
$config['cookie_domain'] = "internlabs.net";
$config['cookie_name'] = "phpbb3_yay";
$user->session_begin();
$auth->acl($user->data);
$user->setup();

function isGroup($userid)
{
    $groups = group_memberships(false,$userid);
	$return = false;
    foreach ($groups as $grouprec)
    {
        if($grouprec['group_id'] == 22 || $grouprec['group_id'] == 11 || $grouprec['group_id'] == 12 || $grouprec['group_id'] == 10 || $grouprec['group_id'] == 4)
		{
			$return = true;
		}
    }  
	return $return;
}


	global $SRVRON;
	global $cache;
	if (($status = $cache->get('_aoserver2status')) === false)
	{
		unset($status);
		$serverip = "server.internlabs.net";
		$port = "3074";
		if($fp=@fsockopen($serverip,$port,$ERROR_NO,$ERROR_STR,1))
		{
			fclose($fp);
			$status = "online";
		}
		else 
		{
			$status = "offline";
		}
		$cache->put('_aoserverstatus', $status, 60);
	}
	if($status == "offline")
	{
		$SRVRON = false;
	}
	else
	{	
		$SRVRON = true;
	}
	
	$tweets = ""; 
	$context = stream_context_create(array(
    'http' => array(
        'timeout' => 1      // Timeout in seconds
    )
));
	
	if (($tweets = $cache->get('_aotweets')) === false)
	{
		
		unset($tweets);
		if($tweets = @file_get_contents("http://twitter.com/statuses/user_timeline/internLabs.json?count=5&exclude_replies=true", 0, $context))
		{
			$cache->put('_aotweets', $tweets, 60);
		}
		else
		{
			$cache->put('_aotweets', $tweets, 10);
		}
	}

function isValidID($tid) {
	if ($tid == USER_FOUNDER)
	{
		return true;
	}
	return false;
}

function userInput($string){
  if(get_magic_quotes_gpc()){
    $string = stripslashes($string);
  }
  if (phpversion() >= '4.3.0'){
    $string = mysql_real_escape_string($string);
  } else{
    $string = mysql_escape_string($string);
  }
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


if(isset($_POST['submit']))
{
    $username = request_var('username', '', true);
    $password = request_var('password', '', true);
    $result = $auth->login($username, $password);
    if ($result['status'] != LOGIN_SUCCESS)
    {
       $loginError = 1; 
    }
}
if($user->data['is_registered'])
{
    $isLoggedIn = 1;
    $userID = $user->data['user_id']; 
    $banReason = check_ban($userID, '', '', 1);
    if($banReason != "")
    {
      $loginError = 1;   
      $isLoggedIn = 0;
    }
    else
    {
        $user_id = array($userID);
        $canhave = get_profile_fields($user_id);
        if($canhave[$userID]['can_has_servers']['value'] == 1 || $canhave[$userID]['can_has_servers']['value'] == 0 || $canhave[$userID]['can_has_servers']['value'] == "")
        {
            $canHaveServers = true;
        }

        if ($canhave[$userID]['can_play_expire']['value'] <= time())
        {
                $canhave[$userID]['can_play']['value'] = 1;
        }

        if($canhave[$userID]['can_play']['value'] == 1 || $canhave[$userID]['can_play']['value'] == 0 || $canhave[$userID]['can_play']['value'] == "")
        {
            $canPlay = true;
        }
        else
        {
            $canHaveServers = false;   
        }
        $keys = array();
        if($canHaveServers)
        {
            $keysql = 'SELECT * FROM server_keys WHERE user_id = '.$userID.' ';
            $keyresult = $db->sql_query($keysql);
            while ($keyrow = $db->sql_fetchrow($keyresult))
            {
                $keys[] = $keyrow;
            }
            $db->sql_freeresult($keyresult);
        }
        $regDate = $user->data['user_regdate'];
        $userTimeFormat = $user->data['user_dateformat'];
        $sixMonthsAgo = strtotime('-'.$monthsNeeded.' months');
		/*if(!isGroup($userID))
        {
            $canHaveServers = false;   
            $canPlay = false;   
        }*/
        if(isset($_GET['download']))
        {
            if((true || isGroup($userID)) && $canPlay)
            {
                header('Content-type: application/zip');
                header('Content-Disposition: attachment; filename="interOps.zip"');
                header("X-Accel-Redirect: /keys/protected/interOps.zip");
                exit();
            }
        } 
        if(isset($_GET['changedescription']))
        {
            if($canHaveServers)
            {
                $keyid = userInput($_GET['keyid']);
                $description = substr(trim(urldecode($_GET['description'])), 0, 20);
                $description = userInput($description);
                $keysql = "UPDATE server_keys SET description='".$description."' WHERE user_id = '".$userID."' AND id = '".$keyid."';";
                $keyresult = $db->sql_query($keysql); 
                $db->sql_freeresult($keyresult);
                echo $description;
                die();
            }
        }
        if(isset($_GET['requestkey']))
        {
            if($canHaveServers)
            {
                if(count($keys) < 15)
                {   
                    $length = 24;
                    $characters = '0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ';
                    $unique = false;
                    
                    while($unique == false)
                    {
                        $string = '';
                        $key = '';
                        $hashkey = '';
                        $first8hash = '';
                        for ($p = 0; $p < $length; $p++) {
                            $string .= $characters[mt_rand(0, strlen($characters)-1)];
                        }
                        $unique = true;
                        $key = $string;
                        $hashkey = hash('tiger192,3', $key);
                        $first8hash = substr($hashkey, 0, 16);
                        
                        $keysql = "SELECT * FROM server_keys WHERE first8 = '".$first8hash."' ";
                        $keyresult = $db->sql_query($keysql);
                        while ($keyrow = $db->sql_fetchrow($keyresult))
                        {
                            $unique = false;
                        }
                        $db->sql_freeresult($keyresult);
                        
                    }
                    
                    $keysql = "INSERT INTO server_keys (`user_id`, `serverkey`, `tigerkey`, `first8`, `description`) VALUES ('".$userID."', '".$key."', '".$hashkey."', '".$first8hash."', '');";
                    $keyresult = $db->sql_query($keysql); 
                    echo $keyresult; 
                    $db->sql_freeresult($keyresult);    
                    $keys[] = array('id'=>0, 'user_id'=>$userID, 'serverkey'=>$key, 'tigerkey'=>$hashkey, 'first8'=>$first8hash);
                    header("Location: ".$currentLocation);
                    die();
                }
            }
        }
		
        if(isset($_GET['downloaddw']))
        {
            if($canHaveServers)
            {
               if(count($keys) > 0)
               {
                    header('Content-type: text/plain');
                    header('Content-Disposition: attachment; filename="dwkey.dat"');
                    foreach($keys as $key)
                    {
                        echo $key['serverkey']."\r\n";
                    }
                    exit();
               }
           }
        }
		
		if(isset($_GET['managekeys']))
		{
			if(isValidID($user->data['user_type']))
			{
				$manageKeys = true;
			}
		}
		
		if(isset($_GET['changetype']))
        {
            if(isValidID($user->data['user_type']))
            {
                $type = userInput($_GET['type']);
                $key = userInput($_GET['key']);
                $keysql = "UPDATE server_keys SET type='" . $type . "' WHERE serverkey = '" . $key . "';";
                $keyresult = $db->sql_query($keysql); 
                $db->sql_freeresult($keyresult);
                echo $type;
                die();
            }
        }
		
		if(isset($_GET['getkeybymember']))
		{
			if(isValidID($user->data['user_type']))
			{
				$tkeys = array();
				$tuser = urldecode($_GET['getkeybymember']);
				
				$sql = "SELECT user_id, username, user_password, user_passchg, user_pass_convert, user_email, user_type, user_login_attempts FROM " . USERS_TABLE . " WHERE username_clean = '" . $db->sql_escape(utf8_clean_string($tuser)) . "'";
				$result = $db->sql_query($sql);
				$row = $db->sql_fetchrow($result);
				$db->sql_freeresult($result); 
				
				$keysql = 'SELECT * FROM server_keys WHERE user_id = ' . $row['user_id'] . ' ';
				$keyresult = $db->sql_query($keysql);
				while ($keyrow = $db->sql_fetchrow($keyresult))
				{
					$tkeys[] = $keyrow;
				}
				$db->sql_freeresult($keyresult);
				
				header('Content-type: text/plain');
				
				if(count($tkeys) == 0)
				{
					echo "";
				}
				else
				{
					$gotOne = false;
					foreach($tkeys as $key)
					{
						if ($gotOne)
						{
							echo "," . $key['type'] . "." . $key['serverkey'];
						}
						else
						{
							$gotOne = true;
							echo $key['type'] . "." . $key['serverkey'];
						}
					}
				}
				
				exit();
			}
		}
    }
}

if(isset($_GET['logout']))
{
    $user->session_kill();
    $user->session_begin();
    header("Location: ".$currentLocation);
}
?>
<html>
<head>
<meta http-equiv="content-type" content="text/html; charset=UTF-8" />
<title>interOps</title>
<script src="http://ajax.googleapis.com/ajax/libs/jquery/1.5/jquery.min.js"></script>
<link rel="stylesheet" type="text/css" href="css2/style.css" />
<!--[if IE]>
<link rel="stylesheet" type="text/css" href="css2/ie.css" />
<![endif]-->
<script type="text/javascript">

function saveDesc(item) {
	var description = $(item).parent().find('.newdescription').val();
	var keyid = $(item).parent().find('.keyid').val();
	$.ajax({
		type: "GET",
		url: "index.php",
		data: "changedescription=1&description="+encodeURI(description)+"&keyid="+keyid,
		async: true,
		timeout: 1000,
		success: function(r) {
			description = r;
		}
	});
	$(item).hide();
	$(item).parent().find('input').hide();
	if($.trim(description) != "")
	{
		description.replace(/</g,"&lt;").replace(/>/g,"&gt;");
		$(item).parent().find('span').html(description);
	}
	else
	{
		$(item).parent().find('span').html('Click to add a description');
	}
	$(item).parent().find('span').show();
	
}

<?php
if($manageKeys)
{
?>
function getIDforVal(val) {
	switch(val)
	{
		case "Unranked":
		  return "1";
		case "Ranked":
		  return "2";
		 case "Official":
		  return "4";
		default:
		  return "2";
	}
}

function getByMember(item) {
	var user = $(item).parent().find('.user').val();
	
	if (user == "")
	{
		alert("Please type in a username.");
	}
	else
	{
		var output = "";
		$.ajax({
			type: "GET",
			url: "index.php",
			data: "getkeybymember=" + encodeURI(user),
			async: true,
			timeout: 2500,
			success: function(data) {
				output = data;
				if (output == "") {
					$("#keymanage").html("Could not find any keys for the specified user.");
				} else {
					var toPrint = "Displaying keys for user <b>" + user + "</b>:<br/><br/><table id='ekeytable'><tr><th>Server Type</th><th>Key</th></tr>";
					var keys = output.split(',');
					if (keys.length > 1)
					{
						for(var i in keys)
						{
							var keytypes = ['', 'Unranked', 'Ranked', 'Unranked', 'Official'];
							var keyPair = keys[i].split('.');
							toPrint += "<tr><td class='typeholder'><span class='type'>" + keytypes[parseInt(keyPair[0])] + "</span><select class='typesel' style='display: none;'><option value='Unranked'>Unranked</option><option value='Ranked'>Ranked</option><option value='Official'>Official</option></select></td><td class='skey'>" + keyPair[1] + "</td></tr>";
						}
					}
					else
					{
						var keytypes = ['', 'Unranked', 'Ranked', 'Unranked', 'Official'];
						var keyPair = output.split('.');
						toPrint += "<tr><td class='typeholder'><span class='type'>" + keytypes[parseInt(keyPair[0])] + "</span><select class='typesel' style='display: none;'><option value='Unranked'>Unranked</option><option value='Ranked'>Ranked</option><option value='Official'>Official</option></select></td><td class='skey'>" + keyPair[1] + "</td></tr>";
					}
					toPrint += "</table>";
					$("#keymanage").html(toPrint);
				}
				
				$('p.backbtn').show();
				
				$('span.type').click(function() {
					$(this).hide();
					$(this).parent().find('select').show().focus();
					$(this).parent().find('select').val($(this).html()).attr('selected', 'selected');
				}); 
				
				$("select.typesel").change(function() {
					var key = $(this).parent().parent().find('td.skey').html();
					var newVal = $(this).val();
					$(this).parent().find('span').html(newVal);
					var valID = getIDforVal(newVal);
					$.ajax({
						type: "GET",
						url: "index.php",
						data: "changetype=1&type=" + valID + "&key=" + key,
						async: true,
						timeout: 2000,
					});
					$(this).parent().find('select').hide();
					$(this).parent().find('span').show();
				});
				
				$("select.typesel").blur(function() {
					$(this).parent().find('select').hide();
					$(this).parent().find('span').show();
				});
			}
		});
	}
}
<?php
}
?>

$(document).ready(function() {
	$('span.description').click(function() {
		$(this).hide();
		$(this).parent().find('input').show().focus();
	}); 
	
<?php
if( $manageKeys )
{
?>	
	$("#keymanage input").keypress(function (e) {
		if ((e.which && e.which == 13) || (e.keyCode && e.keyCode == 13)) {
			getByMember(this);
			return false;
		} else {
			return true;
		}
	});
	
	$('p.backbtn a').click(function() {
		$('p.backbtn').hide();
		$("#keymanage").html("Search for server keys by user: <input class='user' maxlength='40' type='text' value=''/>");
		$("#keymanage input").keypress(function (e) {
			if ((e.which && e.which == 13) || (e.keyCode && e.keyCode == 13)) {
				getByMember(this);
				return false;
			} else {
				return true;
			}
		});
	}); 
	
<?php
}
?>
	$("#keytable input").keypress(function (e) {
        if ((e.which && e.which == 13) || (e.keyCode && e.keyCode == 13)) {
			saveDesc(this);
            return false;
        } else {
            return true;
        }
	});
});
</script>
</head>
<body>
<div class="topBar" >
</div>
<br />
<div class="logo" >
<a href="http://secretschemes.net"><img src="images/logo.png"/></a><br/>
<?php
				global $SRVRON;
		
				if($SRVRON == false)
				{
					echo '<h3>Server <span style="color: red;">Offline</span></h3>';
				}
				else
				{
					echo '<h3>Server <span style="color: green;">Online</span></h3>';
				}
?>
</div>
<center>
<br/>
<br/>
<div id="wrap">
<br>
<div class="wrapbg">
<span class="corners-top"><span></span></span>
<div id="content">
News
<hr class="hr1" />
<br />
<ul>
<?php
function clickable_link($text)
{
$text = preg_replace('#(script|about|applet|activex|chrome):#is', "\\1:", $text);
$ret = ' ' . $text;
$ret = preg_replace("#(^|[\n ])([\w]+?://[\w\#$%&~/.\-;:=,?@\[\]+]*)#is", "\\1<a href=\"\\2\">\\2</a>", $ret);
$ret = preg_replace("#(^|[\n ])((www|ftp)\.[\w\#$%&~/.\-;:=,?@\[\]+]*)#is", "\\1<a href=\"http://\\2\">\\2</a>", $ret);
$ret = preg_replace("#(^|[\n ])([a-z0-9&\-_.]+?)@([\w\-]+\.([\w\-\.]+\.)*[\w]+)#i", "\\1<a href=\"mailto:\\2@\\3\">\\2@\\3</a>", $ret);
$ret = preg_replace('/@(\w+)/','<a href="http://twitter.com/$1">@$1</a>',$ret);
$ret = preg_replace('/\s+#(\w+)/',' <a href="http://search.twitter.com/search?q=%23$1">#$1</a>', $ret);
$ret = substr($ret, 1);
return $ret;
}
if($tweets != "")
{
$obj = json_decode($tweets);  
foreach($obj as $var => $value)
{   
	echo "<li>".clickable_link($obj[$var]->text)."</li>";                            
}
}
else
{
echo "Twitter feed not available!";
}

?>
<ul>
</div>
<span class="corners-bottom"><span></span></span>
</div>
<br/>
<div class="wrapbg">
<span class="corners-top"><span></span></span>
<div id="content">
<?php
if($isLoggedIn == 1)
{
if ($manageKeys)
{
?>
Manage Keys<p class="backBtn" style="display: none;">&nbsp;|&nbsp;<a href="#">Back</a></p>
<hr class="hr1" />
<br />
<center>
<?php
echo "Welcome ".$user->data['username']."!";
?>
<br /><br />
<div id='errorBox'>
<div id='keymanage'>
Search for server keys by user:
<input class='user' maxlength='40' type='text' value=''/>
</div>
</div>
</center>
<?php
}
else
{
?>
Download
<?php
if(isValidID($user->data['user_type']))
{
?>
<p class="backBtn">|&nbsp;<a href="?managekeys=1">Manage Server Keys</a></p>
<?php
}
?>
<hr class="hr1" />
<br />
<center>
<?php
echo "Welcome ".$user->data['username']."!";
if(true || isGroup($userID))
{
?><br /><br />
<div id='errorBox'>Notice: this is a beta release. It's unstable and support is limited.
<hr class="hr1">
<div id='keycont'>
<a class='ltitle'>Your server keys:</a>
<?php
if(!$canHaveServers)
{
    echo "You can't have any servers.<br/>";   
}
elseif(count($keys) == 0)
{
    echo "You don't have any keys yet.<br/>";
}
else
{
    $keytypes = array('', 'Unranked', 'Ranked', 'Unranked', 'Official');
    echo "<table id='keytable'>";
    echo "<tr><th>sv_licensenum</th><th>Server Type</th><th>Key</th><th>Description</th></tr>";
    $count = 0;
    foreach($keys as $key)
    {
        $description = "Click to add a description";
        $description2 = "";
        if($key['description'] != "")
        {
            $description = htmlentities($key['description']);
            $description2 = $key['description'];
        }
        echo "<tr><td>".$count."</td><td>".$keytypes[$key['type']]."</td><td>".$key['serverkey']."</td><td class='desc'><span class='description'>".$description."</span><input style='display: none;' class='newdescription' maxlength='20' type='text' value='".$description2."'/><input type='hidden' class='keyid' name='keyid' value='".$key['id']."'></td></tr>";
        $count++;
    }
    echo "</table>";
}
if(count($keys) < 15 && $canHaveServers)
{
?>
<a href="?requestkey">Request key (<?php echo 15-count($keys); ?> left)</a><br />
<?php
}
if(count($keys) > 0 && $canHaveServers)
{
?>
<a href="?downloaddw">Download dwkey.dat</a>
<?php
}
?>
</div></div>
<br/><br />
<?php
if($canPlay)
{
?>
<a href="?download=1"><img src="images/download-now-button.png"></a>
<?php
}
else
{
    echo "For some reason, you have been disallowed from playing this. Please contact <a href='mailto:admin@internlabs.net'>us</a>";
}
?>
</center>
<?php
}
else
{
?>
<br/><br />
<div id='errorBox'>You can't download this. You have to be staff or contributor.</div>
</center>
<?php
}
}
}
else
{   
?>
Login
<hr class="hr1" />
<center>
<?php
if($loginError == 1)
{
    $err = (!$config['board_contact']) ? sprintf($user->lang[$result['error_msg']], '', '') : sprintf($user->lang[$result['error_msg']], '<a href="mailto:' . htmlspecialchars($config['board_contact']) . '">', '</a>');
    if($banReason == "")
    {
        echo "<div id='errorBox'>Login error, try again.<br/>".$err."</div>";
    }
    else
    {
       echo "<div id='errorBox'>Banned, reason: '".$banReason."'</div>"; 
    }
}
?>
<br/><p class="pNote">Please login with your internLabs forum account, you can <a href="http://internlabs.net/ucp.php?mode=register">register here</a> and <a href="http://internlabs.net/ucp.php?mode=sendpassword">request a new password here</a> in case of password loss.</p><br />
<form method="POST">
<table>
<tr><td class="lTxt">Username:</td> <td><input name="username" id="username" type="text" /></td></tr>
<tr><td class="lTxt">Password:</td> <td><input type="password" name="password" /></td></tr>
<tr><td></td><td><input name="submit" type="submit" value="" class="lgnBtn"/></td></tr>
</table>
</form>
</center>
<?php
}
?>
</div>
<span class="corners-bottom"><span></span></span>
</div>
<br />
<div id="footer">
Copyright &copy; 2010-<?php echo date('Y'); ?>, <a href="http://secretschemes.net">internLabs</a>. All rights reserved. <?php
if($isLoggedIn == 1)
{
    ?>
    <a href="?logout=1">Logout</a>
    <?php
}
?>
</div>
</div>
</center>
</body>
</html>
