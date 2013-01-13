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
		<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" /> 
		<meta http-equiv="Content-Style-Type" content="text/css" /> 
		<meta http-equiv="Content-Script-Type" content="text/javascript" /> 
		<meta http-equiv="imagetoolbar" content="false" /> 
		<meta name="MSSmartTagsPreventParsing" content="true" /> 
		<meta name="genTime" content="Mon May 02 09:25:33 CEST 2011"/> 
		<meta name="generator" content="Government Site Builder 4.0"/> 
		<meta name="google-site-verification" content="SImQzSmyFuCBHlFPcEZY9_VMe7h_UZNKB8Cu9vX0FlI" />    
		<link rel="bookmark" href="http://www.fz-juelich.de/inm/inm-4/EN/Leistungen/JEMRIS/_node.html#Start" type="text/html" title="Zum Inhalt" /><!-- NOTE: points to the beginning of the document -->		<link rel="copyright" href="http://www.fz-juelich.de/portal/EN/Service/Imprint/imprint_node.html" type="text/html" title="Imprint" /> 
		<link rel="glossary" href="http://www.fz-juelich.de/" type="text/html" title="Glossary" /> 
		<link rel="help" href="http://www.fz-juelich.de/" type="text/html" title="Help" /> 
		<link rel="start" href="http://www.fz-juelich.de/inm/inm-4/EN/Home/home_node.html" type="text/html" title="Homepage" /> 
		<link rel="contents" href="http://www.fz-juelich.de/portal/DE/Service/Sitemap/sitemap_node.html" type="text/html" title="Sitemap" /> 
		<link rel="search" href="http://www.fz-juelich.de/portal/EN/Service/Search/search_node.html" type="text/html" title="Search" /> 
		<link rel="up" href="http://www.fz-juelich.de/inm/inm-4/EN/Leistungen/leistungen_node.html" type="text/html" title="Expertise" /> 
		<link rel="shortcut icon" href="http://www.fz-juelich.de//cae/servlet/contentblob/8466/normal/15/favicon.ico" type="image/ico" /> 
		<link rel="stylesheet" href="http://www.fz-juelich.de/SiteGlobals/StyleBundles/CSS/visual/visual.css?v=2" type="text/css" media="print, projection, screen" /> 
		<link rel="stylesheet" href="http://www.fz-juelich.de/SiteGlobals/StyleBundles/CSS/screen/screen-a.css?v=3" type="text/css" media="projection, screen"  /> 
		<!-- Additional IE/Win specific style sheet (Conditional Comments) --><!--[if lte IE 7]><link rel="stylesheet" href="http://www.fz-juelich.de/SiteGlobals/StyleBundles/CSS/screen/screen_iew.css?v=2" type="text/css" media="projection, screen" /><![endif]--> 
		<link rel="stylesheet"  href="http://www.fz-juelich.de/SiteGlobals/StyleBundles/CSS/screen/startseite2.css?v=5" type="text/css" media ="projection, screen"  /> 
		<link rel="stylesheet" href="http://www.fz-juelich.de/SiteGlobals/StyleBundles/CSS/print/print.css?v=2" type="text/css" media="print" /> 
		<link rel="stylesheet"  href="http://www.fz-juelich.de/SiteGlobals/StyleBundles/CSS/screen/kalender.css?v=3" type="text/css" media ="projection, screen"  /> 
		<link rel="stylesheet"  href="http://www.fz-juelich.de/SiteGlobals/StyleBundles/CSS/screen/HintergrundInstitute/hintergrundbild-allgemein.css?v=4" type="text/css" media ="projection, screen"/> 
		<link rel="stylesheet"  href="../../doxygen/doxygen.css" type="text/css"/>
	  </head> 
	  
	  <body class="gsb mitHeader" onload="sh_highlightDocument(); navhl();"> 
		<div id="wrapperBranding"> 
		  <div id="branding_1" class="outer"> 
			<div id="brandingInnen"> 
			  <p><a href="http://fz-juelich.de/portal/EN/Home/home_node.html" title="link to homepage"><img src="http://www.fz-juelich.de/cae/servlet/contentblob/12836/normal/3235/logo.gif" alt="link to homepage" /></a></p> 
			  <div id="instNaviLinkWrapper"><a href="http://www.fz-juelich.de/inm/inm-4/EN/Leistungen/JEMRIS/_node.html#instNaviContentLink" id="instNaviLink">Institutes</a></div> 
			  <div id="instHeaders"> 
				<div class="bereich1">Institute for Neuroscience and Medicine</div> 
			  <div class="bereich2"><a href="http://www.fz-juelich.de/inm/inm-4/EN/Home/home_node.html" title="ZumInstitutsbereich"><strong>Medical Imaging Physics (INM-4)</strong></a></div></div> 
			  <div id="navPrimary"> 
				<h2> Main Menu</h2> 
				<ul>
				  <li><a href="http://www.fz-juelich.de/inm/inm-4/EN/Aktuelles/aktuelles_node.html">News</a></li>
				  <li><a href="http://www.fz-juelich.de/inm/inm-4/EN/Forschung/forschung_node.html">Research</a></li>
				  <li><em><a href="http://www.fz-juelich.de/inm/inm-4/EN/Leistungen/leistungen_node.html">Expertise</a></em></li>
				  <li><a href="http://www.fz-juelich.de/inm/inm-4/EN/Karriere/karriere_node.html">Career</a></li>
				  <li><a href="http://www.fz-juelich.de/inm/inm-4/EN/UeberUns/ueberUns_node.html">About us</a></li>
				</ul> 
			  </div><!-- #navPrimary --> 
			</div><!-- #brandingInnen --> 
		  </div><!-- #branding_1 --> 
		</div><!-- #wrapperBranding --> 
		
		<div id="wrapperOuter" > 
		  <div id="wrapperInner"> 
			<a id="Start" name="Start"></a> 
			<h1 class="navSkip">Navigation and service</h1> 
			<p class="navSkip"><em>Go to:</em></p> 
			<ul class="navSkip"> 
			  <li><a href="http://www.fz-juelich.de/inm/inm-4/EN/Leistungen/JEMRIS/_node.html#Inhalt">Content</a></li> 
			  <li><a href="http://www.fz-juelich.de/inm/inm-4/EN/Leistungen/JEMRIS/_node.html#navPrimary">Main Menu</a></li> 
			  <li><a href="http://www.fz-juelich.de/inm/inm-4/EN/Leistungen/JEMRIS/_node.html#Suche">Search</a></li> 
			</ul><!-- .navSkip --> 
			<div id="navBreadcrumbs"> 
			  <h2 class="aural">You are here:</h2> 
			  <ol>
				<li><a class="home" href="http://www.fz-juelich.de/inm/inm-4/EN/Home/home_node.html"><span><span>INM-4</span></span></a></li>
				<li><a href="http://www.fz-juelich.de/inm/inm-4/EN/Leistungen/leistungen_node.html"><span><span>Expertise</span></span></a></li>
				<li><strong><span><span>JEMRIS: MR Simulations Software</span></span></strong></li>
			  </ol> 
			  <div class="clear"></div> 
			</div><!-- #navBreadcrumbs --> 
			<hr /> 
			<div id="wrapperDivisions" class="modgrid"> 
			  <div id="navSecondary"> 
				<h2 class="aural"><a id="Bereichsmenu" name="Bereichsmenu">Area Menu</a></h2> 
				<div class="navMain">
				  <script src="nav.js" type="text/javascript"></script>
				</div><!-- #navService --> 
			  </div><!-- #navSecondary --> 
			  
			  <div id="wrapperContent"> 
				
				<div id="content"> 
				  <a id="Inhalt" name="Inhalt"></a> 
				  
				  <!--
				  <h1 class="isFirstInSlot">API documentation</h1>
				  <h1 class="isFirstInSlot">User guide</h1>
				  -->
				  
				  <xsl:value-of select="article" disable-output-escaping="yes"/>
				  
				  <div class="footer"><br/>last modified: <script>document.write(document.lastModified);</script> by <a href="mailto:k.vahedipour@fz-juelich.de?Subject=jemris">Kaveh Vahedipour</a></div>
				</div><!-- #content --> 
			  </div><!-- #wrapperContent --> 
			  <div id="supplement"  > 
				<h2 class="aural"><a id="Zusatzinformationen" name="Zusatzinformationen">Additional Information</a></h2> 
				
				
				
			  </div><!-- #supplement --> 
			  <div class="clear"></div> 
			</div><!-- #wrapperDivisions --> 
		  </div><!-- #wrapperInner --> 
		  <hr /> 
		  <div id="siteInfo"> 
			<h2>Servicemeu</h2> 
			<div id="navServiceFooter"> 
			  
			  
			  <ul><li><a href="http://www.fz-juelich.de/portal/EN/Service/Imprint/imprint_node.html">Imprint</a>&#160;&#160;</li><li><a href="http://www.fz-juelich.de/portal/EN/Service/Contact/contact_node.html?docId=894704">Contact</a>&#160;&#160;</li><li><a title="Zum deutschen Auftritt" class="languageLink lang_de" href="http://www.fz-juelich.de/inm/inm-4/DE/Leistungen/JEMRIS/_node.html" xml:lang="de" hreflang="de" lang="de">Deutsch</a>&#160;&#160;</li><li><span title="Switch to English (not active)" class="languageLink lang_en" xml:lang="en" lang="en">English</span></li></ul> 
			  <!--<ul id="navFunctions"> 
				  <li id="navFunctionsRecommend"><a href="http://www.fz-juelich.de/SiteGlobals/Functions/SeiteEmpfehlen/EN/Mailversand.html?nn=894704&amp;handOverParams=uriHash%253De6f6e65f69dd0baf%2526uri%253Dhttp%25253A%25252F%25252Fwww.fz-juelich.de%25252Finm%25252Finm-4%25252FEN%25252FLeistungen%25252FJEMRIS%25252F_node.html" class="" >Recommend page</a></li> 
				  
				  <li class="social"><a href="http://del.icio.us/post?url=http%3A%2F%2Fwww.fz-juelich.de%2Finm%2Finm-4%2FEN%2FLeistungen%2FJEMRIS%2F_node.html" title="Delicious" target="_blank"><img src="http://www.fz-juelich.de//cae/servlet/contentblob/46862/normal/147/delicious.png" alt="Bookmarken Sie diese Seite via Delicious - öffnet ein neues Browserfenster"/></a></li> 
				  <li class="social"><a href="http://de.facebook.com/sharer.php?u=http%3A%2F%2Fwww.fz-juelich.de%2Finm%2Finm-4%2FEN%2FLeistungen%2FJEMRIS%2F_node.html" title="Facebook" target="_blank"><img src="http://www.fz-juelich.de//cae/servlet/contentblob/46864/normal/333/facebook.gif" alt="Bookmarken Sie diese Seite via facebook - öffnet ein neues Browserfenster"/></a></li> 
				  <li class="social"><a href="http://www.folkd.com/submit/http%3A%2F%2Fwww.fz-juelich.de%2Finm%2Finm-4%2FEN%2FLeistungen%2FJEMRIS%2F_node.html" title="Folkd" target="_blank"><img src="http://www.fz-juelich.de//cae/servlet/contentblob/46866/normal/149/folkd.gif" alt="Bookmarken Sie diese Seite via folkd - öffnet ein neues Browserfenster"/></a></li> 
				  <li class="social"><a href="http://www.google.com/bookmarks/mark?op=add&amp;bkmk=http%3A%2F%2Fwww.fz-juelich.de%2Finm%2Finm-4%2FEN%2FLeistungen%2FJEMRIS%2F_node.html" title="Google" target="_blank"><img src="http://www.fz-juelich.de//cae/servlet/contentblob/46868/normal/150/googleBookmarks.png" alt="Bookmarken Sie diese Seite via Google Bookmarks - öffnet ein neues Browserfenster"/></a></li> 
				  <li class="social"><a href="http://linkarena.com/bookmarks/addlink/?url=http%3A%2F%2Fwww.fz-juelich.de%2Finm%2Finm-4%2FEN%2FLeistungen%2FJEMRIS%2F_node.html" title="Linkarena" target="_blank"><img src="http://www.fz-juelich.de//cae/servlet/contentblob/46870/normal/151/linkarena.gif" alt="Bookmarken Sie diese Seite via Linkarena - öffnet ein neues Browserfenster"/></a></li> 
				  <li class="social"><a href="http://www.mister-wong.de/index.php?action=addurl&amp;bm_url=http%3A%2F%2Fwww.fz-juelich.de%2Finm%2Finm-4%2FEN%2FLeistungen%2FJEMRIS%2F_node.html" title="Mister Wong" target="_blank"><img src="http://www.fz-juelich.de//cae/servlet/contentblob/46872/normal/152/misterWong.gif" alt="Bookmarken Sie diese Seite via Mister Wong - öffnet ein neues Browserfenster"/></a></li> 
				  <li class="social"><a href="http://www.myspace.com/index.cfm?fuseaction=postto&amp;=http%3A%2F%2Fwww.fz-juelich.de%2Finm%2Finm-4%2FEN%2FLeistungen%2FJEMRIS%2F_node.html" title="MySpace" target="_blank"><img src="http://www.fz-juelich.de//cae/servlet/contentblob/46874/normal/153/mySpace.gif" alt="Bookmarken Sie diese Seite via MySpace - öffnet ein neues Browserfenster"/></a></li> 
				  <li class="social"><a href="http://www.netvibes.com/signin?url=http%3A%2F%2Fwww.fz-juelich.de%2Finm%2Finm-4%2FEN%2FLeistungen%2FJEMRIS%2F_node.html" title="Netvibes" target="_blank"><img src="http://www.fz-juelich.de//cae/servlet/contentblob/46876/normal/154/netvibes.png" alt="Bookmarken Sie diese Seite via Netvibes - öffnet ein neues Browserfenster"/></a></li> 
				  <li class="social"><a href="http://www.newsvine.com/_tools/seed&amp;save?u=http%3A%2F%2Fwww.fz-juelich.de%2Finm%2Finm-4%2FEN%2FLeistungen%2FJEMRIS%2F_node.html" title="Newsvine" target="_blank"><img src="http://www.fz-juelich.de//cae/servlet/contentblob/46878/normal/155/newsvine.png" alt="Bookmarken Sie diese Seite via Newsvine - öffnet ein neues Browserfenster"/></a></li> 
				  <li class="social"><a href="http://reddit.com/submit?url=http%3A%2F%2Fwww.fz-juelich.de%2Finm%2Finm-4%2FEN%2FLeistungen%2FJEMRIS%2F_node.html" title="reddit" target="_blank"><img src="http://www.fz-juelich.de//cae/servlet/contentblob/46880/normal/156/reddit.png" alt="Bookmarken Sie diese Seite via reddit - öffnet ein neues Browserfenster"/></a></li> 
				  <li class="social"><a href="http://www.studivz.net/Suggest/Selection/?u=http%3A%2F%2Fwww.fz-juelich.de%2Finm%2Finm-4%2FEN%2FLeistungen%2FJEMRIS%2F_node.html" title="studiVZ" target="_blank"><img src="http://www.fz-juelich.de//cae/servlet/contentblob/46882/normal/157/studiVZ.gif" alt="Bookmarken via studiVZ"/></a></li> 
				  <li class="social"><a href="http://www.stumbleupon.com/submit?url=http%3A%2F%2Fwww.fz-juelich.de%2Finm%2Finm-4%2FEN%2FLeistungen%2FJEMRIS%2F_node.html" title="Stumbleupon" target="_blank"><img src="http://www.fz-juelich.de//cae/servlet/contentblob/46884/normal/158/stumbleupon.png" alt="Bookmarken Sie diese Seite via Stumbleupon - öffnet ein neues Browserfenster"/></a></li> 
				  <li class="social"><a href="http://twitter.com/home?status=http%3A%2F%2Fwww.fz-juelich.de%2Finm%2Finm-4%2FEN%2FLeistungen%2FJEMRIS%2F_node.html" title="twitter" target="_blank"><img src="http://www.fz-juelich.de//cae/servlet/contentblob/46886/normal/159/twitter.png" alt="Bookmarken Sie diese Seite via twitter - öffnet ein neues Browserfenster"/></a></li> 
				  <li class="social"><a href="https://favorites.live.com/quickadd.aspx?marklet=1&amp;mkt=en-us&amp;url=http%3A%2F%2Fwww.fz-juelich.de%2Finm%2Finm-4%2FEN%2FLeistungen%2FJEMRIS%2F_node.html" title="Windows Live" target="_blank"><img src="http://www.fz-juelich.de//cae/servlet/contentblob/46888/normal/160/windowsLive.gif" alt="Bookmarken Sie diese Seite via Windows Live - öffnet ein neues Browserfenster"/></a></li> 
				  <li class="social"><a href="http://myweb2.search.yahoo.com/myresults/bookmarklet?t=&amp;d=&amp;tag=&amp;u=http%3A%2F%2Fwww.fz-juelich.de%2Finm%2Finm-4%2FEN%2FLeistungen%2FJEMRIS%2F_node.html" title="Yahoo" target="_blank"><img src="http://www.fz-juelich.de//cae/servlet/contentblob/46890/normal/148/yahoo.png" alt="Bookmarken Sie diese Seite via Yahoo - öffnet ein neues Browserfenster"/></a></li> 
				  <li class="social"><a href="http://www.yigg.de/neu?exturl=http%3A%2F%2Fwww.fz-juelich.de%2Finm%2Finm-4%2FEN%2FLeistungen%2FJEMRIS%2F_node.html" title="Yigg" target="_blank"><img src="http://www.fz-juelich.de//cae/servlet/contentblob/46892/normal/162/yigg.gif" alt="Bookmarken Sie diese Seite via Yigg - öffnet ein neues Browserfenster"/></a></li> 
				  </ul>--><!-- #navFunctions --> 
			  <div class="clear"></div>       
			</div><!-- #navServiceFooter --> 
			<p class="logo"> 
			  <img src="http://www.fz-juelich.de//cae/servlet/contentblob/8472/normal/326/logo_helmholtz.gif" title="Helmholtz-Gemeinschaft" alt="Helmholtz-Gemeinschaft" /> 
			</p> 
		  </div><!-- #SiteInfo --> 
		  <div class="clear"></div> 
		  <div id="instNavi"> 
			<a id="instNaviContentLink"></a> 
			<div class="institute"> 
			  <h1>institutes</h1> 
			  <ul> 
				<li class="mod">
				  <a  href="http://www.fz-juelich.de/portal/EN/AboutUs/Institutes_Facilities/Institutes/InstituteAdvancedSimulation/_node.html" title="" class="themenLink">Institute for Advanced Simulation (IAS)</a>
				</li> 
				<li class="mod">
				  <a  href="http://www.fz-juelich.de/portal/EN/AboutUs/Institutes_Facilities/Institutes/InstituteBioGeosciences/_node.html" title="" class="themenLink">Institute of Bio- and Geosciences (IBG)</a>
				</li> 
				<li class="mod">
				  <a  href="http://www.fz-juelich.de/portal/EN/AboutUs/Institutes_Facilities/Institutes/InstituteComplexSystems/_node.html" title="" class="themenLink">Institute of Complex Systems (ICS)</a>
				</li> 
				<li class="mod">
				  <a  href="http://www.fz-juelich.de/portal/EN/AboutUs/Institutes_Facilities/Institutes/InstituteEnergyClimate/_node.html" title="" class="themenLink">Institute of Energy and Climate Research (IEK)</a>
				</li> 
			  </ul> 
			  <ul> 
				<li class="mod">
				  <a  href="http://www.fz-juelich.de/portal/EN/AboutUs/Institutes_Facilities/Institutes/InstituteNeuroscienceMedicine/_node.html" title="" class="themenLink">Institute of Neuroscience and Medicine (INM)</a>
				</li> 
				<li class="mod">
				  <a  href="http://www.fz-juelich.de/portal/EN/AboutUs/Institutes_Facilities/Institutes/JuelichCentreNeutronScience/_node.html" title="" class="themenLink">Jülich Centre for Neutron Science (JCNS)</a>
				</li> 
				<li class="mod">
				  <a  href="http://www.fz-juelich.de/portal/EN/AboutUs/Institutes_Facilities/Institutes/NuclearPhysicsInstitute/_node.html" title="" class="themenLink">Nuclear Physics Institute (IKP)</a>
				</li> 
				<li class="mod">
				  <a  href="http://www.fz-juelich.de/portal/EN/AboutUs/Institutes_Facilities/Institutes/PeterGruenbergInstitute/_node.html" title="" class="themenLink">Peter Grünberg Institute (PGI)</a>
				</li> 
			  </ul> 
			</div> 
			<div class="einrichtungen"> 
			  <h1>Scientific Technical Facilities</h1> 
			  <ul> 
				<li class="mod">
				  <a  href="http://www.fz-juelich.de/zat/DE/Home/home_node.html" title="" class="themenLink">Zentralabteilung Technologie (ZAT)</a>
				</li> 
				<li class="mod">
				  <a  href="http://www.fz-juelich.de/zch/EN/Home/home_node.html" title="" class="themenLink">Central Division of Analytical Chemistry (ZCH)</a>
				</li> 
				<li class="mod"><a class="external" href="http://www2.fz-juelich.de/zel/zel_home" title="Opens new window" target="_blank">Central Institute for Electronics (ZEL)</a></li> 
			  </ul> 
			</div> 
		  </div> 
		</div> <!-- #wrapperOuter --> 
		<script type="text/javascript">//<![CDATA[
		// i18n
		var PRINT_PAGE_TEXT = 'Print';
		var PRINT_TOOLTIP = 'Print (opens dialog)';      
		//]]></script> 
		<script src="http://www.fz-juelich.de/SiteGlobals/Functions/JavaScript/libs/jquery.js?view=renderPlain&amp;nn=894704" type="text/javascript" ></script> 
		<script src="http://www.fz-juelich.de/SiteGlobals/Functions/JavaScript/libs/jquery.ui.core.js?view=renderPlain&amp;nn=894704" type="text/javascript" ></script> 
		<script src="http://www.fz-juelich.de/SiteGlobals/Functions/JavaScript/libs/jquery.ui.widget.js?view=renderPlain&amp;nn=894704" type="text/javascript" ></script> 
		<script src="http://www.fz-juelich.de/SiteGlobals/Functions/JavaScript/rotator.js" type="text/javascript" ></script> 
		<script src="http://www.fz-juelich.de/SiteGlobals/Functions/JavaScript/instnav.js?view=renderPlain&amp;nn=894704" type="text/javascript" ></script> 
		<script src="http://www.fz-juelich.de/SiteGlobals/Functions/JavaScript/img_switcher.js?view=renderPlain&amp;nn=894704" type="text/javascript" ></script> 
		<script src="http://www.fz-juelich.de/SiteGlobals/Functions/JavaScript/calendar.js?view=renderPlain&amp;nn=894704" type="text/javascript" ></script> 
		<script src="http://www.fz-juelich.de/SiteGlobals/Functions/JavaScript/blind.js?view=renderPlain&amp;nn=894704" type="text/javascript" ></script> 
		<script src="http://www.fz-juelich.de/SiteGlobals/Functions/JavaScript/initjs.js?view=renderPlain&amp;nn=894704" type="text/javascript" ></script> 
		<script src="http://www.fz-juelich.de/SiteGlobals/Functions/JavaScript/lib.js?view=renderPlain&amp;nn=894704" type="text/javascript" ></script> 
		<script src="http://www.fz-juelich.de/SiteGlobals/Functions/JavaScript/print.js?view=renderPlain&amp;nn=894704" type="text/javascript" ></script> 
		<script src="http://www.fz-juelich.de/SiteGlobals/Functions/JavaScript/behavior.js?view=render&amp;nn=894704" type="text/javascript" ></script> 
		<!--Realisiert mit dem Government Site Builder, der Content Management Lösung der Bundesverwaltung, www.government-site-builder.de --> 
	  </body> 
	</html>
  </xsl:template>
</xsl:stylesheet>
