<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
<!-- XSLT stylesheet to view devlog.xml -->
<!-- (C) 2004 Michael Fink              -->

<xsl:output method="html"/>

<!-- transform all "entry" nodes under "devlog", the root node -->
<xsl:template match="/devlog">
<xsl:apply-templates select="entry"/>
</xsl:template>

<!-- process all entries, output date and all paragraph bodies -->
<xsl:template match="entry">
<span class="devlogHeading"><xsl:value-of select="@date"/></span>
<xsl:for-each select="para">
<p class="devlogBody">
 <xsl:value-of select="text()"/>
</p>
</xsl:for-each>
</xsl:template>

</xsl:stylesheet>
