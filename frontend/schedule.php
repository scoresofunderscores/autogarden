<!DOCTYPE html>
<head>
  <title>Adding task...</title>
</head>
<body>
  <?php
    function return_text()
    {
        echo "<script> window.location.href='edit.php'; </script>";
        echo "<a href='edit.php'>Click here if you have JS disabled and want to go back.</a>";
    }
        
    if(empty($_POST['gpio']))
    {
        echo "The light inside is broken, but I still work.";
        return_text();
    }
    $task = "/autogarden/schedule add " . $_POST['gpio'] . " " . $_POST['time'] . " " . $_POST['pump'];
    exec($task);
    return_text();
    ?>
</body>
</html>
