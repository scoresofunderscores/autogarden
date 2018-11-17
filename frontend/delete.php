<!DOCTYPE html>
<head>
  <title>Removing task...</title>
</head>
<body>
  <?php
    $task = "/autogarden/schedule remove ".$_GET['task'];
    exec($task);
    echo "<script> window.location.href='edit.php'; </script>";
    echo "<a href='edit.php'>Click here if you have JS disabled and want to go back.</a>";
  ?>
</body>
</html>
