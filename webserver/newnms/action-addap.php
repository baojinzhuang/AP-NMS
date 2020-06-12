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
$netmask = $_POST['netmask'];
$Apserver_IP = $_POST['Apserver_IP'];

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
		  $fist = $tar[0];
	  }
	  elseif($SSID_name != "")
	  {
		  $fist = $SSID_name;
	  }
	  if($SSID_psw == ""){
		  $fist1 = $tar[1];
	  }
	  elseif($SSID_psw != "")
	  {
		  $fist1 = $SSID_psw;
	  }
	  if($login_name == ""){
		  $fist2 = $tar[2];
	  }
	  elseif($login_name != "")
	  {
		  $fist2 = $login_name;
	  }
	  if($login_psw == ""){
		  $fist3 = $tar[3];
	  }
	  elseif($login_psw != "")
	  {
		  $fist3 = $login_psw;
	  }
	  if($NATorBride == ""){
		  $fist4 = $tar[4];
	  }
	  elseif($NATorBride != "")
	  {
		  $fist4 = $NATorBride;
	  }
	  if($pool_start == ""){
		  $fist5 = $tar[5];
	  }
	  elseif($pool_start != "")
	  {
		  $fist5 = $pool_start;
	  }
	  if($pool_end == ""){
		  $fist6 = $tar[6];
	  }
	  elseif($pool_end != "")
	  {
		  $fist6 = $pool_end;
	  }
	  if($dhcporstatic == ""){
		  $fist7 = $tar[7];
	  }
	  elseif($dhcporstatic != "")
	  {
		  $fist7 = $dhcporstatic;
	  }
	  if($ip_adress == ""){
		  $fist8 = $tar[8];
	  }
	  elseif($ip_adress != "")
	  {
		  $fist8 = $ip_adress;
	  }
	  if($gateway == ""){
		  $fist9 = $tar[9];
	  }
	  elseif($gateway != "")
	  {
		  $fist9 = $gateway;
	  }
	  if($Apserver_IP == ""){
		  $fistend = $tar[10];
	  }
	  elseif($Apserver_IP != "")
	  {
		  $fistend = $Apserver_IP;
	  }
	  if($netmask == ""){
                  $end = $tar[11];
          }
          elseif($netmask != "")
          {
                  $end = $netmask;
          }


    // 插入数据
    mysqli_query($link,"insert into AP(serials_ID,template_ID,SSID_name,SSID_psw,login_name,login_psw,NATorBride,pool_start,pool_end,dhcporstatic,ip_adress,gateway,Apserver_IP,state,user_number,mode,version,antenna_number,netmask) values ('{$serials_ID}','{$template_ID}','{$fist}','{$fist1}','{$fist2}','{$fist3}','{$fist4}','{$fist5}','{$fist6}','{$fist7}','{$fist8}','{$fist9}','{$fistend}','0','0','0','0','0','{$end}')") or die('添加数据出错：'.mysqli_error()); 
    mysqli_query($link,"insert into control(id,object,number,operation,time,state,content) values ('{$b}','1','1','0','{$k}','0','{$serials_ID}')") or die('添加数据出错：'.mysqli_error());  
   mysqli_close($link);
   header("Location:dubiao.php");

}
else{
   $test = mysqli_query($link,"select * from config where template_ID ='{$template_ID}' ");
   // 结果集
   $tar = mysqli_fetch_array($test);
	
  	  if($SSID_name == ""){
		  $fist = $tar[1];
	  }
	  elseif($SSID_name != "")
	  {
		  $fist = $SSID_name;
	  }
	  if($SSID_psw == ""){
		  $fist1 = $tar[2];
	  }
	  elseif($SSID_psw != "")
	  {
		  $fist1 = $SSID_psw;
	  }
	  if($login_name == ""){
		  $fist2 = $tar[3];
	  }
	  elseif($login_name != "")
	  {
		  $fist2 = $login_name;
	  }
	  if($login_psw == ""){
		  $fist3 = $tar[4];
	  }
	  elseif($login_psw != "")
	  {
		  $fist3 = $login_psw;
	  }
	  if($NATorBride == ""){
		  $fist4 = $tar[5];
	  }
	  elseif($NATorBride != "")
	  {
		  $fist4 = $NATorBride;
	  }
	  if($pool_start == ""){
		  $fist5 = $tar[6];
	  }
	  elseif($pool_start != "")
	  {
		  $fist5 = $pool_start;
	  }
	  if($pool_end == ""){
		  $fist6 = $tar[7];
	  }
	  elseif($pool_end != "")
	  {
		  $fist6 = $pool_end;
	  }
	  if($dhcporstatic == ""){
		  $fist7 = $tar[8];
	  }
	  elseif($dhcporstatic != "")
	  {
		  $fist7 = $dhcporstatic;
	  }
	  if($ip_adress == ""){
		  $fist8 = $tar[9];
	  }
	  elseif($ip_adress != "")
	  {
		  $fist8 = $ip_adress;
	  }
	  if($gateway == ""){
		  $fist9 = $tar[10];
	  }
	  elseif($gateway != "")
	  {
		  $fist9 = $gateway;
	  }
	  if($Apserver_IP == ""){
		  $fistend = $tar[11];
	  }
	  elseif($Apserver_IP != "")
	  {
		  $fistend = $Apserver_IP;
	  }
          if($netmask == ""){
                  $end = $tar[12];
          }
          elseif($netmask != "")
          {
                  $end = $netmask;
          }

    // 插入数据
    // 插入数据
   mysqli_query($link,"insert into AP(serials_ID,template_ID,SSID_name,SSID_psw,login_name,login_psw,NATorBride,pool_start,pool_end,dhcporstatic,ip_adress,gateway,Apserver_IP,state,user_number,mode,version,antenna_number,netmask) values ('{$serials_ID}','{$template_ID}','{$fist}','{$fist1}','{$fist2}','{$fist3}','{$fist4}','{$fist5}','{$fist6}','{$fist7}','{$fist8}','{$fist9}','{$fistend}','0','0','0','0','0','{$end}')") or die('添加数据出错：'.mysqli_error()); 
   mysqli_query($link,"insert into control(id,object,number,operation,time,state,content) values ('{$b}','1','1','0','{$k}','0','{$serials_ID}')") or die('添加数据出错：'.mysqli_error());  
   mysqli_close($link);
   header("Location:dubiao.php");
}
?>
