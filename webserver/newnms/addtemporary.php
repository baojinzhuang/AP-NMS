<!DOCTYPE html>                                                                                                                                                                  
 <html>
<head lang="en">  
     <meta charset="UTF-8">  
	 <title>添加用户</title>  
</head>
<style type="text/css">
	 form{
        margin: 20px;
     }
</style>
<body>
<form action="action-addtemporary.php" method="post">
    <label>APserisls_ID：</label>             
       <?php

         // 2. 连接mysql
        $link = mysqli_connect("localhost","root","xlh123xlh","nmsdatabase") or die("提示：数据库连接失败！".mysqli_error());
         // 选择数据库
         mysqli_select_db($link,"AP");
         // 编码设置
         mysqli_set_charset($link,'utf8');
 
         // 3. 从DBNAME中查询到news数据库，返回数据库结果集,并按照addtime降序排列  
       $sql = 'select * from AP ';
         // 结果集
         $result = mysqli_query($link,$sql);

          
 
          
 
         ?>
 
          
 

 
               
 
              <?php while($row=mysqli_fetch_array($result)):?>
 
              <input name="s[]" type="checkbox" value="<?=$row['serials_ID']?>"><?=$row['serials_ID']?><br>

            <?php endwhile?>
 
          
        <input type="submit" value="提交"> 
    

</form>
</body>
</html>
