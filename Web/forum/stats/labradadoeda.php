<?php

ini_set('display_errors', 1);
error_reporting(E_ALL);

mysql_connect('localhost', 'aiw3', 'NRvFafRdHVcUzBRR') or die(mysql_error());
mysql_select_db('aiw3') or die(mysql_error());
$result = mysql_query("SELECT * FROM stats ORDER BY time DESC LIMIT 2000");

$count = 0;
$realcount = 29;
while ($row = mysql_fetch_array($result)) {
    $realcount++;
    if ($realcount == 30) {
        $count++;
        $servers[] = $row['servers'];
        $players[] = $row['players'];
	$time[] = $row['time'] * 1000;
        $realcount = 0;
    }
}

$newcount = 0;
$content = "var servers = [";

while ($newcount < $count) {
    if ($newcount == ($count - 1)) {
        $content .= "[" . $time[$newcount] . ", " . $servers[$newcount] . "]";
    } else {
        $content .= "[" . $time[$newcount] . ", " . $servers[$newcount] . "], ";
    }
    $newcount++;
}
$content .= "], ";
$newcount = 0;
$content .= "players = [";
while($newcount < $count)
{
if($newcount == ($count - 1))
{
$content .= "[".$time[$newcount].", ".$players[$newcount]."]";
}
else
{
$content .= "[".$time[$newcount].", ".$players[$newcount]."], ";
}
$newcount++;
}
$content .= "];";

$myFile = "/var/www/aiw3.net/stats/jscode.txt";
$fh = fopen($myFile, 'w') or die("can't open file");
fwrite($fh, $content);
fclose($fh);
echo $content;
?>
