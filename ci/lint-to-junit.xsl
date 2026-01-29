<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="xml" indent="yes" encoding="UTF-8"/>
  <xsl:template match="/issues">
    <testsuites>
      <testsuite name="AndroidLint"
                 tests="{count(issue)}"
                 failures="{count(issue)}">
        <xsl:for-each select="issue">
          <testcase classname="Android Lint" name="{@id}" file="{location/@file}">
            <failure>
              <xsl:value-of select="@summary"/>
            </failure>
            <system-out>
              <xsl:value-of select="@message"/>
              <xsl:text>&#10;</xsl:text>
              <xsl:text>&#10;</xsl:text><xsl:value-of select="@explanation"/>
            </system-out>
            <system-err>
              <xsl:text>Severity: </xsl:text><xsl:value-of select="@severity"/>
              <xsl:text>&#10;Category: </xsl:text><xsl:value-of select="@category"/>
              <xsl:text>&#10;Priority: </xsl:text><xsl:value-of select="@priority"/><xsl:text> / 10</xsl:text>
              <xsl:text>&#10;Line: </xsl:text><xsl:value-of select="location/@line"/>
              <xsl:text>&#10;Column: </xsl:text><xsl:value-of select="location/@column"/>
              <xsl:text>&#10;</xsl:text>
              <xsl:text>&#10;</xsl:text><xsl:value-of select="@errorLine1"/>
              <xsl:text>&#10;</xsl:text><xsl:value-of select="@errorLine2"/>
            </system-err>
          </testcase>
        </xsl:for-each>
      </testsuite>
    </testsuites>
  </xsl:template>
</xsl:stylesheet>
