<?php
// 处理删除操作的页面 
require "dbconfig.php";
// 连接mysql
$link = mysqli_connect("localhost","root","xlh123xlh","nmsdatabase")  or die("提示：数据库连接失败！".mysqli_error());
// 选择数据库
mysqli_select_db($link,"control");
// 编码设置
mysqli_set_charset($link,'utf8');

$id = $_GET['id'];
//删除指定数据  
mysqli_query($link,"DELETE FROM control WHERE id={$id}") or die('删除数据出错：'.mysqli_error()); 
// 删除完跳转到新闻页
header("Location:control.php");
?>  