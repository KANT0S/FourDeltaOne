<?php
global $phpbb_root_path;
include($phpbb_root_path . 'includes/bbcode.' . $phpEx);
include($phpbb_root_path . 'includes/functions_display.' . $phpEx);

class aiw_controller_portal
{
	public function main()
	{
		global $db, $template, $user;
		global $phpbb_root_path, $phpEx;

		global $cache;

		$aconfig = $this->array_first($this->get_topics('aiw_portal2_config'));

		if ($aconfig == NULL)
		{
			trigger_error('Portal configuration was not found.', E_USER_NOTICE);
		}

		$aconfig = trim($aconfig['raw']);
		$portal_config = $this->parse_configstring($aconfig);

		$template->assign_var('PAGE_TITLE', 'Portal');

		$template->assign_vars(array(
			'INTRODUCTION'    => $this->get_announcment_text($portal_config['staff_forum'], $portal_config['introduction']),
			'FEATURES'        => $this->get_announcment_text($portal_config['staff_forum'], $portal_config['features']),
			'CHAT'            => $this->get_announcment_text($portal_config['staff_forum'], $portal_config['chat']),
			'SUPPORT'         => $this->get_announcment_text($portal_config['staff_forum'], $portal_config['support']),
			'VIDEO_ID'        => '',
		));

		$sql = 'SELECT p.*, t.topic_id, t.forum_id, t.topic_poster, t.topic_first_poster_colour, t.topic_title, t.topic_first_poster_name, t.topic_time FROM ' . POSTS_TABLE . ' p, ' . TOPICS_TABLE . ' t WHERE t.forum_id = ' . $portal_config['announcement_forum'] . ' AND t.topic_id = p.topic_id AND t.topic_first_post_id = p.post_id ORDER BY t.topic_id DESC';

		$result = $db->sql_query_limit($sql, 5);

		while ($row = $db->sql_fetchrow($result))
		{
			$row['bbcode_options'] = (($row['enable_bbcode']) ? OPTION_FLAG_BBCODE : 0) + (($row['enable_smilies']) ? OPTION_FLAG_SMILIES : 0) + (($row['enable_magic_url']) ? OPTION_FLAG_LINKS : 0);
			$template->assign_block_vars('announcements', array(
				'TOPIC_TITLE' => $row['topic_title'],
				'TOPIC_AUTHOR' => get_username_string('full', $row['topic_poster'], $row['topic_first_poster_name'], $row['topic_first_poster_colour']),
				'TOPIC_DATE' => date("Y-m-d H:i:s", $row['topic_time']),
				'TOPIC_TEXT' => generate_text_for_display($row['post_text'], $row['bbcode_uid'], $row['bbcode_bitfield'], $row['bbcode_options']),
				'TOPIC_LINK' => append_sid("{$phpbb_root_path}viewtopic.$phpEx", 'f=' . $row['forum_id'] . '&t=' . $row['topic_id']),
			));
		}

		// latest posts

		$sql = 'SELECT topic_id, forum_id, topic_poster, topic_first_poster_colour, topic_title, topic_first_poster_name, topic_time FROM ' . TOPICS_TABLE . ' WHERE forum_id != 4 AND forum_id != 5 ORDER BY topic_time DESC';

		$result = $db->sql_query_limit($sql, $portal_config['topics']);

		while ($row = $db->sql_fetchrow($result))
		{
			$template->assign_block_vars('latest_topics', array(
				'TITLE' => $row['topic_title'],
				'U_VIEW_TOPIC' => append_sid("{$phpbb_root_path}viewtopic.$phpEx", 'f=' . $row['forum_id'] . '&t=' . $row['topic_id']),
			));
		}

		// hot topics

		$sql = 'SELECT topic_id, forum_id, topic_poster, topic_first_poster_colour, topic_title, topic_first_poster_name, topic_time FROM ' . TOPICS_TABLE . ' WHERE forum_id != 4 AND forum_id != 5 ORDER BY topic_replies DESC, topic_time DESC';

		$result = $db->sql_query_limit($sql, $portal_config['topics']);

		while ($row = $db->sql_fetchrow($result))
		{
			$template->assign_block_vars('latest_hot_topics', array(
				'TITLE' => $row['topic_title'],
				'U_VIEW_TOPIC' => append_sid("{$phpbb_root_path}viewtopic.$phpEx", 'f=' . $row['forum_id'] . '&t=' . $row['topic_id']),
			));
		}

		$mysqli = new mysqli('localhost', 'aiw3', 'NRvFafRdHVcUzBRR', 'aiw3_xnp');

		if (mysqli_connect_errno()) {
			printf("Connect failed: %s\n", mysqli_connect_error());
			exit();
		}

		$keysql = "SELECT user_id FROM aiw3_xnp.iw4_profiles";
		$keyresult = $db->sql_query($keysql);
		$players_all_time = (int) $db->sql_fetchfield('user_id');

		// site stats
		if (($data = $cache->get('_sitestats')) === false) {
			if($data = @file_get_contents('http://cloudrack.io:3000/total'))
			{
				if($data != "")
				{
					$cache->put('_sitestats', $data, 1800);
				}
			}
		}

		$data_decoded = json_decode($data, true);

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

		$template->assign_vars(array(
			'NUM_SERVERS' => $this->get_numstring($servers),
			'NUM_PLAYERSCUR' => $this->get_numstring($players),
			'NUM_PLAYERS' => $this->get_numstring($players_all_time),
			'NUM_PLAYERS24' => $this->get_numstring($data_decoded['players24']),
		));

	}

