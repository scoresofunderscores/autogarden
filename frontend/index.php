<!DOCTYPE html>
<head>
  <title>autogarden</title>
  <link rel="stylesheet" type="text/css" href="ss.css"/>
  <link rel="stylesheet" type="text/css" href="weather-icons.css"/>
  <link rel="shortcut icon" type="image/ico" href="favicon.ico"/>
</head>
<body>
  <center><h1>autogarden<span id="blinky">_</span></h1><br></center>
  <br/>
  <div class="pane" id="manual">
    <br/>
    <center>
      <a href="on.php"><img src="img/start.svg" alt="Turn on."></img></a>
      <a href="off.php"><img src="img/cancel.svg" alt="Turn off."></img></a>
      <a href="edit.php"><img src="img/edit.svg" alt="Tune."></img></a>
    </center>
    <br/>
  </div>
  
  <div class="pane" id="status">
    <!-- Alter this section for your specific messages on the control panel-->
    <br/>
    <?php                                                     
      $weatherStatus = array(
          "Windy" => "<center><b class='wi wi-day-windy'></b>It's gonna be windy.",
          "Thunderstorm" => "<center><b class='wi wi-day-thunderstorm'></b>Thunderstorms expected.",
          "Mist" => "<center><b class='wi wi-day-haze'></b>Ooooh. Misty.", 
          "Rain" => "<center><b class='wi wi-day-rain'></b>Rain showers expected.", 
          "Clear" => "<center><b class='wi wi-day-sunny'></b>Clear skies.",
          "Clouds" => "<center><b class='wi wi-day-cloudy'></b>Pretty cloudy outside.",
          "RainDrizzle" => "<center><b class='wi wi-day-sprinkle'></b>Drizzling...",
          "RainMist" => "<center><b class='wi wi-day-haze'></b>Mist with rain",
          "RainMistDrizzle" => "<center><b class='wi wi-day-rain'></b>Drizzle with slight mist.",
          "RainThunderstormMist" => "<center><b class='wi wi-day-thunderstorm'></b>Thunderstorms with additional mist.",
          "RainMistThunderstorm" => "<center><b class='wi wi-day-thunderstorm'></b>Thunderstorms with additional mist.");
      $stats = array();
      exec('/autogarden/raincheck', $stats);
      echo "<center><br><span id='temp'>$stats[0]</span></center>";
      if($weatherStatus[$stats[1]] === NULL)
      {
          if(strpos($stats[1], "Rain") === false)
          {
              echo "<center><b>?</b> $stats[1]";
          }
          else
          {
              echo "<center><b class='wi wi-day-rain'></b> $stats[1]";
          }
      }
      else
      {
          echo $weatherStatus[$stats[1]];
      }
      echo "</center>";
    ?>
    <br/>
  </div>

  <div class="pane" id="other">
    <center><a id="halp" href="help.html">(need some help?)</a></center>
  </div>
</body>

</html>
