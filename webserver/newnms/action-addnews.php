<?php
// 处理增加操作的页面 

// 连接mysql
$link = mysqli_connect("localhost","root","xlh123xlh","user_info") or die("提示：数据库连接失败！");
// 选择数据库
mysqli_select_db($link,"user");
// 编码设置
mysqli_set_charset($link,'utf8');

// 获取增加的新闻
$id = $_POST['id'];
$username = $_POST['username'];
$password = $_POST['password'];
$isdelete = $_POST['isdelete'];
// 插入数据
mysqli_query($link,"insert into user(id,usename,password,isdelete) values ('{$id}','{$username}','{$password}','{$isdelete}')") or die('添加数据出错：'.mysqli_error()); 
mysqli_close($link);
header("Location:monitor01.php");
?>