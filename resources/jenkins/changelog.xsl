<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output indent="yes" method="html" omit-xml-declaration="yes"/>

  <xsl:template match="/">
      <html>
      <head>
          <meta charset="utf-8" />
      </head>
      <body>
      <xsl:for-each select="changesets">

          <xsl:for-each select="changeset">
              <h3>Changeset <xsl:value-of select="@rev"/> : <xsl:value-of select="@node"/></h3>
              <b>Author:</b> <xsl:value-of select="@author"/><br/>
              <b>Files:</b> <xsl:value-of select="files"/><br/>
              <b>Comment:</b> <pre><xsl:value-of select="msg"/></pre>
              <hr/>
          </xsl:for-each>

      </xsl:for-each>
      </body></html>
  </xsl:template>

</xsl:stylesheet>
