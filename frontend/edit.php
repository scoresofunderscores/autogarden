<!DOCTYPE html>
<head>
  <title>Tune your system.</title>
  <link rel="stylesheet" href="ss.css" type="text/css"/>
  <link rel="shortcut icon" type="image/ico" href="favicon.ico"/>
</head>
<body>
  <center id="version">version 0.14 alpha</center>
  <center><h1>alter your autogarden<span id="blinky">_</span></h1></center>
  <div id="top_bar">
    <center><a href="pinConf.php" target="_blank">view pin layout</a></center>
  </div>
  <br>
  <div class="pane">
    <center><img src="img/edit.svg" alt="Settings"/></center>
  </div>
  <div class="pane">
    <center><b id="subcat">schedule new tasks</b></center>
    <form action="schedule.php" method="post" id="schedule">
      <input type="number" name="gpio" min="0" max="64"><b class="inputData">gpio pin number</b></input>
      <br><br><!--Dont judge me.-->
      <input type="time" name="time"><b class="inputData">task start time</b></input>
      <br><br><!--Seriously stop it.-->
      <input type="number" name="pump" min="1" max="60"><b class="inputData">duration (in seconds)</b></input>
      <button type="submit">create task</button>
    </form>
    <br><br><br><br><br><!--is the sound a rotary engine makes-->
    <center><b id="subcat">current tasks</b></center>
    <?php #incoming spaghetti
      $tasks = @fopen("/autogarden/tasks", "r");
      $i = 0;
      if($tasks)
      {
          while(($buf = fgets($tasks,16)) !== false)
          {
              echo "&nbsp;<div class='task'><span id='pin'><b>".strtok($buf,' ')."</b></span> will activate at ";
              echo strtok(' ')." for ".strtok(' ')." seconds. <a href='delete.php?task=$i'><div class='del'>x</div></a></div>";
              $i++;
          }
      }
      if($i === 0)
      {
          if($tasks)
          {
              echo "<center><b id='error'>(no tasks in /autogarden/tasks)</b></center>";
          }
          else
          {
              echo "<center><b id='error'>(could not open /autogarden/tasks)</b></center>";
          }
      }
  ?>
  </div>
</body>
</html>
