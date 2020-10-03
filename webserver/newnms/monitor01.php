<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>NMS后台管理系统</title>
	<link rel="stylesheet" href="css/Quttons.css">
	<link rel="stylesheet" href="css/main.css">
</head>
<style type="text/css">
.wrapper {width: 1000px;margin: 20px auto;}
h2 {text-align: center;}
.add {margin-bottom: 20px;}
.add a {text-decoration: none;color: #fff;background-color: green;padding: 6px;border-radius: 5px;}
td {text-align: center;}
#customers {
    font-family: "Trebuchet MS", Arial, Helvetica, sans-serif;
    width: 100%;
    border-collapse: collapse;
}

#customers td,
#customers th {
    font-size: 1em;
    border: 1px solid #98bf21;
    padding: 3px 7px 2px 7px;
}

#customers th {
    font-size: 1.1em;
    text-align: left;
    padding-top: 5px;
    padding-bottom: 4px;
    background-color: #A7C942;
    color: #ffffff;
}



</style>
	<style type="text/css">
		


		.smart-green {
			margin-left:auto;
			margin-right:auto;
			max-width: 500px;
			background: #F8F8F8;
			padding: 30px 30px 20px 30px;
			font: 12px Arial, Helvetica, sans-serif;
			color: #666;
			border-radius: 5px;
			-webkit-border-radius: 5px;
			-moz-border-radius: 5px;
		}
		.smart-green h1 {
			font: 24px "Trebuchet MS", Arial, Helvetica, sans-serif;
			padding: 20px 0px 20px 40px;
			display: block;
			margin: -30px -30px 10px -30px;
			color: #FFF;
			background: #9DC45F;
			text-shadow: 1px 1px 1px #949494;
			border-radius: 5px 5px 0px 0px;
			-webkit-border-radius: 5px 5px 0px 0px;
			-moz-border-radius: 5px 5px 0px 0px;
			border-bottom:1px solid #89AF4C;

		}
		.smart-green h1>span {
			display: block;
			font-size: 11px;
			color: #FFF;
		}

		.smart-green label {
			display: block;
			margin: 0px 0px 5px;
		}
		.smart-green label>span {
			float: left;
			margin-top: 10px;
			color: #5E5E5E;
		}
		.smart-green input[type="text"], .smart-green input[type="password"], .smart-green textarea, .smart-green select {
			color: #555;
			height: 30px;
			line-height:15px;
			width: 100%;
			padding: 0px 0px 0px 10px;
			margin-top: 2px;
			border: 1px solid #E5E5E5;
			background: #FBFBFB;
			outline: 0;
			-webkit-box-shadow: inset 1px 1px 2px rgba(238, 238, 238, 0.2);
			box-shadow: inset 1px 1px 2px rgba(238, 238, 238, 0.2);
			font: normal 14px/14px Arial, Helvetica, sans-serif;
		}
		.smart-green textarea{
			height:100px;
			padding-top: 10px;
		}

		.smart-green .button {
			background-color: #9DC45F;
			border-radius: 5px;
			-webkit-border-radius: 5px;
			-moz-border-border-radius: 5px;
			border: none;
			padding: 10px 25px 10px 25px;
			color: #FFF;
			text-shadow: 1px 1px 1px #949494;
		}
		.smart-green .button:hover {
			background-color:#80A24A;
		}
	</style>
<body>
    <div class="wrapper">
        <h2>NMS后台管理系统</h2>
        <div class="add">
            <a href="addnews.html">增加参数</a>
        </div>
        <table id="customers">
            <tr>
                <th>ID</th>
                <th>用户名</th>
                <th>密码</th>
                <th>isdelete</th>
                <th>操作</th>
            </tr>

            <?php
                // 1.导入配置文件
               
                // 2. 连接mysql
                $link = mysqli_connect("localhost","root","xlh123xlh","user_info") or die("提示：数据库连接失败！".mysqli_error());
                // 选择数据库
                mysqli_select_db($link,"user");
                // 编码设置
                mysqli_set_charset($link,'utf8');

                // 3. 从DBNAME中查询到news数据库，返回数据库结果集,并按照addtime降序排列  
                $sql = 'select * from user ';
                // 结果集
                $result = mysqli_query($link,$sql);
                // var_dump($result);die;

                // 解析结果集,$row为新闻所有数据，$newsNum为新闻数目
                while($row = mysqli_fetch_array($result)){
					echo "<tr>";
					echo "<td>".$row["id"]."</td><td>".$row["usename"]."</td><td>".$row["password"]."</td><td>".$row["isdelete"]."</td>";
                    echo "<td>
                            <a href='javascript:del({$row['id']})'>删除</a>
                            <a href='javascript:edi({$row['id']})'>修改</a>
                          </td>";
                    echo "</tr>";
                }
                // 5. 释放结果集
                mysqli_free_result($result);
                mysqli_close($link);
            ?>
        </table>
    </div>
    
    <script type="text/javascript">
        function del (id) {
            if (confirm("确定删除这个用户吗？")){
                window.location = "action-del.php?id="+id;
            }
        }
		function edi(id){
			if (confirm("确定修改这个用户吗？")){
				window.location = "editnews.php?id="+id;
			}
		}
    </script>

</body>
</html>