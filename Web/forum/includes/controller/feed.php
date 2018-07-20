<?php
global $phpbb_root_path;
include($phpbb_root_path . 'includes/bbcode.' . $phpEx);
include($phpbb_root_path . 'includes/functions_display.' . $phpEx);

class aiw_controller_feed
{
	public function main($event_id)
	{
		global $db, $template, $user;

		$template->assign_var('PAGE_TITLE', 'Feed');

		switch($event_id) {
			case 0: // status update (your friends posts)
				$sql = 'SELECT f.*, fl.*, u.user_id, u.username, u.user_colour, u.user_avatar, u.user_avatar_type, u.user_avatar_width, u.user_avatar_height FROM feed f, aiw3_xnp.friends fl, phpbb_users u
						WHERE (f.feed_type = 0 AND f.user_id = fl.friend_id AND fl.friend_id != ' . intval($user->data['user_id']) .' AND fl.user_id = ' . intval($user->data['user_id']) . '  AND f.user_id = u.user_id)
						OR (f.feed_type = 0 AND f.user_id = ' . intval($user->data['user_id']) .' AND f.user_id = u.user_id) GROUP BY f.feed_id ORDER BY f.feed_time DESC';
				break;
			case 1: // friend add (we should only see this if we're EXISTING friends with the user that has added someone else)
				$sql = 'SELECT f.*, fl.*, u.user_id, u.username, u.user_colour, u.user_avatar, u.user_avatar_type, u.user_avatar_width, u.user_avatar_height FROM feed f, aiw3_xnp.friends fl, phpbb_users u
						WHERE f.feed_type = 1 AND (f.relation_id = fl.friend_id AND fl.friend_id = ' . intval($user->data['user_id']) . ' AND f.relation_id != ' . intval($user->data['user_id']) . ') ORDER BY f.feed_time DESC';
				break;
			case 2: // friend delete
				$sql = 'SELECT f.*, fl.*, u.user_id, u.username, u.user_colour, u.user_avatar, u.user_avatar_type, u.user_avatar_width, u.user_avatar_height FROM feed f, aiw3_xnp.friends fl, phpbb_users u
						WHERE (f.feed_type = 2 AND f.relation_id = fl.friend_id AND fl.friend_id = ' . intval($user->data['user_id']) . ' AND f.relation_id != ' . intval($user->data['user_id']) . ') ORDER BY f.feed_time DESC';
				break;
		}

		$result = $db->sql_query_limit($sql, 10);

		while ($row = $db->sql_fetchrow($result))
		{
			if ($result->num_rows == 0) {

			} else {
				$template->assign_vars('.feed');
				$template->assign_block_vars('feed', array(
					'FEED_ID'        => $row['feed_id'],
					'FEED_TYPE'      => $row['feed_type'],
					'FEED_USERNAME'  => get_username_string('full', $row['user_id'], $row['username'], $row['user_colour']),
					'FEED_AVATAR'    => ($row['user_avatar']) ? get_user_avatar_location($row['user_avatar'], $row['user_avatar_type'], $row['user_avatar_width'], $row['user_avatar_height']) : '/assets/img/unknown.jpg',
					'FEED_PROFILE'   => get_username_string('profile', $row['user_id'], $row['username']),
					'FEED_RELATION'  => $this->get_fullusername($row['relation_id']),
					'FEED_TEXT'      => generate_text_for_display($row['feed_text'], $row['bbcode_uid'], $row['bbcode_bitfield'], $row['bbcode_options']),
					'FEED_TIME'      => $user->format_date($row['feed_time']),
					'U_DELETE'       => $this->is_users_post(intval($user->data['user_id']), intval($row['feed_id'])) ? '<a class="remove" href="./home.php?m=1&id=' . $row['feed_id'] . '"><span>Remove</span></a>' : '',
					'U_LIKE'         => $this->is_liked(intval($user->data['user_id']), intval($row['feed_id'])) ? '<a class="like" href="./home.php?m=2&id=' . $row['feed_id'] . '"><span>Unlike</span></a>' : '<a class="like" href="./home.php?m=0&id=' . $row['feed_id'] . '"><span>Like</span></a>',
					'LIKES'          => $this->get_likes($row['feed_id']),
				));
			}
		}
	}

	private function get_fullusername($user_id)
	{
		global $db, $user;

		$sql = 'SELECT * FROM phpbb_users WHERE user_id = ' . $user_id;

		$result = $db->sql_query_limit($sql, 1);

		while ($row = $db->sql_fetchrow($result))
		{
			return get_username_string('full', $row['user_id'], $row['username'], $row['user_colour']);
		}
	}

