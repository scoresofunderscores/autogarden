<!DOCTYPE html>
<head>
  <title>MANUAL SHUTOFF ENGAGED</title>
  <link rel="stylesheet" href="ss.css" type="text/css"/>
</head>
<body>
<?php
    exec('/autogarden/d off');
    $pins = @fopen("/autogarden/pins", "r");
    if($pins)
    {
        while(($buf = fgets($pins,2)) !== false)
        {
            //add on screen error display for user after testing
            $job = "gpio mode $buf out && gpio write $buf 0";
            exec($job);
        }
        echo "<div class='pane'><center><b>TURNED ALL DEVICES OFF</b></center><center><a href='..'> click if the page doesn't redirect</a></center></div>";
        echo "<script>setTimeout(function(){window.history.back();},1200);</script>";
    }
    else
    {
        for($i = 0; $i < 6; $i++)
        {
            $job = "gpio mode $i out && gpio write $i 0";
            exec($job);
        }
        echo "<div class='pane'><center><img src='error.svg'></img><b>Could not find /autogarden/pins file! Toggled default pins: 0,1,2,3,4,5. Unless your relays are on those pins it will not work. Make a file at /autogarden/pins thats readable by the user www-data (or apache, depends on your distro), and put in your pin numbers there, one per line, to fix this issue.</b></center></div>";
        echo"<br><br><div class='pane'><center><a href='..'> click here to go back</a></center></div>";
    }
?>
</body>
</html>
