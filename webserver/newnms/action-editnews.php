<?php
// 处理编辑操作的页面 
require "dbconfig.php";
// 连接mysql
$link = mysqli_connect("localhost","root","xlh123xlh","user_info")  or die("提示：数据库连接失败！".mysqli_error());
// 选择数据库
mysqli_select_db($link,"user");
// 编码设置
mysqli_set_charset($link,'utf8');

// 获取修改的新闻
$id = $_POST['id'];
$username = $_POST['username'];
$password = $_POST['password'];
$isdelete = $_POST['isdelete'];
// 更新数据
mysqli_query($link,"UPDATE user SET id='{$id}',usename='{$username}',password='{$password}',isdelete='{$isdelete}' WHERE id={$id} ") or die('修改数据出错：'.mysqli_error()); 
header("Location:monitor01.php");
?> 