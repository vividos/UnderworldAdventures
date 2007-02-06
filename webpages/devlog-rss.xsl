<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
<!-- XSLT stylesheet to process devlog.xml to a RSS format file -->
<!-- (C) 2004 Michael Fink                                      -->

<xsl:output method="xml"/>

<!-- transform all "entry" nodes under "devlog", the root node -->
<xsl:template match="/devlog">
 <rss version="2.0" xmlns:blogChannel="http://backend.userland.com/blogChannelModule">
  <channel>
   <title>Underworld Adventures - Developer Log</title>
   <link>http://uwadv.sourceforge.net/index.php?page=devlog</link>
   <description>Developer's Log for the Underworld Adventures project.</description>
   <language>en-us</language>
   <copyright>Copyright 2002-2004 Michael Fink</copyright>
   <webMaster>vividos@users.sourceforge.net</webMaster>
<xsl:apply-templates select="entry"/>
  </channel>
 </rss>
</xsl:template>

<!-- process all entries, output date and all paragraph bodies -->
<xsl:template match="entry">
  <item>
   <pubDate><xsl:value-of select="@date"/></pubDate>
   <description>
<xsl:for-each select="para">
 <xsl:value-of select="text()"/>
</xsl:for-each>
   </description>
   <guid isPermaLink="false">date-<xsl:value-of select="@date"/></guid>
  </item>
</xsl:template>

</xsl:stylesheet>
