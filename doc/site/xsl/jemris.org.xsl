<?xml version="1.0" encoding="ISO-8859-15"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="html" encoding="ISO-8859-15" indent="yes"/>
  
  <xsl:template match="br">
	<br/>
  </xsl:template>
  
  <xsl:template match="a">
	<a href="{@href}"><xsl:apply-templates/></a>
  </xsl:template>
  
  <xsl:template match="main">
    <html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en" > 

	  <head> 
		<!--<base href=""></base>-->
		<title>jemris - Extensible MRI Simulator - <xsl:value-of select="title" disable-output-escaping="yes"/></title>
        <link rel="stylesheet" href="jemris.css" type="text/css" media="projection, screen"/> 
	  </head> 
	  
	  <body class="nav" onload="navhl();"> 
        <div id="headbar"/>
        <div id="main">  
          <div id="nav"> <br/><br/><br/><br/><br/><br/></div>
          <div id="content"> 
            
            <xsl:value-of select="article" disable-output-escaping="yes"/>
            
            <div class="footer"><br/>last modified: <script>document.write(document.lastModified);</script> by <a href="mailto:k.vahedipour@fz-juelich.de?Subject=jemris">Kaveh Vahedipour</a></div>
          </div><!-- #content --> 
        </div><!-- #all -->
        <div id="footbar">Test him</div>
      </body> 
	</html>
  </xsl:template>
</xsl:stylesheet>
