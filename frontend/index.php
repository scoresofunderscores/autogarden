<!DOCTYPE html>
<head>
  <title>autogarden</title>
  <link rel="stylesheet" type="text/css" href="ss.css"/>
  <link rel="shortcut icon" type="image/ico" href="favicon.ico"/>
</head>
<body>
  <center><h1>autogarden<span id="blinky">_</span></h1><br></center>
  <br>
  <div class="pane" id="manual">
    <br>
    <center>
      <a href="on.php"><img src="img/start.svg" alt="Turn on."></img></a>
      <a href="off.php"><img src="img/cancel.svg" alt="Turn off."></img></a>
      <a href="edit.php"><img src="img/edit.svg" alt="Tune."></img></a>
    </center>
    <br>
  </div>
  
  <div class="pane" id="status">
    <!-- Alter this section for your specific messages on the control panel-->
    <br>
    <?php                                                     
      exec('/autogarden/raincheck', $stats);
      $weatherImage = "img/" . $stats[0] . ".png";
      echo "<center><span id='temp'>$stats[1]</span><br></center><br><center>$stats[2]</center><br><center><img id='weather' src='$weatherImage'></img></center>";
    ?>
    <br>
  </div>

  <div class="pane" id="other">
    <center><a id="halp" href="help.html">(need some help?)</a></center>
    <center><p id="version">v 0.14</p></center>
  </div>
</body>

</html>
