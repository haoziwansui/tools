<?php

// Monitor configuration

return array(

     // Default scan path is `script root`
	 'root'   => '/var/www/html/phpmyadmin/',

     // allowed multi root
     // 'root' => ['/first', '/second', ...]

     //'root' => array(
     //   __DIR__ . '\\1',
     //   __DIR__ . '\\2'
     //),

     // skip this dirs
     //'ignore_dirs' => [
     // '/home/decoder/',
	//'/home/ubuntu/',
	//'/home/xctf/'
     //],

     // ServerTag for text reports, default _SERVER[SERVER_NAME]
	 // 'server' => 'server_name',

     // files pattern
	 'files' => '(\.php.?|\.htaccess|\.txt |\.py |\.sh)$',

      // write logs to ./logs/Ym/d-m-y.log
     'log' => true,

      // notify administrator email
	 'mail' => array(
	 	'from'   => '123@123.ru',
	 	'to'   	 => '123@gmail.com',

	 	// disabled by default
	 	'enable' => false
	 )

);
