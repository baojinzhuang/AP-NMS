<?php
// 处理增加操作的页面 

// 连接mysql
$link = mysqli_connect("localhost","root","xlh123xlh","nmsdatabase") or die("提示：数据库连接失败！");
// 选择数据库
mysqli_select_db($link,"control");
mysqli_select_db($link,"AP"); 
// 编码设置
mysqli_set_charset($link,'utf8');

// 获取增加的新闻
$a=$_POST["s"];
$str = implode(",",$a);
$k = date("Y-m-d");
$fh = mysqli_query($link,"select MAX(id) from control");
$c_echo = mysqli_fetch_array($fh);
$t = $c_echo[0];
$b = $t + 1;
// 插入数据
mysqli_query($link,"insert into control(id,object,number,operation,time,state,content) values ('{$b}','3','0','0','{$k}','0','{$str}')") or die('添加数据出错：'.mysqli_error());  
mysqli_close($link);
header("Location:temporary.php");
?>
