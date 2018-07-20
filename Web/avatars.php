<?php
define('IN_PHPBB', true);
$phpbb_root_path = '../';
$phpEx = substr(strrchr(__FILE__, '.'), 1);
include($phpbb_root_path . 'common.' . $phpEx);
include_once($phpbb_root_path . 'includes/functions_display.' . $phpEx);

error_reporting(E_ALL);
$x = 128;
$y = 128;
$resized = imagecreatetruecolor($x,$y);

function LoadImage($ext, $imgname)
{
    $im = false;

    /* Attempt to open */
    if($ext == "png") {
        $im = imagecreatefrompng($imgname);
    }
    if($ext == "jpg") {
        $im = imagecreatefromjpeg($imgname);
    }
    if($ext == "gif") {
        $im = imagecreatefromgif($imgname);
    }

    /* See if it failed */
    if(!$im)
    {
        /* Create a black image
        $im  = imagecreatetruecolor(300, 30);
        $bgc = imagecolorallocate($im, 255, 255, 255);
        $tc  = imagecolorallocate($im, 0, 0, 0);

	die($imgname);

        imagefilledrectangle($im, 0, 0, 300, 30, $bgc);

        /* Output an error message 
        imagestring($im, 1, 5, 5, 'Error loading ' . $imgname, $tc); */
	$im = imagecreatefrompng("noavatar.png");
    }

    return $im;
}

$user_id = intval($_GET['u']);

$sql = "SELECT user_avatar, user_avatar_type, user_avatar_width, user_avatar_height FROM phpbb_users WHERE user_id = " . $user_id . "";

$result = $db->sql_query_limit($sql, 1);

while ($row = $db->sql_fetchrow($result))
{
        $filename = $row['user_avatar'];

        $ext = substr(strrchr($filename, '.'), 1);
        $stamp = (int) substr(stristr($filename, '_'), 1);
        $filename = (int) $filename;

        $file = $filename . '.' . $ext;

	global $config, $phpbb_root_path;
	$prefix = $config['avatar_salt'] . '_';
	$image_dir = $config['avatar_path'];
	
	$file_path = $phpbb_root_path . $image_dir . '/' . $prefix . $file;

        //$image = get_user_avatar_location($row['user_avatar'], $row['user_avatar_type'], $row['user_avatar_width'], $row['user_avatar_height']);

        $img = LoadImage($ext, $file_path);

	header('Content-Type: image/png');

//	imagealphablending($img, false);
//	imagesavealpha($img, true);

	imagecopyresampled($resized, $img, 0, 0, 0, 0, $x, $y, imagesx($img), imagesy($img));

        imagepng($resized);
        imagedestroy($img);

}

?>