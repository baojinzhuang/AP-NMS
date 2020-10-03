<!doctype html>
<html>
<head>
    <meta charset="UTF-8">
    <title>登录系统的后台执行过程</title>
</head>
<body>
<?php
session_start();//登录系统开启一个session内容
$username=$_REQUEST["username"];//获取html中的用户名（通过post请求）
$password=$_REQUEST["password"];//获取html中的密码（通过post请求）

$con=mysqli_connect("localhost","root","xlh123xlh","user_info");//连接mysql 数据库，账户名root ，密码root
if (!$con) {
    die('数据库连接失败'.mysqli_error());
}
mysqli_select_db($con,"user");//use user_info(1)数据库；
$dbusername=null;
$dbpassword=null;
$result=mysqli_query($con,"select * from user where usename ='{$username}';");//查出对应用户名的信息，isdelete表示在数据库已被删除的内容
//user_info为所创建的表
while ($row=mysqli_fetch_array($result,MYSQLI_NUM)) {//while循环将$result中的结果找出来
    $dbusername=$row[1];
    $dbpassword=$row[2];
}
if (is_null($dbusername)) {//用户名在数据库中不存在时跳回index.html界面
    ?>
    <script type="text/javascript">
        alert("用户名不存在");
        window.location.href="index.html";
    </script>
<?php
}
else {
if ($dbpassword!=$password){//当对应密码不对时跳回index.html界面
?>
    <script type="text/javascript">
        alert("密码错误");
        window.location.href="index.html";
    </script>
<?php
}
else {
$_SESSION["username"]=$username;
$_SESSION["code"]=mt_rand(0, 100000);//给session附一个随机值，防止用户直接通过调用界面访问welcome.php
?>
    <script type="text/javascript">
        window.location.href="welcome.php";
    </script>
    <?php
}
}

mysqli_close($con);//关闭数据库连接，如不关闭，下次连接时会出错
?>
</body>
</html>
