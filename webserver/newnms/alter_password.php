<!doctype html>
<html>
<head>
    <meta charset="UTF-8">
    <title>正在修改密码</title>
</head>
<body>
<?php
session_start ();
$username = $_REQUEST ["username"];
$oldpassword = $_REQUEST ["oldpassword"];
$newpassword = $_REQUEST ["newpassword"];

$con=mysqli_connect("localhost","root","xlh123xlh","user_info");
if (! $con) {
    die ( '数据库连接失败' . mysqli_connect_error () );
}
mysqli_select_db ( "user", $con );
$dbusername = null;
$dbpassword = null;
$result = mysqli_query ( $con,"select * from user where  usename ='{$username}';" );
while ( $row = mysqli_fetch_array ( $result,MYSQLI_NUM ) ) {
    $dbusername = $row [1];
    $dbpassword = $row [2];
}
if (is_null ( $dbusername )) {
    ?>
    <script type="text/javascript">
        alert("用户名不存在");
        window.location.href="alter_password.html";
    </script>
    <?php
}
if ($oldpassword != $dbpassword) {
    ?>
    <script type="text/javascript">
        alert("密码错误");
        window.location.href="alter_password.html";
    </script>
    <?php
}

mysqli_query ( $con,"update user set password='{$newpassword}' where usename='{$username}'" ) or die ( "存入数据库失败" . mysqli_connect_error () );//如果上述用户名密码判定不错，则update进数据库中

mysqli_close ( $con );
?>


<script type="text/javascript">
    alert("密码修改成功");
    window.location.href="index.html";
</script>
</body>
</html>


<script type="text/javascript">
    alert("密码修改成功");
    window.location.href="index.html";
</script>
</body>
</html>
