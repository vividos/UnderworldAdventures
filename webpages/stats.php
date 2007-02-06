<h2>home page statistics</h2>

<p>
Here are some statistics from the Underworld Adventures home page.
</p>

<?php

   $today = strtotime(date("Y-m-d"));

   $detailed = false;
   if (0!=strlen($details))
   {
      rawurldecode($details);
      echo "<p>detailed statistics for " .
         "<a href=\"$details\" target=\"_blank\">$details</a></p>\n";
      $detailed = true;
   }

   if (!$detailed && $show != "rating")
   {
      echo '<p>ratings-based statistics are available <a href="http://uwadv.sourceforge.net/index.php?page=stats&show=rating">here</a>.';
   }

   $aliases = array(
      "http://www.google.com/" => array("www.google.de", "www.google.co.uk",
         "www.google.fr", "www.google.nl", "www.google.com.br",
         "www.google.at", "www.google.ca", "google.co.jp",
         "www.google.co.kr", "www.google.co.il", "www.google.be",
         "www.google.it", "www.google.ch", "www.google.com.pl",
         "www.google.co.nz", "www.google.com.ar", "www.google.ie",
         "www.google.cl", "www.google.com.ru", "google.yahoo.com"),
      "http://sourceforge.net/projects/uwadv/" => array("sourceforge.net/news",
         "sourceforge.net/mail", "sourceforge.net/project",
         "sourceforge.net/cvs", "sourceforge.net/tracker",
         "sourceforge.net/forum"),
      "http://www.freshmeat.net/" => array("freshmeat.net/projects/uwadv",
         "freshmeat.net/lounge", "freshmeat.net/browse",
         "freshmeat.net/admin", "freshmeat.net/search",
         "freshmeat.net/releases"),
      "http://directory.google.com/Top/Games/Video_Games/Roleplaying/U/Ultima_Series/" => array(
         "Games/Video_Games/Roleplaying/Ultima_Series"
      ),
      "http://www.stygianabyss.com/" => array("http://stygianabyss.com/"),
      "http://www.linuxgaming.co.uk/" => array("http://linuxgaming.co.uk"),
      "http://www.classicgaming.com/" => array("http://classicgaming.com/"),
      "http://www.happypenguin.org/" => array("happypenguin.com",
         "http://happypenguin.org/"),
   );

/* generate stats arrays from referer file ---------------- */

   $daycount = 0;
   $dayhitcount = 0;
   $maxdaycount = 0;
   $maxday = "";

   $fd = fopen("/home/groups/u/uw/uwadv/htdocs/referer.txt","rt");
   while(!feof($fd))
   {
      $line = trim(fgets($fd,256));

      if (eregi("^date:",$line)==1)
      {
         // we have a new day
         if ($dayhitcount > $maxdaycount)
         {
            $maxdaycount = $dayhitcount;
            $maxday = substr($line,5);
         }

         $dayhitcount = 0;
         $daycount++;

         $curparsedate = strtotime(substr($line,5));

         continue;
      }

      if (strlen($line)==0) continue;

      $dayhitcount++;

      // special treatment for forum urls
      if (eregi("showthread.php",$line)==1 && eregi("\?s\=",$line)==1)
      {
         $pos = strpos($line,"?s=");
	 $line = substr_replace($line,"?",$pos,
            strpos($line,"&",$pos+3)-$pos+1);
      }

      $line2 = explode("?",$line);
      $line2 = explode("#", $line2[0]);
      if ($detailed)
      {
         // check for passing that url is later, in alias check
         $addstr = $line;
      }
      else
      {
         $addstr = $line2[0];
         $strpos = strrchr($addstr,'/');
         $pos = strlen($addstr)-strlen($strpos);

         if (!($strpos===false) && $pos>7)
            $addstr = substr($addstr,0,$pos+1);
         if ($addstr[strlen($addstr)-1] != "/")
            $addstr .= "/";
      }

      // do alias replacement
      reset($aliases);
      $passed_details = false;
      while(list($alias_url,$aliases_array) = each($aliases))
      {
         if ($detailed && !stristr($alias_url,$details))
            continue; // don't check that alias

         reset($aliases_array);
         while(list($dummy,$alias_replace) = each($aliases_array))
         {
            if (stristr($addstr,$alias_replace))
            if ($detailed)
            {
               $passed_details=true;
               break;
            }
            else
            {
               $addstr = $alias_url;
               break;
            }
         }
      }

      // check if we have a no-alias url and it passes
      if ($detailed && !$passed_details)
      {
         if (stristr($addstr,$details)) ; else
         continue;
      }

      $referer[$addstr]++;

      $distdate = ($today-$curparsedate)/(3600*24);
      if ($distdate<30)
      {
         $age = $distdate/30;

         // y=(1 - (x^3 + 3*x^2*(1-x)))^3
         $rate = 1-($age*$age*$age + 3*$age*$age*(1-$age));
         $rate = $rate*$rate*$rate;

         $rating[$addstr] += $rate;
      }
   }

