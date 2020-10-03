<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>修改参数</title>
</head>
<body>
<?php


    $link = mysqli_connect("localhost","root","xlh123xlh","nmsdatabase")  or die("提示：数据库连接失败！".mysqli_error());
    mysqli_select_db($link,"ap_server");
    mysqli_set_charset($link,'utf8');
    $id = $_GET['id'];
	
    $sql = mysqli_query($link,"SELECT * FROM ap_server WHERE id={$id}");
    $sql_arr = mysqli_fetch_array($sql); 

 
?>
<form action="action-editapserver.php" method="post">
    <label>序号：</label><input type="text" name="id" value="<?php echo $sql_arr['id']?>">
	<label>Apserver_IP：</label><input type="text" name="Apserver_IP" value="<?php echo $sql_arr['Apserver_IP']?>">
    <input type="submit" value="提交">
</form>

</body>
</html>