	private function get_announcment_text($forum_id, $topic_id)
	{
		global $db, $template;

		$sql = 'SELECT topic_id, forum_id, post_text, bbcode_uid, bbcode_bitfield, enable_bbcode, enable_smilies, enable_magic_url FROM ' . POSTS_TABLE . ' WHERE forum_id = ' . $forum_id . ' AND topic_id = ' . $topic_id . '';

		$result = $db->sql_query($sql);

		while ($row = $db->sql_fetchrow($result))
		{
			$row['bbcode_options'] = (($row['enable_bbcode']) ? OPTION_FLAG_BBCODE : 0) + (($row['enable_smilies']) ? OPTION_FLAG_SMILIES : 0) + (($row['enable_magic_url']) ? OPTION_FLAG_LINKS : 0);
			$text = generate_text_for_display($row['post_text'], $row['bbcode_uid'], $row['bbcode_bitfield'], $row['bbcode_options']);
			return $text;
		}

		$db->sql_freeresult($result);
	}

	private function array_first($array)
	{
		foreach ($array as $el)
		{
			return $el;
		}

		return NULL;
	}

	private function get_numstring($num)
	{
		if ($num < 10000)
		{
			return round($num / 1000, 1) . "K";
		}
		else
		{
			return round($num / 1000, 0) . "K";
		}
	}

	private function get_topics($topic_ids)
	{
		global $db;
		$sql = 'SELECT t.topic_id, p.bbcode_uid, p.bbcode_bitfield, p.enable_bbcode, p.enable_smilies, p.enable_magic_url, p.post_text FROM ' . TOPICS_TABLE . ' t ' . 
			   'LEFT JOIN ' . POSTS_TABLE . ' p ON (p.post_id = t.topic_first_post_id) ' .
		   	((is_array($topic_ids)) ? ('WHERE t.topic_id IN (' . implode(', ', $topic_ids) . ')') : ('WHERE t.topic_title = \'' . $db->sql_escape($topic_ids) . '\''));

		$result = $db->sql_query($sql);
		$topics = array();

		while ($row = $db->sql_fetchrow($result))
		{
			$bbcode_options = (($row['enable_bbcode']) ? OPTION_FLAG_BBCODE : 0) +
				(($row['enable_smilies']) ? OPTION_FLAG_SMILIES : 0) +
				(($row['enable_magic_url']) ? OPTION_FLAG_LINKS : 0);


			$topics[$row['topic_id']] = array(
				'raw' => $row['post_text'],
				'formatted' => generate_text_for_display($row['post_text'], $row['bbcode_uid'], $row['bbcode_bitfield'], $bbcode_options)
			);
		}

		$db->sql_freeresult($result);

		return $topics;
	}

	private function parse_configstring($aconfig)
	{
		$aconfig = explode("\n", $aconfig);
		$portal_config = array();
		foreach ($aconfig as $item)
		{
			$item = explode('=', trim($item));
			$portal_config[$item[0]] = $item[1];
		}
		return $portal_config;
	}
}

?>
