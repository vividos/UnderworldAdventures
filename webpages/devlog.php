<?php
/* uwadv.sourceforge.net devlog php script -------------------------------- */

   $type = $_GET['type'];

   // send proper content type when devlog.php is accessed directly
   if (strpos($HTTP_SERVER_VARS['SCRIPT_NAME'], "devlog.php"))
   {
      if ($type == 'rss')
         header("Content-Type: application/xml\n\n");
      else
         header("Location: /index.php?page=devlog\n\n");
   }

   if ($type != 'rss')
   {
?>
<h2>uwadv dev log</h2>

<p>
The dev log is the place where I write about the daily ongoings of the project. It is updated occasionally.
An RSS feed is also available: <a href="devlog.php?type=rss"><img src="rss_xml.gif"/></a>
</p>

<?php
   }

/* filenames -------------------------------------------------------------- */

   $xmlfile = "devlog.xml";
   $xslfile = "devlog.xsl";
   $xslfile_rss = "devlog-rss.xsl";

/* xsl-transform devlog.xml to desired format ----------------------------- */
/*
   // once sourceforge.net updates to a more current version of PHP with the
   // domxml extensions, we can use this:

   if (!$xmldoc = domxml_open_file($xmlfile))
      echo "<p>The dev log cannot be found.</p>";
   else
   {
      if ($type == 'rss')
         $xsldoc = domxml_xslt_stylesheet_file($xslfile_rss);
      else
         $xsldoc = domxml_xslt_stylesheet_file($xslfile);
      $result = $xsldoc->process($xmldoc);
      print $xsldoc->result_dump_mem($result);
   }
*/
/* expat xml parsing and manual transform --------------------------------- */
   // this version uses the expat parser. how ugly:

   function cdata_handler($expat, $data)
   {
      echo htmlspecialchars($data);
   }

   function html_start_elem_handler($expat, $name, $attr)
   {
      if (0 == strcasecmp($name, "devlog")) echo ""; else
      if (0 == strcasecmp($name, "entry")) echo "<span class=\"devlogHeading\">" . $attr['date'] ."</span>"; else
      if (0 == strcasecmp($name, "para")) echo "<p>"; else
      {
         echo "<" . $name;
         while (list($key, $val) = each ($attr))
            echo " $key=\"$val\"";
         echo ">\n";
      }
   }

   function html_end_elem_handler($expat, $name)
   {
      if (0 == strcasecmp($name, "devlog")) echo ""; else
      if (0 == strcasecmp($name, "entry")) echo "</span>"; else
      if (0 == strcasecmp($name, "para")) echo "</p>"; else
      	echo "</" . $name . ">";
   }

   function rss_start_elem_handler($expat, $name, $attr)
   {
      if (0 == strcasecmp($name, "devlog"))
      {
         echo "<rss version=\"2.0\" xmlns:blogChannel=\"http://backend.userland.com/blogChannelModule\">\n" .
              "<channel>\n" .
              "<title>Underworld Adventures - Developer Log</title>\n" .
              "<link>http://uwadv.sourceforge.net/index.php?page=devlog</link>\n" .
              "<description>Developer's Log for the Underworld Adventures project.</description>\n" .
              "<language>en-us</language>\n" .
              "<copyright>Copyright 2002-2004 Michael Fink</copyright>\n" .
              "<webMaster>vividos@users.sourceforge.net</webMaster>";
      }
      else
      if (0 == strcasecmp($name, "entry"))
      {
         // convert to RFC-822 date
         $date = $attr['date'];
         $date_rfc = date("r", strtotime($date));

         echo "<item>\n" .
              "  <pubDate>" . $date_rfc . "</pubDate>\n" .
              "  <guid isPermaLink=\"false\">date-" . $date ."</guid>\n" .
              "  <description>";
      }
      else
      if (0 == strcasecmp($name, "para"))
      {
         echo "&lt;p&gt;";
      }
      else
      {
         echo "&lt;" . $name;
         while (list($key, $val) = each ($attr))
            echo " $key=\"" . htmlentities($val) . "\"";
         echo "&gt;\n";
      }
   }

   function rss_end_elem_handler($expat, $name)
   {
      if (0 == strcasecmp($name, "devlog"))
      {
         echo "</channel>\n" .
              "</rss>\n";
      }
      else
      if (0 == strcasecmp($name, "entry"))
      {
         echo "  </description>" .
              " </item>\n";
      }
      else
      if (0 == strcasecmp($name, "para"))
      {
         echo "&lt;/p&gt;";
      }
      else
      	echo "&lt;/" . $name . "&gt;";
   }

   $expat = xml_parser_create();
   xml_parser_set_option($expat, XML_OPTION_CASE_FOLDING, FALSE);

   //$xml = file_get_contents($xmlfile); // only works with php > 4.3.0
   $xml = implode('', file($xmlfile));

   xml_set_character_data_handler($expat, cdata_handler);

   if ($type == 'rss')
      xml_set_element_handler($expat, rss_start_elem_handler, rss_end_elem_handler);
   else
      xml_set_element_handler($expat, html_start_elem_handler, html_end_elem_handler);

   xml_parse($expat, $xml, TRUE);

   xml_parser_free($expat);

/* ------------------------------------------------------------------------ */
?>