/* output referer stats ----------------------------------- */

   $count = 0;
   while (list($key, $val) = each($referer)) $count += $val;
   reset($referer);

   if ($show == "rating")
   {
      $ratecount = 0;
      while (list($key, $val) = each($rating)) $ratecount += $val;
      reset($rating);

      // divide rating by counts
      while (list($key, $val) = each($rating))
         $rating[$key] *= $referer[$key]/$ratecount;

      reset($rating);

      $referer = $rating;
   }

   arsort($referer);

   $count = 0;
   while (list($key, $val) = each($referer)) $count += $val;
   reset($referer);

   echo "<p>\n";
   echo " <table border=\"1\" cellspacing=\"0\" cellpadding=\"3\">\n";

   if ($show == "rating")
      echo "  <tr><td><b>referer</b></td><td><b>rating<b></td></tr>\n";
   else
      echo "  <tr><td><b>referer</b></td><td><b>count</b></td><td><b>percent</b></td></tr>\n";

   $percentsum = 0.0;
   $countsum = 0;
   $omitcount = 0;

   while (list($key, $val) = each($referer))
   {
      if ($show != "all" && $show != "rating" && $val/$count < 0.01)
      {
         echo "  <tr><td>omitted <a href=\"index.php?page=stats&show=all" .
            ($detailed ? "&details=$details" : "") .
            "\">" .
            "[show all]</a></td><td>" . ($count - $countsum) .
            "</td><td>" . sprintf("%2.1f",(1.0 - $percentsum)*100.0) .
            "</td></tr>\n";
         break;
      }

      if ($detailed)
         echo "  <tr><td><a href=\"" . $key . "\" target=\"_blank\">";
      else
         echo "  <tr><td><a href=\"index.php?page=stats&details=" . $key .
            ($show == "rating"? "&show=rating" : "") .
            "\">";

      echo substr($key,0,50) . (strlen($key)>=50 ? "[...]" : "");
      echo "</a></td>";

      if ($show != "rating")
         echo "<td>$val</td>";
      else
         echo "<td>" . sprintf("%2.1f",$val) . "</td>";

      if ($show != "rating")
         echo "<td>" . sprintf("%2.1f",$val*100.0/$count) . "</td>";

      echo "</tr>\n";

      $countsum += $val;
      $percentsum += $val/$count;
   }

   if ($show != "rating")
      echo "  <tr><td>total</td><td>$count</td><td>100.0</td></tr>\n";

   echo " </table>\n";
   echo "</p>\n\n";

   if (!$detailed)
   {
      echo "<p>average visits per day: " .
        sprintf("%2.1f",$count/$daycount) . "<br>\n";
      echo "day with most visits: $maxday. visits: $maxdaycount</p>";
   }

/* -------------------------------------------------------- */
?>
