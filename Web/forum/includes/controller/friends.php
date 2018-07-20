<?php
global $phpbb_root_path;
include($phpbb_root_path . 'includes/functions_privmsgs.php');

class aiw_controller_friends
{
	public function main()
	{
		global $db, $template, $user;

		$template->assign_var('PAGE_TITLE', 'Friends');

		if (!$user->data['is_registered'])
		{
			header('Location: /');
			exit;
		}

		$sql = 'SELECT f.*, u.username, u.user_colour, u.user_avatar, u.user_avatar_type, u.user_avatar_width, u.user_avatar_height FROM aiw3_xnp.friends f, aiw3.phpbb_users u WHERE u.user_id = f.friend_id AND f.user_id = ' . intval($user->data['user_id']) . ' ORDER BY u.user_id ASC';

		$result = $db->sql_query($sql);

		while ($row = $db->sql_fetchrow($result))
		{
			$type = ($row['friend']) ? 'friends' : 'pending';
			if ($result->num_rows == 0) {

			} else {
				$template->assign_vars('.' . $type, '');
				$template->assign_block_vars($type, array(
					'USERNAME' => get_username_string('full', $row['friend_id'], $row['username'], $row['user_colour']),
					'U_ONLINE' => ($this->is_user_online($row['friend_id'])) ? ' style="border:1px solid #006400"' : '',
					'U_AVATAR' => ($row['user_avatar']) ? get_user_avatar_location($row['user_avatar'], $row['user_avatar_type'], $row['user_avatar_width'], $row['user_avatar_height']) : '/assets/img/unknown.jpg',
					'U_DENY' => './friends/deny.php?u=' . $row['friend_id'],
					'U_PROFILE' => get_username_string('profile', $row['friend_id'], $row['username']),
				));
			}
		}

		// get friend requests
		$sql = 'SELECT f.*, u.username, u.user_colour, u.user_avatar, u.user_avatar_type, u.user_avatar_width, u.user_avatar_height FROM aiw3_xnp.friends f, phpbb_users u WHERE u.user_id = f.user_id AND f.friend = 0 AND f.friend_id = ' . intval($user->data['user_id']) . ' ORDER BY u.user_id ASC';

		$result = $db->sql_query($sql);

		while ($row = $db->sql_fetchrow($result))
		{
			$type = 'requests';
			if ($result->num_rows == 0) {

			} else {
				$template->assign_vars('.' . $type, '');
				$template->assign_block_vars($type, array(
					'USERNAME' => get_username_string('full', $row['user_id'], $row['username'], $row['user_colour']),
					'U_ONLINE' => ($this->is_user_online($row['friend_id'])) ? ' style="border:1px solid #006400"' : '',
					'U_AVATAR' => ($row['user_avatar']) ? get_user_avatar_location($row['user_avatar'], $row['user_avatar_type'], $row['user_avatar_width'], $row['user_avatar_height']) : '/assets/img/unknown.jpg',
					'U_ACCEPT' => './friends/accept.php?u=' . $row['user_id'],
					'U_DENY' => './friends/deny.php?u=' . $row['user_id'],
					'U_PROFILE' => get_username_string('profile', $row['user_id'], $row['username']),
				));
			}
		}

	}

	public function accept_friend($self, $friend)
	{
		global $db;

		$self = intval($self);
		$friend = intval($friend);

		$sql = 'UPDATE aiw3_xnp.friends SET friend = 1 WHERE user_id = ' . $friend . ' AND friend_id = ' . $self;
		$db->sql_query($sql);

		$sql = 'INSERT INTO aiw3_xnp.friends ' . $db->sql_build_array('INSERT', array(
			'user_id' => $self,
			'friend_id' => $friend,
			'friend' => 1
		));

		$db->sql_query($sql);
	}


