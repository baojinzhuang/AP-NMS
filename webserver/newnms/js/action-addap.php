<?php
// 处理增加操作的页面 

// 连接mysql
$link = mysqli_connect("localhost","root","xlh123xlh","nmsdatabase") or die("提示：数据库连接失败！");
// 选择数据库
mysqli_select_db($link,"AP");
mysqli_select_db($link,"control");
mysqli_select_db($link,"config");
mysqli_select_db($link,"temporary");

// 编码设置
mysqli_set_charset($link,'utf8');

// 获取增加的新闻
$serials_ID = $_POST['serials_ID'];
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
$gateway = $_POST['gateway'];
$Apserver_IP = $_POST['Apserver_IP'];
$state = $_POST['state'];
$user_number = $_POST['user_number'];
$mode = $_POST['mode'];
$version = $_POST['version'];
$antenna_number = $_POST['antenna_number'];


$k = date("Y-m-d");
$fh = mysqli_query($link,"select MAX(id) from control");
$c_echo = mysqli_fetch_array($fh);
$t = $c_echo[0];
$b = $t + 1;
if($template_ID == 999){

   $test = mysqli_query($link,"select * from temporary limit 1 ");
    // 结果集
    $tar = mysqli_fetch_array($test);
      
	  if($SSID_name == ""){
		  $a = $tar[0];
	  }
	  elseif($SSID_name != "")
	  {
		  $a = $SSID_name;
	  }
    // 插入数据
    mysqli_query($link,"insert into AP(serials_ID,template_ID,SSID_name,SSID_psw,login_name,login_psw,NATorBride,pool_start,pool_end,dhcporstatic,ip_adress,gateway,Apserver_IP,state,user_number,mode,version,antenna_number) values ('{$serials_ID}','{$template_ID}','{$a}','{$tar[1]}','{$tar[2]}','{$tar[3]}','{$tar[4]}','{$tar[5]}','{$tar[6]}','{$tar[7]}','{$tar[8]}','{$tar[9]}','{$tar[10]}','{$state}','{$user_number}','{$mode}','{$version}','{$antenna_number}')") or die('添加数据出错：'.mysqli_error()); 
    mysqli_query($link,"insert into control(id,object,number,operation,time,state,content) values ('{$b}','1','1','0','{$k}','0','{$serials_ID}')") or die('添加数据出错：'.mysqli_error());  
   mysqli_close($link);
   header("Location:dubiao.php");

}
else{
   $test = mysqli_query($link,"select * from config where template_ID ='{$template_ID}' ");
   // 结果集
   $tar = mysqli_fetch_array($test);
	
   // 插入数据
   mysqli_query($link,"insert into AP(serials_ID,template_ID,SSID_name,SSID_psw,login_name,login_psw,NATorBride,pool_start,pool_end,dhcporstatic,ip_adress,gateway,Apserver_IP,state,user_number,mode,version,antenna_number) values ('{$serials_ID}','{$template_ID}','{$tar[1]}','{$tar[2]}','{$tar[3]}','{$tar[4]}','{$tar[5]}','{$tar[6]}','{$tar[7]}','{$tar[8]}','{$tar[9]}','{$tar[10]}','{$tar[11]}','{$state}','{$user_number}','{$mode}','{$version}','{$antenna_number}')") or die('添加数据出错：'.mysqli_error()); 
   mysqli_query($link,"insert into control(id,object,number,operation,time,state,content) values ('{$b}','1','1','0','{$k}','0','{$serials_ID}')") or die('添加数据出错：'.mysqli_error());  
   mysqli_close($link);
   header("Location:dubiao.php");
}
?>
