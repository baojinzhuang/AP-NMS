<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>修改参数</title>
</head>
<body>
<?php


    $link = mysqli_connect("localhost","root","xlh123xlh","user_info")  or die("提示：数据库连接失败！".mysqli_error());
    mysqli_select_db($link,"user");
    mysqli_set_charset($link,'utf8');
    
    $id = $_GET['id'];
	
    $sql = mysqli_query($link,"SELECT * FROM user WHERE id={$id} ");
    $sql_arr = mysqli_fetch_array($sql); 


?>
<form action="action-editnews.php" method="post">
    <label>ID: </label><input type="text" name="id" value="<?php echo $sql_arr['id']?>">
    <label>用户名：</label><input type="text" name="username" value="<?php echo $sql_arr['usename']?>">
    <label>密码：</label><input type="password" name="password" value="<?php echo $sql_arr['password']?>">
    <label>isdelete：</label><input type="text" name="isdelete" value="<?php echo $sql_arr['isdelete']?>">
    
    <input type="submit" value="提交">
</form>

</body>
</html>