	public function add_friend($self, $friend)
	{
		global $db;

		$self = intval($self);
		$friend = intval($friend);

		// first check if the attempted friend is already a friend of ours
		$sql = 'SELECT user_id FROM aiw3_xnp.friends WHERE user_id = ' . $self . ' AND friend_id = ' . $friend;

		$result = $db->sql_query($sql);

		if ($db->sql_fetchrow($result))
		{
			return 'This user is already your friend.';
		}

		// then check if this is a pending request
		$sql = 'SELECT user_id FROM aiw3_xnp.friends WHERE friend_id = ' . $self . ' AND user_id = ' . $friend . ' AND friend = 0';
		$result = $db->sql_query($sql);

		if ($db->sql_fetchrow($result))
		{
			$this->accept_friend($self, $friend);
			return false;
		}

		// and lastly, just send the friend request
		$sql = 'INSERT INTO aiw3_xnp.friends ' . $db->sql_build_array('INSERT', array(
			'user_id' => $self,
			'friend_id' => $friend,
			'friend' => 0
		));

		$db->sql_query($sql);

		$sql = 'INSERT INTO feed ' . $db->sql_build_array('INSERT', array(
			'feed_id' => null,
			'feed_type' => 1,
			'user_id' => $self,
			'relation_id' => $friend,
			'feed_text' => '',
			'feed_time' => time(),
			'bbcode_uid'        => 0,
			'bbcode_bitfield'   => 0,
			'bbcode_options'    => 0,
		));

		$db->sql_query($sql);

		global $user;

		$text = utf8_normalize_nfc($user->data['username'] . ' wants to be your friend! Go to [url=http://aiw3.net/friends.php]the friend list[/url] to accept/deny this request.');
		$uid = $bitfield = $options = '';

		$subject = $user->data['username'] . ' wants to be your friend';

	 	generate_text_for_storage($subject, $uid, $bitfield, $options, false, false, false);
		generate_text_for_storage($text, $uid, $bitfield, $options, true, true, true);

		$data = array(
			'address_list'	  => array ('u' => array($friend => 'to')),
			'from_user_id'	  => $user->data['user_id'],
			'from_username'	 => $user->data['username'],
			'icon_id'		   => 0,
			'from_user_ip'	  => $user->data['user_ip'],

			'enable_bbcode'	 => true,
			'enable_smilies'	=> true,
			'enable_urls'	   => true,
			'enable_sig'		=> true,

			'message'		   => $text,
			'bbcode_bitfield'   => $bitfield,
			'bbcode_uid'		=> $uid,
		);

		submit_pm('post', $subject, $data, true);

		return 'The friend has been added sucessfully';
	}

	public function accept()
	{
		global $db, $user;

		$self = intval($user->data['user_id']);
		$friend = intval($this->arguments[0]);

		$sql = 'SELECT * FROM aiw3_xnp.friends WHERE friend_id = ' . $self . ' AND user_id = ' . $friend . ' AND friend = 0';
		$result = $db->sql_query($sql);

		if ($db->sql_fetchrow($result))
		{
			$this->accept_friend($self, $friend);
		}

		header('Location: /friends.php');
		exit;

	}

	public function deny($self, $friend)
	{
		global $db;

		$self = intval($self);
		$friend = intval($friend);

		// remove requests and mutual friendships
		$sql = 'DELETE FROM aiw3_xnp.friends WHERE friend_id = ' . $self . ' AND user_id = ' . $friend;
		$db->sql_query($sql);

		// remove friendships and pending requests
		$sql = 'DELETE FROM aiw3_xnp.friends WHERE user_id = ' . $self . ' AND friend_id = ' . $friend;
		$db->sql_query($sql);

		$sql = 'INSERT INTO feed ' . $db->sql_build_array('INSERT', array(
			'feed_id' => null,
			'feed_type' => 2,
			'user_id' => $self,
			'relation_id' => $friend,
			'feed_text' => '',
			'feed_time' => time(),
			'bbcode_uid'        => 0,
			'bbcode_bitfield'   => 0,
			'bbcode_options'    => 0,
		));

		$db->sql_query($sql);

		header('Location: /friends.php');
		exit;
	}

	public function add()
	{
		global $db, $user;

		if (!$user->data['is_registered'])
		{
			header('Location: /');
			exit;
		}

		$name = request_var('username', '');
		$error = false;

		if (empty($name))
		{
			$error = 'Please enter a username.';
		}
		else
		{
			$sql = 'SELECT user_id FROM phpbb_users WHERE username_clean = \'' . $db->sql_escape(utf8_clean_string($name)) . '\'';

			$result = $db->sql_query($sql);
			$row = $db->sql_fetchrow($result);
			$db->sql_freeresult($result);

			if (!$row)
			{
				$error = 'This user does not exist.';
			}
			else
			{
				$error = $this->add_friend($user->data['user_id'], $row['user_id']);
			}
		}

		/*if (!is_tnet_request())
		{
			header('Location: /friends.php');
			exit;
		}*/

		if (!$error)
		{
			$class = 'alert-success';
			$message = 'The request has been successfully sent.';
		}
		else
		{
			$class = 'alert-error';
			$message = $error;
		}

		$data = ob_start();

?>
<div class="alert <?php echo $class ?>">
  <button type="button" class="close" data-dismiss="alert">&times;</button>
  <?php echo $message ?>
</div>
<?php

		$d = ob_get_contents();
		ob_end_clean();

		return $d;
	}

	private function is_user_online($usrid)
	{
		global $db;
		$var = false;
		$onlinetime = time() - 300;

		$sql = 'SELECT session_user_id FROM ' . SESSIONS_TABLE . ' WHERE session_user_id = ' . $usrid . ' AND session_time > ' . $onlinetime;

		$result = $db->sql_query($sql);

		while ($row = $db->sql_fetchrow($result))
		{
			if ($result->num_rows != 0) {
				$var = true;
			} else {
				$var = false;
			}
		}
		$db->sql_freeresult($result);
		return $var;
	}
}
