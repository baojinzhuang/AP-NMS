<!doctype html>
<html>
<head>
    <meta charset="UTF-8">
    <title>注册用户</title>
</head>
<body>
<?php
session_start();
$username=$_REQUEST["username"];
$password=$_REQUEST["password"];

$con=mysqli_connect("localhost","root","xlh123xlh","user_info");
if (! $con) {
    die ( '数据库连接失败' . mysqli_connect_error () );
}
mysqli_select_db ( $con , "user");
$dbusername = null;
$dbpassword = null;
$result = mysqli_query ( $con,"select * from user where  usename ='{$username}';" );
while ( $row = mysqli_fetch_array ( $result,MYSQLI_NUM ) ) {
    $dbusername=$row[1];
    $dbpassword=$row[2];
}
if($username == $dbusername ){

    ?>
    <script type="text/javascript">
        alert("用户已存在");
        window.location.href="register.html";
    </script>

    <?php
    exit;
}

mysqli_query($con,"insert into user (id,usename,password,isdelete) values('4','{$username}','{$password}','0')") or die("存入数据库失败".mysqli_error()) ;

mysqli_close($con);
?>
<script type="text/javascript">
    alert("注册成功");
    window.location.href="index.html";
</script>





</body>
</html>
