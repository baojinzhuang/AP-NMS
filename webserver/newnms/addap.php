<!DOCTYPE html>  
<html>  
<head lang="en">  
    <meta charset="UTF-8">  
    <title>添加用户</title>  
</head>
    <link rel="stylesheet" href="https://ajax.aspnetcdn.com/ajax/bootstrap/4.2.1/css/bootstrap.min.css">
    <link rel="stylesheet" href="css/style.default.css" id="theme-stylesheet">
<style type="text/css">
    form{
        margin: 20px;
    }
</style>


<body>
<form action="action-addap.php" method="post">  
    <label>serials_ID：</label><input type="text" name="serials_ID" > 
	<label>template_ID：</label>             
		<?php

		// 2. 连接mysql
		$link = mysqli_connect("localhost","root","xlh123xlh","nmsdatabase") or die("提示：数据库连接失败！".mysqli_error());
		// 选择数据库
		mysqli_select_db($link,"config");
		// 编码设置
		mysqli_set_charset($link,'utf8');

		// 3. 从DBNAME中查询到news数据库，返回数据库结果集,并按照addtime降序排列  
		$sql = 'select * from config ';
		// 结果集
		$result = mysqli_query($link,$sql);

		 

		 

		?>

		 

		<select name="template_ID">

           <option value=999>999</option>		 

			<?php while($row=mysqli_fetch_array($result)):?>

			<option value="<?=$row['template_ID']?>"><?=$row['template_ID']?></option>

			<?php endwhile?>

		 

		</select>
		<label>SSID_name：</label><input type="text" name="SSID_name" >
		<label>SSID_psw：</label><input type="text" name="SSID_psw" >
		<label>login_name: </label><input type="text" name="login_name" >
		<label>login_psw：</label><input type="text" name="login_psw" >
		<label>NATorBride：</label><input type="text" name="NATorBride" >
		<label>pool_start：</label><input type="text" name="pool_start">
		<label>pool_end: </label><input type="text" name="pool_end" >
		<label>dhcporstatic：</label><input type="text" name="dhcporstatic" >
		<label>ip_adress：</label><input type="text" name="ip_adress">
                <label>netmask：</label><input type="text" name="netmask" >
		<label>gateway：</label><input type="text" name="gateway" >
		<label>Apserver_IP：</label><input type="text" name="Apserver_IP">
		<input type="submit" value="提交"> 


</form> 
    <script src="js/jquery.min.js"></script>
    <script src="js/bootstrap.min.js"></script>
    <script src="vendor/jquery-validation/jquery.validate.min.js"></script><!--表单验证-->
    <!-- Main File-->
    <script src="js/front.js"></script> 
</body>  
</html>
