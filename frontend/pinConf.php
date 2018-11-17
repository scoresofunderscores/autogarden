<!DOCTYPE html>
<head>
  <title>Pin Layout</title>
  <style>
    html,body
    {
	background-color:#000;
	color:#FFF;
	font-size:1.2em;
    }

    pre
    {
	color:#00bf00;
    }
  </style>
</head>
<body>
  <h4>To get this information on the command line run <em>gpio readall</em>.</h4>
  <?php
    $pinConf = array();
    exec('gpio readall', $pinConf);
    foreach($pinConf as $line)
    {
        echo "<pre>$line</pre>";
    }
  ?>
</body>
</html>
