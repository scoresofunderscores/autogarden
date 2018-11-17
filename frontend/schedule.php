<!DOCTYPE html>
<head>
  <title>Adding task...</title>
</head>
<body>
  <?php
    if(empty($_POST['gpio']))
    {
        echo "The light inside is broken, but I still work.";
        echo "<script> window.location.href='edit.php'; </script>";
        echo "<a href='edit.php'>Click here if you have JS disabled and want to go back.</a>";
    }
    $task = "/autogarden/schedule add " . $_POST['gpio'] . " " . $_POST['time'] . " " . $_POST['pump'];
    exec($task);
    echo "<script> window.location.href='edit.php'; </script>";
    echo "<a href='edit.php'>Click here if you have JS disabled and want to go back.</a>";
    ?>
</body>
</html>
