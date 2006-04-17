<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
<!-- XSLT stylesheet to view news from news.php as rss 2.0 feed -->
<!-- (C) 2006 Michael Fink                                      -->

<xsl:output method="xml"/>

<xsl:template match="/div">
<rss version="2.0">
 <channel>
  <title>Underworld Adventures News</title>
  <link>http://uwadv.sourceforge.net/</link>
  <description>News items for Underworld Adventures site</description>
  <language>en-en</language>
  <copyright>News items (C) Michael Fink</copyright>
  <xsl:apply-templates select="div"/>
 </channel>
</rss>
</xsl:template>

<!-- process all news entries -->
<xsl:template match="div[@id='entry']">
  <item>
   <description>
  &lt;h1&gt;<xsl:value-of select="div[@id='heading']"/>&lt;h1&gt;
<xsl:apply-templates select="p"/>
   </description>
  </item>
</xsl:template>

<xsl:template match="p">
  &lt;p&gt;<xsl:value-of select="."/>&lt;/p&gt;
</xsl:template>

</xsl:stylesheet>
