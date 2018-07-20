<?php
	include("geoip.inc");
	
	$gi = geoip_open("GeoIP.dat", GEOIP_STANDARD);
	
	$country_code = geoip_country_code_by_addr($gi, $_SERVER['REMOTE_ADDR']);
	
	geoip_close($gi);
	
	if($country_code)
	{
		echo $country_code;
	}
	else
	{
		echo 'GB';
	}
?>