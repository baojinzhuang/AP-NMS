<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>修改参数</title>
</head>
<body>
<?php


    $link = mysqli_connect("localhost","root","xlh123xlh","nmsdatabase")  or die("提示：数据库连接失败！".mysqli_error());
    mysqli_select_db($link,"AP");
    mysqli_set_charset($link,'utf8');
    $serials_ID = $_GET['serials_ID'];
	
    $sql = mysqli_query($link,"SELECT * FROM AP WHERE serials_ID={$serials_ID} ");
    $sql_arr = mysqli_fetch_array($sql); 

 
?>
<form action="action-editap.php" method="post">
    <label>serials_ID: </label><input type="text" name="serials_ID" value="<?php echo $sql_arr['serials_ID']?>">
    <label>template_ID：</label><input type="text" name="template_ID" value="<?php echo $sql_arr['template_ID']?>">
    <label>SSID_name：</label><input type="text" name="SSID_name" value="<?php echo $sql_arr['SSID_name']?>">
    <label>SSID_psw：</label><input type="text" name="SSID_psw" value="<?php echo $sql_arr['SSID_psw']?>">
    <label>login_name: </label><input type="text" name="login_name" value="<?php echo $sql_arr['login_name']?>">
    <label>login_psw：</label><input type="text" name="login_psw" value="<?php echo $sql_arr['login_psw']?>">
    <label>NATorBride：</label><input type="text" name="NATorBride" value="<?php echo $sql_arr['NATorBride']?>">
    <label>pool_start：</label><input type="text" name="pool_start" value="<?php echo $sql_arr['pool_start']?>">
	<label>pool_end: </label><input type="text" name="pool_end" value="<?php echo $sql_arr['pool_end']?>">
    <label>dhcporstatic：</label><input type="text" name="dhcporstatic" value="<?php echo $sql_arr['dhcporstatic']?>">
    <label>ip_adress：</label><input type="text" name="ip_adress" value="<?php echo $sql_arr['ip_adress']?>">
    <label>gateway：</label><input type="text" name="gateway" value="<?php echo $sql_arr['gateway']?>">
	<label>Apserver_IP：</label><input type="text" name="Apserver_IP" value="<?php echo $sql_arr['Apserver_IP']?>">
    <label>state：</label><input type="text" name="state" value="<?php echo $sql_arr['state']?>">
	<label>user_number: </label><input type="text" name="user_number" value="<?php echo $sql_arr['user_number']?>">
    <label>mode：</label><input type="text" name="mode" value="<?php echo $sql_arr['mode']?>">
    <label>version：</label><input type="text" name="version" value="<?php echo $sql_arr['version']?>">
    <label>antenna_number：</label><input type="text" name="antenna_number" value="<?php echo $sql_arr['antenna_number']?>">
    <input type="submit" value="提交">
</form>

</body>
</html>