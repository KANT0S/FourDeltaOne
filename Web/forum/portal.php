<?php
define('IN_PHPBB', true);
$phpbb_root_path = (defined('PHPBB_ROOT_PATH')) ? PHPBB_ROOT_PATH : './';
$phpEx = substr(strrchr(__FILE__, '.'), 1);
include($phpbb_root_path . 'common.' . $phpEx);
include($phpbb_root_path . 'includes/controller/portal.' . $phpEx);

// Start session management
$user->session_begin();
$auth->acl($user->data);
$user->setup();

page_header('Portal');

$portal = new aiw_controller_portal;
$portal->main();

$template->set_filenames(array(
    'body' => 'portal2_body.html',
));

page_footer();
?>
