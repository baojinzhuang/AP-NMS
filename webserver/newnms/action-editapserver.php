<?php
// 处理编辑操作的页面 
require "dbconfig.php";
// 连接mysql
$link = mysqli_connect("localhost","root","xlh123xlh","nmsdatabase")  or die("提示：数据库连接失败！".mysqli_error());
// 选择数据库
mysqli_select_db($link,"ap_server");

mysqli_select_db($link,"control");
// 编码设置
mysqli_set_charset($link,'utf8');

// 获取修改的新闻

$id = $_POST['id'];
$Apserver_IP = $_POST['Apserver_IP'];
// 更新数据

$k = date("Y-m-d");
$fh = mysqli_query($link,"select MAX(id) from control");
$c_echo = mysqli_fetch_array($fh);
$t = $c_echo[0];
$b = $t + 1;
$sql = mysqli_query($link,"select * from control where id ='{$t}' ");
// 结果集
$row = mysqli_fetch_array($sql);
if ($row[5] == 0 && $row[6] == $Apserver_IP){
		echo "<script> alert('有命令正在运行，请等上一个命令运行完再输入修改'); </script>";
		echo "<meta http-equiv='Refresh' content='0;URL=apserver.php'>";
}
else{


	mysqli_query($link,"UPDATE ap_server SET id='{$id}', Apserver_IP='{$Apserver_IP}' WHERE id={$id} ") or die('修改数据出错：'.mysqli_error());
	mysqli_query($link,"insert into control(id,object,number,operation,time,state,content) values ('{$b}','2','1','2','{$k}','0','{$Apserver_IP}')") or die('添加数据出错：'.mysqli_error());  
	header("Location:apserver.php");
}
?> 