	public function add_event($user_id, $feed_text)
	{
		if ($user->data['user_id'] == ANONYMOUS) {
			header('Location: ./home.php');
			exit();
		}
		global $db;
		if (!empty($feed_text)) {

			$text = $feed_text;
			//$text = utf8_normalize_nfc($feed_text);
			$uid = $bitfield = $options = ''; // will be modified by generate_text_for_storage
			$allow_bbcode = $allow_urls = $allow_smilies = true;
			generate_text_for_storage($text, $uid, $bitfield, $options, $allow_bbcode, $allow_urls, $allow_smilies);

			//$feed_text = strip_tags($feed_text);
			$sql = 'INSERT INTO feed ' . $db->sql_build_array('INSERT', array(
				'feed_id' => null,
				'feed_type' => 0,
				'user_id' => $user_id,
				'relation_id' => 0,
				'feed_text' => $text,
				'feed_time' => time(),
				'bbcode_uid'        => $uid,
				'bbcode_bitfield'   => $bitfield,
				'bbcode_options'    => $options,
			));

			$db->sql_query($sql);
			header('Location: ./home.php');
			exit();
		} else {

		}
	}

	public function delete_event($user_id, $event_id)
	{
		global $db;

		$user_id = intval($user_id);
		$event_id = intval($event_id);

		if (empty($user_id) || empty($event_id)) {
			header('Location: ./home.php');
			exit();
		}

		if ($this->is_users_post($user_id, $event_id)) {

			// remove the actual event
			$sql = 'DELETE FROM feed WHERE user_id = ' . $user_id . ' AND feed_id = ' . $event_id;
			$db->sql_query($sql);

			// remove any existing likes for the event
			$sql = 'DELETE FROM feed_likes WHERE feed_id = ' . $event_id;
			$db->sql_query($sql);
		} else {
			header('Location: ./home.php');
			exit();
		}
	}

	public function like_event($user_id, $event_id)
	{
		global $db, $user;

		if ($user->data['user_id'] == ANONYMOUS) {
			header('Location: ./home.php');
			exit();
		}
		if (!empty($event_id)) {
			$sql = 'SELECT * FROM feed_likes WHERE user_id = ' . $user_id . ' AND feed_id = ' . $event_id;
			$result = $db->sql_query($sql);

			if ($result->num_rows == 0) {
				$sql = 'INSERT INTO feed_likes ' . $db->sql_build_array('INSERT', array(
					'like_id' => null,
					'user_id' => $user_id,
					'feed_id' => $event_id,
					'like_time' => time()
				));

				$db->sql_query($sql);
				header('Location: ./home.php');
				exit();
			} else {
				header('Location: ./home.php');
				exit();
			}
		} else {
			echo "invalid data";
		}
	}

	public function unlike_event($user_id, $event_id)
	{
		global $db, $user;

		if (!empty($event_id)) {
			$sql = 'DELETE FROM feed_likes WHERE user_id = ' . $user_id . ' AND feed_id = ' . $event_id;
			$result = $db->sql_query($sql);

			header('Location: ./home.php');
			exit();
		}
	}

	private function get_likes($event_id)
	{
		global $db, $user;
		$count = 0;
		$output = ''; // :))))))
		$event_id = intval($event_id);
		$sql = 'SELECT f.*, fl.*, u.user_id, u.username, u.user_colour FROM feed f, feed_likes fl, phpbb_users u
				WHERE f.feed_id = ' . $event_id . ' AND fl.feed_id = ' . intval($event_id) . ' AND fl.user_id = u.user_id ORDER BY like_time DESC';

		$result = $db->sql_query($sql);

		while ($row = $db->sql_fetchrow($result)) {
			// Example result: Flashback, Avail and 3 others like this update.

			if ($count < 2) {
				if ($count == 0) {
					$output = get_username_string('full', $row['user_id'], $row['username'], $row['user_colour']);
				}
				else {
					if ($result->num_rows > 2)
						$output = $output . ", " . get_username_string('full', $row['user_id'], $row['username'], $row['user_colour']);
					else
						$output = $output . " and " . get_username_string('full', $row['user_id'], $row['username'], $row['user_colour']);

				}
			}
			$count++;
		}

		if ($count > 2) {
			$output = $output . ' and ' . ($count - 2) . ' others';
		}
		if ($count != 0){
			$output = $output . ' liked this update.';
		}
		return $output;
	}

	private function is_users_post($user_id, $event_id)
	{
		global $db, $user;

		$sql = 'SELECT * FROM feed WHERE user_id = ' . $user_id . ' AND feed_id = ' . $event_id;
		$result = $db->sql_query($sql);

		while ($row = $db->sql_fetchrow($result)) {

			if ($result->num_rows == 0) {
				return false;
			} else {
				return true;
			}
		}
	}

	private function is_liked($user_id, $event_id)
	{
		global $db, $user;

		$sql = 'SELECT * FROM feed_likes WHERE user_id = ' . $user_id . ' AND feed_id = ' . $event_id;
		$result = $db->sql_query($sql);

		while ($row = $db->sql_fetchrow($result)) {
			if ($result->num_rows == 0) {
				return false;
			} else {
				return true;
			}
		}
	}
}

?>

