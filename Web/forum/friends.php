<?php
define('IN_PHPBB', true);
$phpbb_root_path = './';
$phpEx = substr(strrchr(__FILE__, '.'), 1);
include($phpbb_root_path . 'common.' . $phpEx);
include_once($phpbb_root_path . 'includes/functions_display.' . $phpEx);
include_once($phpbb_root_path . 'includes/controller/friends.' . $phpEx);

// Start session management
$user->session_begin();
$auth->acl($user->data);
$user->setup();

$friends = new aiw_controller_friends;
$friends->main();

page_header('Friends');

$template->set_filenames(array(
    'body' => 'home_friends.html',
));

page_footer();
?>
