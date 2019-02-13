<?php
/* uwadv.sourceforge.net site template ------------------------------------ */

function template_head()
{
/* ------------------------------------------------------------------------ */
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
<head>
 <title>Underworld Adventures Home Page</title>
 <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
 <meta name="author" content="Michael Fink"/>
 <meta name="keywords" content="ultima underworld adventures uw1 hack project"/>
 <link rel="icon" href="favicon.ico" type="image/x-icon"/>
 <style type="text/css">

  body { color:black; background-color:white; }
  body,div,p,td { font-family:Tahoma,sans-serif; font-size:11pt; color:black; }

  h1,h2,h3 { font-family:Tahoma,sans-serif; font-weight:bold; }

  h1 { font-size:24pt; text-align:center; }
  h2 { font-size:20pt; text-align:center; }
  h3 { font-size:16pt; }

  a:link    { color:#0000af; }
  a:visited { color:#8080ff; }
  a:active  { color:#cf0000; }

  td.menu { background-color:#cfcfcf; }

  .newsHeading { font-size:14pt; font-weight:bold; }
  .newsBody { font-size:11pt; margin-left:32px; }

  .devlogHeading { margin-left:20px; font-weight:bold; padding-top:10px; }
  .devlogBody { font-size:11pt; }

 </style>
</head>
<?php
/* ------------------------------------------------------------------------ */
}

function template_body_menu()
{
/* ------------------------------------------------------------------------ */
?>
<body>

<table border="0" width="80%" cellspacing="3" cellpadding="2" align="center">
<tr><td>
<!-- title -->

 <table width="100%" border="0" bgcolor="black" cellspacing="0" cellpadding="1" align="center">
 <tr><td>
  <table width="100%" border="0" bgcolor="white" cellspacing="0" cellpadding="0" align="center">
  <tr><td bgcolor="#7f7f7f">
   <img src="top1.jpg" align="left" alt="Underworld Adventures logo part 1"/>
   <img src="top2.jpg" align="right" alt="Underworld Adventures logo part 2"/>
  </td></tr></table>
 </td></tr></table>
<!-- title end -->

<!-- redirect note -->
 <table width="100%" border="0" bgcolor="black" cellspacing="0" cellpadding="1" align="center" style="margin-top: 6px; margin-bottom: 2px">
 <tr><td>
  <table width="100%" border="0" bgcolor="white" cellspacing="0" cellpadding="0" align="center">
  <tr><td bgcolor="#ffea00" height="40" style="padding:6px">
    The new Underworld Adventures project homepage can be found here:<br/>
	<b style="text-si"><a href="https://vividos.github.io/UnderworldAdventures/">https://vividos.github.io/UnderworldAdventures/</a></b>
  </td></tr></table>
 </td></tr></table>
<!-- redirect note end -->

</td></tr>

<tr valign="top"><td>
<!-- menu -->


 <table width="100%" border="0" bgcolor="black" cellspacing="0" cellpadding="1" align="center">
 <tr><td>
  <table width="100%" border="0" bgcolor="white" cellspacing="0" cellpadding="5" align="center">
  <tr valign="top"><td class="menu" width="120">
   <p>user menu<br/>
    * <a href="index.php?page=news">news</a><br/>
    * <a href="index.php?page=screenshots">screenshots</a><br/>
    * <a href="index.php?page=download">download</a><br/>
    * <a href="index.php?page=aboutus">about us</a><br/>
    * <a href="index.php?page=links">links</a>
   </p>

   <p>dev menu<br/>
    * <a href="index.php?page=dev">development</a><br/>
    * <a href="index.php?page=devlog">dev log</a><br/>
    * <a href="http://lists.sourceforge.net/lists/listinfo/uwadv-devel">mailing list</a><br/>
    * <a href="http://sourceforge.net/tracker/?group_id=50987&amp;atid=461662">bug tracker</a><br/>
    * <a href="http://cvs.sourceforge.net/cgi-bin/viewcvs.cgi/uwadv/uwadv/" target="_blank">view cvs</a><br/>
    * <a href="index.php?page=irc">#uwadv channel</a>
   </p>

   <p>
    <a href="http://sourceforge.net/projects/uwadv/" target="_blank">SourceForge project home page</a><br/><br/>

    <a href="http://sourceforge.net">
     <img src="http://sourceforge.net/sflogo.php?group_id=50987" width="88" height="31" border="0" alt="[SF Logo]"/>
    </a>
   </p>

   <p>
    <a href="http://validator.w3.org/check/referer">
     <img src="valid-xhtml10.png" alt="Valid XHTML 1.0!" height="31" width="88" border="0"/>
    </a>
   </p>

   <table><tr><td><img src="clear.gif" width="120" height="1" alt="."/></td></tr></table>


<!-- menu end -->
</td><td>
<!-- content -->


  <table width="100%" border="0" bgcolor="white" cellspacing="0" cellpadding="5" align="center">
  <tr><td>

<?php
/* ------------------------------------------------------------------------ */
}

function template_bottom($whichphp)
{
/* ------------------------------------------------------------------------ */
?>

   <table><tr><td><img src="clear.gif" width="600" height="1" alt="."/></td></tr></table>

  </td></tr></table>

<!-- content end -->
</td></tr></table>
</td></tr></table>
</td></tr>

<tr><td>
<!-- bottom -->


 <table width="100%" border="0" bgcolor="black" cellspacing="0" cellpadding="1" align="center">
 <tr><td>
  <table width="100%" border="0" bgcolor="white" cellspacing="0" cellpadding="5" align="center">
  <tr><td class="menu">

   <p align="center" style="margin:0px;">
    mail:<img src="mailgray.png" style="vertical-align:middle" border="0" alt="vividos' mail address"/> -
    last updated:
    <?php
/* output "last modified" date -------------------------------------------- */

   $modified = stat($whichphp);
   echo date(" Y-m-d H:i:s ",$modified[9]);

/* ------------------------------------------------------------------------ */
    ?>
   </p>

  </td></tr></table>
 </td></tr></table>


<!-- bottom end -->
</td></tr>

</table>

</body>
</html>
<?php
}

/* ------------------------------------------------------------------------ */
?>
