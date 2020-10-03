<?php
// 处理编辑操作的页面 
require "dbconfig.php";
// 连接mysql
$link = mysqli_connect("localhost","root","xlh123xlh","nmsdatabase")  or die("提示：数据库连接失败！".mysqli_error());
// 选择数据库
mysqli_select_db($link,"temporary");
// 编码设置
mysqli_set_charset($link,'utf8');

// 获取修改的新闻
$SSID_name = $_POST['SSID_name'];
$SSID_psw = $_POST['SSID_psw'];
$login_name = $_POST['login_name'];
$login_psw = $_POST['login_psw'];
$NATorBride = $_POST['NATorBride'];
$pool_start = $_POST['pool_start'];
$pool_end = $_POST['pool_end'];
$dhcporstatic = $_POST['dhcporstatic'];
$ip_adress = $_POST['ip_adress'];
$gateway = $_POST['gateway'];
$Apserver_IP = $_POST['Apserver_IP'];
// 更新数据
mysqli_query($link,"UPDATE temporary SET SSID_name='{$SSID_name}',SSID_psw='{$SSID_psw}',login_name='{$login_name}',login_psw='{$login_psw}',NATorBride='{$NATorBride}',pool_start='{$pool_start}',pool_end='{$pool_end}',dhcporstatic='{$dhcporstatic}',ip_adress='{$ip_adress}',gateway='{$gateway}',Apserver_IP='{$Apserver_IP}' WHERE SSID_name={$SSID_name} ") or die('修改数据出错：'.mysqli_error()); 
header("Location:temporary.php");
?> 