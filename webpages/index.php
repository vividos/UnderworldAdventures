<?php
/* uwadv.sourceforge.net php script --------------------------------------- */

/* check if we are on uwadv.sourceforge.net ------------------------------- */

//   if ($SERVER_NAME != "uwadv.sourceforge.net" &&
//       $SERVER_NAME != "uwadv.sf.net" )
//      header("Location: http://uwadv.sourceforge.net\n\n");

/* function to insert mail address ---------------------------------------- */

function insert_mail()
{
   echo '<a href="mailto:@users.sourceforge.net?subject=[uwadv]">';
   echo '<img src="mail.png" style="vertical-align:middle" border="0" alt="vividos\' mail address"/>';
   echo '</a>';
}

/* insert template head --------------------------------------------------- */

   include "template.inc.php";

   template_head();
   template_body_menu();

/* insert page content ---------------------------------------------------- */

   $page = $_GET['page'];

   $whichphp="news.php";

   if ($page=="news") $whichphp = "news.php";
   else if ($page=="archive") $whichphp = "archive.php";
   else if ($page=="screenshots") $whichphp = "screenshots.php";
   else if ($page=="download") $whichphp = "download.php";
   else if ($page=="oldreleases") $whichphp = "oldreleases.php";
   else if ($page=="aboutus") $whichphp = "aboutus.php";
   else if ($page=="links") $whichphp = "links.php";
   else if ($page=="dev") $whichphp = "dev.php";
   else if ($page=="devlog") $whichphp = "devlog.php";
   else if ($page=="irc") $whichphp = "irc.php";
   else if ($page=="image") $whichphp = "image.php";
   else if ($page=="stats") $whichphp = "stats.php";

   require($whichphp);

/* insert template bottom ------------------------------------------------- */

   template_bottom($whichphp);

/* log referer from outside pages ----------------------------------------- */

   if (strlen($HTTP_REFERER)>0 &&
       !eregi("uwadv.sourceforge.net",$HTTP_REFERER) &&
       eregi("^http://",$HTTP_REFERER))
   {
      $fd = @fopen("/home/groups/u/uw/uwadv/htdocs/referer.txt","at");
      @fputs($fd, $HTTP_REFERER . "\n");
      @fclose($fd);
   }

/* ------------------------------------------------------------------------ */
?>
