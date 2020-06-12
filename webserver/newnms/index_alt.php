<?php
$link=mysqli_connect("localhost","root","xlh123xlh");//链接数据库
if($link) {
    //echo"链接数据库成功";
    mysqli_query($link,'SET NAMES UTF8');
    $select = mysqli_select_db($link, "user_info");//选择数据库
    if ($select) {
        //echo"选择数据库成功！";
        $username = $_REQUEST ["name"];
        $new_username = $_REQUEST ["newname"];
        $oldpassword = $_REQUEST ["password"];
        $newpassword = $_REQUEST ["newpassword"];
//        if (isset($username) && isset($oldpassword) && isset($newpassword)) {
//            $str="select * from register where username="."'"."$username"."'";
//            $result = mysqli_query($link, $str);
//        }
    }
}
$str="select * from user where usename="."'"."$username"."'";
$result = mysqli_query($link, $str);
while ( $row = mysqli_fetch_array ( $result,MYSQLI_NUM ) ) {
    $dbusername = $row [1];
    $dbpassword = $row [2];
}
if(! $dbusername )
{
    die('连接失败: ' . mysqli_error($link));
}
//if (is_null ( $dbusername )) {
//    ?>
<!--    <script type="text/javascript">-->
<!--        alert("用户名不存在");-->
<!--        window.location.href="password_modify.html";-->
<!--    </script>-->
<!--    --><?php
//}
if ($oldpassword != $dbpassword) {
    ?>
    <script type="text/javascript">
        alert("密码错误");
        window.location.href = "monitor01.php";
    </script>
    <?php
}
$str="update user set password = '$newpassword' where usename = '$username'";
mysqli_query ( $link,$str );//如果上述用户名密码判定不错，则update进数据库中
$str="update user set usename = '$new_username' where usename = '$username'";
mysqli_query ( $link,$str );
mysqli_close ( $link );
?>
<script type="text/javascript">
    alert("密码修改成功");
    window.location.href="monitor01.php";
</script>
