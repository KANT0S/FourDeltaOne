<?php

	$MySQL_Host = "localhost";
	$MySQL_User = "xnp";
	$MySQL_Pass = "";
	$MySQL_DB = "";

	function ParsePost( )
	{
		$username = '';
		$password = '';
		
		$post = file_get_contents( "php://input" );
		
		$post = str_replace( "&", " ", $post );
		
		sscanf( $post, "%s  %s", $username, $password );
		
		return array( 'user' => $username,
					  'pass' => $password
					);
	}
	
	function mysql_fetch_full_result_array( $result )
	{
		$table_result = array();
		$r = 0;
		
		if( $result === true )
		{
			return $result;
		}
		
		if( mysql_num_rows( $result ) == 0 )
		{
			return $result;
		}
		
		while( $row = mysql_fetch_assoc( $result ) )
		{
			$arr_row = array();
			$c = 0;
			
			while ( $c < mysql_num_fields( $result ) )
			{       
				$col = mysql_fetch_field( $result, $c );   
				$arr_row[ $col -> name ] = $row[ $col -> name ];           
				$c++;
			}   
			
			$table_result[ $r ] = $arr_row;
			$r++;
		}   
		
		return $table_result;
	}
	
	class DWAuth
	{
		var $keys;
	
		function AddDWValue( $val )
		{
			$this->keys[] = $val;
		}
		
		function GetAuthString( )
		{
			$result = "";
			
			foreach( $this->keys as $c )
			{
				$result .= $c."#";
			}
			
			return $result;
		}
	}
	
	class DB
	{
		var $connection;
		var $started;
		
		function start()
		{
			global $MySQL_Host, $MySQL_User, $MySQL_Pass, $MySQL_DB;
			
			$this->connection = mysql_connect( $MySQL_Host, $MySQL_User, $MySQL_Pass );
			mysql_select_db( $MySQL_DB, $this->connection );
		}
		
		function query( $query )
		{
			$result = mysql_query( $query, $this->connection );
			
			if( $result )
			{
				return mysql_fetch_full_result_array( $result );
			}
			else
			{
				return $result;
			}
		}
		
		function end()
		{
			mysql_close( $this->connection );
		}
		
		function isStarted()
		{
			return $started;
		}
	}
	
	class Login
	{
		
		function CheckLogin( $username, $password )
		{
			$db = new DB();
			$db->start();
			
			$query = "SELECT id, password, email FROM users WHERE username='".$username."' AND password='".sha1( $password )."';";
			
			$result = $db->query( $query );
			
			$db->end();
			
			if( $result == false )
				return false;
//				fwrite($fh, $result);
//				fclose($fh);
				
				
			if( sha1( $password ) == $result[ 0 ][ 'password' ] )
			{
				return array( 'id' => $result[ 0 ][ 'id' ],
							  'mail' => $result[ 0 ][ 'email' ],
							  'user' => $username
							);
			}
		}
	}
	
	$packet = new DWAuth();
	$result = ParsePost();
	
	if( ( empty( $result[ 'user' ] ) ) || ( empty( $result[ 'pass' ] ) ) )
	{
		$packet->AddDWValue( "fail" );
		$packet->AddDWValue( "Username and/or password is empty." );
		$packet->AddDWValue( 1 );
		$packet->AddDWValue( "Anonymous" );
		$packet->AddDWValue( "anonymous@example.com" );
		$packet->AddDWValue( 0 );
		
		echo $packet->GetAuthString();
		
		die();
	}
	
	$login = new Login();
	$result = $login->CheckLogin( $result[ 'user' ], $result[ 'pass' ] );
	
	if( $result == false )
	{
		$packet->AddDWValue( "fail" );
		$packet->AddDWValue( "incorrect username and/or password." );
		$packet->AddDWValue( 1 );
		$packet->AddDWValue( "Anonymous" );
		$packet->AddDWValue( "anonymous@example.com" );
		$packet->AddDWValue( 0 );
	}
	else
	{
		$sessionID = md5( rand() );
		// How to make the return
		$packet->AddDWValue( "ok" ); // fail or ok
		$packet->AddDWValue( "Success." ); // Success or error
		$packet->AddDWValue( $result[ 'id' ] ); // UserID
		$packet->AddDWValue( $result[ 'user' ] ); // Username
		$packet->AddDWValue( $result[ 'mail' ] ); // email
		$packet->AddDWValue( $sessionID ); // sessionID
		
		$db = new DB();
		$db->start();
		
		$query = "UPDATE users SET sid='".$sessionID."' WHERE id=".$result[ 'id' ];
		
		$result = $db->query( $query );
		$db->end();
	}
	
	echo $packet->GetAuthString();
	
?>