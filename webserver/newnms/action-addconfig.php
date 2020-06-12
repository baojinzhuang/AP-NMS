<?php
// 处理增加操作的页面 

// 连接mysql
$link = mysqli_connect("localhost","root","xlh123xlh","nmsdatabase") or die("提示：数据库连接失败！");
// 选择数据库
mysqli_select_db($link,"config");
mysqli_select_db($link,"control");
// 编码设置
mysqli_set_charset($link,'utf8');

// 获取增加的新闻
$template_ID = $_POST['template_ID'];
$SSID_name = $_POST['SSID_name'];
$SSID_psw = $_POST['SSID_psw'];
$login_name = $_POST['login_name'];
$login_psw = $_POST['login_psw'];
$NATorBride = $_POST['NATorBride'];
$pool_start = $_POST['pool_start'];
$pool_end = $_POST['pool_end'];
$dhcporstatic = $_POST['dhcporstatic'];
$ip_adress = $_POST['ip_adress'];
$netmask = $_POST['netmask'];
$gateway = $_POST['gateway'];
$Apserver_IP = $_POST['Apserver_IP'];
// 插入数据

$k = date("Y-m-d");
$fh = mysqli_query($link,"select MAX(id) from control");
$c_echo = mysqli_fetch_array($fh);
$t = $c_echo[0];
$b = $t + 1;
mysqli_query($link,"insert into config(template_ID,SSID_name,SSID_psw,login_name,login_psw,NATorBride,pool_start,pool_end,dhcporstatic,ip_adress,gateway,Apserver_IP,netmask) values ('{$template_ID}','{$SSID_name}','{$SSID_psw}','{$login_name}','{$login_psw}','{$NATorBride}','{$pool_start}','{$pool_end}','{$dhcporstatic}','{$ip_adress}','{$gateway}','{$Apserver_IP}','{$netmask}')") or die('添加数据出错：'.mysqli_error()); 
mysqli_query($link,"insert into control(id,object,number,operation,time,state,content) values ('{$b}','0','1','0','{$k}','0','{$template_ID}')") or die('添加数据出错：'.mysqli_error());
mysqli_close($link);
header("Location:config.php");
?>
