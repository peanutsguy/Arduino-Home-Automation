<?php

$url = 'http://192.168.1.177';
$data = array( 
			array(
			'sensor' => 'gps',
			'time' => 1351824120,
			'data' => array(
					48.756080,
					2.302038
				)
			)
		);

$options = array(
    'http' => array(
        'method'  => 'POST',
        'content' => json_encode($data),
        'header'=>  "Content-Type: application/json\r\n" .
                    "Accept: application/json\r\n"
      )
);
 
$context     = stream_context_create($options);
$result      = file_get_contents($url, false, $context);

print_r($result);

?>
