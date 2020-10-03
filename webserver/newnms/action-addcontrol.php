<?php
// 处理增加操作的页面 

// 连接mysql
$link = mysqli_connect("localhost","root","xlh123xlh","nmsdatabase") or die("提示：数据库连接失败！");
// 选择数据库
mysqli_select_db($link,"control");
// 编码设置
mysqli_set_charset($link,'utf8');
// 获取增加的新闻
$id = $_POST['id'];
$object = $_POST['object'];
$number = $_POST['number'];
$operation = $_POST['operation'];
$time = $_POST['time'];
$state = $_POST['state'];
$content = $_POST['content'];
$fh = mysqli_query($link,"select MAX(id) from control");
$c_echo = mysqli_fetch_array($fh);
$t = $c_echo[0];
$sql = mysqli_query($link,"select * from control where id ='{$t}' ");
// 结果集
$row = mysqli_fetch_array($sql);
// var_dump($result);die;

// 解析结果集,$row为新闻所有数据，$newsNum为新闻数
if ($row[5] != 1 && $row[6] == $content)
{
     echo "<script> alert('有命令正在运行，请等上调命令运行完再输入'); </script>";
	 echo "<meta http-equiv='Refresh' content='0;URL=control.php'>";



}
else{
	// 插入数据
	mysqli_query($link,"insert into control(id,object,number,operation,time,state,content) values ('{$id}','{$object}','{$number}','{$operation}','{$time}','{$state}','{$content}')") or die('添加数据出错：'.mysqli_error()); 
	mysqli_close($link);
	header("Location:control.php");
}

?>