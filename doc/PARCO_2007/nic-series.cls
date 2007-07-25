%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%
%% nic-proceedings.cls  : December 2001
%% Class file to be used for proceedings 
%% published by NIC
%% size [9.75" x 6.50"] 
%% Suggestions/comments to: m.marx@fz-juelich.de        or 
%%                          b.herrmann@fz-juelich.de
%% 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
\NeedsTeXFormat{LaTeX2e}[1995/12/01]
\RequirePackage{times}
\RequirePackage{amssymb}
\RequirePackage{amstext}
\RequirePackage{amsmath}
\RequirePackage{ifthen}
\RequirePackage{epsf}
\RequirePackage{rotating}
\RequirePackage{expdlist}
\RequirePackage[nosort,nomove]{overcite}
\RequirePackage{alltt}
\newif\if@restonecol
\newif\if@titlepage
\newboolean{OptPoster}\setboolean{OptPoster}{false}
\newcounter{posternum}
\setcounter{posternum}{1}
\newcounter{posternumB}
\setcounter{posternumB}{1}
%
\DeclareOption{oneside}{\@twosidefalse \@mparswitchfalse}
\DeclareOption{twoside}{\@twosidetrue  \@mparswitchtrue} 
\DeclareOption{draft}{\setlength\overfullrule{5pt}}
\DeclareOption{final}{\setlength\overfullrule{0pt}}
\DeclareOption{twocolumn}{\@twocolumntrue}
\DeclareOption{poster}{
  \setboolean{OptPoster}{true}
}

\def\cleardoublepage{\clearpage\if@twoside \ifodd\c@page\else
    \hbox{}\thispagestyle{empty}\newpage\if@twocolumn\hbox{}\newpage\fi\fi\fi}

%
\ExecuteOptions{a4paper,10pt,twoside,twocolumn,final}
\ProcessOptions
%
\newcommand{\AUTHORTOC}{}
\newcommand{\authortoc}[1]{\renewcommand{\AUTHORTOC}{#1}}
\newcommand{\TITLETOC}{}
\newcommand{\titletoc}[1]{\renewcommand{\TITLETOC}{#1}}
\let\AUTHORTOC\@empty
\let\TITLETOC\@empty
\newcommand\section{\@startsection{section}{1}{\z@}%
                       {-18\p@ \@plus -4\p@ \@minus -4\p@}%
                       {12\p@ \@plus 4\p@ \@minus 4\p@}%
                       {\normalfont\large\bfseries\boldmath
                        \rightskip=\z@ \@plus 8em\pretolerance=10000 }}
\newcommand\subsection{\@startsection{subsection}{2}{\z@}%
                       {-18\p@ \@plus -4\p@ \@minus -4\p@}%
                       {8\p@ \@plus 4\p@ \@minus 4\p@}%
                       {\normalfont\normalsize\bfseries\boldmath
                        \rightskip=\z@ \@plus 8em\pretolerance=10000 }}
\newcommand\subsubsection{\@startsection{subsubsection}{3}{\z@}%
                       {-18\p@ \@plus -4\p@ \@minus -4\p@}%
                       {8\p@ \@plus 4\p@ \@minus 4\p@}%
%                       {-0.5em \@plus -0.22em \@minus -0.1em}%
                       {\normalfont\normalsize\bfseries\boldmath
                        \rightskip=\z@ \@plus 8em\pretolerance=10000  }}
\newcommand\paragraph{\@startsection{paragraph}{4}{\z@}%
                       {-12\p@ \@plus -4\p@ \@minus -4\p@}%
                       {-0.5em \@plus -0.22em \@minus -0.1em}%
                       {\normalfont\normalsize\itshape}}
\newcommand\chapter{\clearpage
 %                   \thispagestyle{empty}%
                    \global\@topnum\z@
                    \@afterindentfalse
                    \secdef\@chapter\@schapter}
\renewcommand\normalsize{%
   \@setfontsize\normalsize\@xpt\@xiipt
   \abovedisplayskip6\p@\@plus4\p@\@minus2\p@
   \abovedisplayshortskip\abovedisplayskip
   \belowdisplayshortskip\abovedisplayshortskip
   \belowdisplayskip\abovedisplayskip
   \let\@listi\@listI}
\normalsize
\newcommand\small{%
   \@setfontsize\small\@ixpt{11}%
   \abovedisplayskip 8.5\p@ \@plus3\p@ \@minus4\p@
   \abovedisplayshortskip \z@ \@plus2\p@
   \belowdisplayshortskip 4\p@ \@plus2\p@ \@minus2\p@
   \def\@listi{\leftmargin\leftmargini
               \topsep 4\p@ \@plus2\p@ \@minus2\p@
               \parsep 2\p@ \@plus\p@ \@minus\p@
               \itemsep \parsep}%
   \belowdisplayskip \abovedisplayskip
}
\newcommand\footnotesize{%
   \@setfontsize\footnotesize\@viiipt{9.5}%
   \abovedisplayskip 6\p@ \@plus2\p@ \@minus4\p@
   \abovedisplayshortskip \z@ \@plus\p@
   \belowdisplayshortskip 3\p@ \@plus\p@ \@minus2\p@
   \def\@listi{\leftmargin\leftmargini
               \topsep 3\p@ \@plus\p@ \@minus\p@
               \parsep 2\p@ \@plus\p@ \@minus\p@
               \itemsep \parsep}%
   \belowdisplayskip \abovedisplayskip
}
\newcommand\scriptsize{\@setfontsize\scriptsize\@viipt\@viiipt}
\newcommand\tiny{\@setfontsize\tiny\@vpt\@vipt}
\newcommand\large{\@setfontsize\large\@xiipt{14}}
\newcommand\Large{\@setfontsize\Large\@xivpt{18}}
\newcommand\LARGE{\@setfontsize\LARGE\@xviipt{22}}
\newcommand\huge{\@setfontsize\huge\@xxpt{25}}
\newcommand\Huge{\@setfontsize\Huge\@xxvpt{30}}
\setlength\parindent{0.25in}
\setlength\smallskipamount{3\p@ \@plus 1\p@ \@minus 1\p@}
\setlength\medskipamount{6\p@ \@plus 2\p@ \@minus 2\p@}
\setlength\bigskipamount{12\p@ \@plus 4\p@ \@minus 4\p@}
%%%%%%
%
\newdimen\trimheight
\newdimen\trimwidth
\newdimen\typeheight
\newdimen\typewidth
\newdimen\tempdimen
\newdimen\normaltextheight
%
\newdimen\tocchpnum
\newdimen\tocsecnum
\newdimen\tocsectotal
\newdimen\tocsubsecnum
\newdimen\tocsubsectotal
\newdimen\tocsubsubsecnum
\newdimen\tocsubsubsectotal
\newdimen\tocparanum
\newdimen\tocparatotal
\newdimen\tocsubparanum
%\tocchpnum=\z@           % chapter numbers desired  % b.herrmann
\tocchpnum=15\p@
\tocsecnum=22\p@          % section 88. plus 2.222pt
\tocsectotal=15\p@
\tocsubsecnum=23\p@       % subsection 88.8 plus 2.222pt
\tocsubsubsecnum=27\p@    % subsubsection 88.8.8 plus 1.444pt
\tocparanum=35\p@         % paragraph 88.8.8.8 plus 1.666pt
\tocsubparanum=43\p@      % subparagraph 88.8.8.8.8 plus 1.888pt

\trimheight9.75in  % 24.765 cm
\addtolength\trimheight{.047in}
\trimwidth6.50in
\typeheight48.10pc    % 20.29 cm

%
\newdimen\headlineindent             % dimension for space between
\headlineindent=1.166cm 
\setlength\parindent{1.5em}%10\p@
\setlength\headheight{6\p@}
\setlength\headsep   {15pt}%11
\setlength\topskip   {7\p@}%{10\p@}
\setlength\footskip{18pt}
\setlength\maxdepth{4\p@}
\setlength\@maxdepth\maxdepth
\setlength\textwidth{30pc}   % 12.65 cm 
\setlength\textheight\typeheight
\advance\textheight-\headheight
\advance\textheight-\headsep
\advance\textheight-3pt  
\setlength\normaltextheight{\textheight}
\setlength\oddsidemargin{60pt}%.625in%
% Marx , naechste Zeile fuer die horizontale Ausrichtung
%        der geraden und ungeraden seiten
\addtolength{\oddsidemargin}{-0.4cm}
%\setlength\oddsidemargin{118pt}%.625in%

%hier eingefuegt fuer tia00cd
\addtolength{\oddsidemargin}{-0.7pt}
\setlength\evensidemargin{\oddsidemargin}
\setlength\marginparwidth{.75in}
\setlength\marginparsep{7\p@}
\setlength\marginparpush{5\p@}
\setlength\topmargin{.65926in}%
\setlength\footnotesep{6\p@}%6pt
\setlength{\skip\footins}{11\p@ \@plus 5\p@ \@minus 2\p@}
\setlength\floatsep    {18\p@ \@plus 2\p@ \@minus 2\p@}
\setlength\textfloatsep{20\p@ \@plus 2\p@ \@minus 4\p@}
\setlength\intextsep   {18\p@ \@plus 2\p@ \@minus 2\p@}
\setlength\dblfloatsep    {18\p@ \@plus 2\p@ \@minus 2\p@}
\setlength\dbltextfloatsep{20\p@ \@plus 2\p@ \@minus 4\p@}
\setlength\@fptop{0\p@}
\setlength\@fpsep{8\p@ \@plus 2fil}
\setlength\@fpbot{0\p@ \@plus 1fil}
\setlength\@dblfptop{0\p@ \@plus 1fil}
\setlength\@dblfpsep{8\p@ \@plus 2fil}
\setlength\@dblfpbot{0\p@ \@plus 1fil}
\setlength\partopsep{0\p@}
%
\setlength\lineskip{1\p@}
\setlength\normallineskip{1\p@}%
\renewcommand\baselinestretch{}%{}
\setlength\parskip{0\p@}
\@lowpenalty   51
\@medpenalty  151
\@highpenalty 301
\@beginparpenalty -\@lowpenalty
\@endparpenalty   -\@lowpenalty
\@itempenalty     -\@lowpenalty
%
\def\l@section{\@dottedtocline{1}{\tocchpnum}{\tocsecnum}}
\def\l@subsection{\@dottedtocline{2}{\tocsectotal}{\tocsubsecnum}}
\def\l@subsubsection{\@dottedtocline{3}{\tocsubsectotal}{\tocsubsubsecnum}}
\def\l@paragraph{\@dottedtocline{4}{\tocsubsubsectotal}{\tocparanum}}
\def\l@subparagraph{\@dottedtocline{5}{\tocparatotal}{\tocsubparanum}}
\def\@schapter#1{\if@twocolumn
                   \@topnewpage[\@makeschapterhead{#1}]%
                 \else
                   \@makeschapterhead{#1}%
                   \@afterheading
                 \fi}
\def\@makeschapterhead#1{%
% \vspace*{50\p@}%
  {\centering
    \normalfont
    \interlinepenalty\@M
    \Large \bfseries  #1\par\nobreak
    \vskip 40\p@
  }}
\def\tableofcontents{\chapter*{\contentsname\@mkboth{{\contentsname}}%
                                                   {{\contentsname}}}%
 \@starttoc{toc}\if@restonecol\twocolumn\fi}                                                 \def\@listi{\leftmargin\leftmargini
            \parsep 4\p@ \@plus2\p@ \@minus\p@
            \topsep 8\p@ \@plus2\p@ \@minus4\p@
            \itemsep4\p@ \@plus2\p@ \@minus\p@}
\let\@listI\@listi
\@listi
\def\@listii {\leftmargin\leftmarginii
              \labelwidth\leftmarginii
              \advance\labelwidth-\labelsep
              \topsep    4\p@ \@plus2\p@ \@minus\p@
              \parsep    2\p@ \@plus\p@  \@minus\p@
              \itemsep   \parsep}
\def\@listiii{\leftmargin\leftmarginiii
              \labelwidth\leftmarginiii
              \advance\labelwidth-\labelsep
              \topsep    2\p@ \@plus\p@\@minus\p@
              \parsep    \z@
              \partopsep \p@ \@plus\z@ \@minus\p@
              \itemsep   \topsep}
\def\@listiv {\leftmargin\leftmarginiv
              \labelwidth\leftmarginiv
              \advance\labelwidth-\labelsep}
\def\@listv  {\leftmargin\leftmarginv
              \labelwidth\leftmarginv
              \advance\labelwidth-\labelsep}
\def\@listvi {\leftmargin\leftmarginvi
              \labelwidth\leftmarginvi
              \advance\labelwidth-\labelsep}
%
\setcounter{topnumber}{3}
\def\topfraction{.98}
\setcounter{bottomnumber}{3}
\def\bottomfraction{.98}
\setcounter{totalnumber}{6}%4
\def\textfraction{.01}
\def\floatpagefraction{.98100}
\setcounter{dbltopnumber}{2}
\def\dbltopfraction{.981}
\def\dblfloatpagefraction{.99}
%
\def\ps@headings{%
      \let\@oddfoot\@empty\let\@evenfoot\@empty
      \def\@evenhead{\thepage\hfil\slshape\leftmark}%
      \def\@oddhead{{\slshape\rightmark}\hfil\thepage}%
      \let\@mkboth\markboth
    \def\sectionmark##1{%
      \markboth {\MakeUppercase{%
        \ifnum \c@secnumdepth >\z@
          \thesection\quad
        \fi
        ##1}}{}}%
    \def\subsectionmark##1{%
      \markright {%
        \ifnum \c@secnumdepth >\@ne
          \thesubsection\quad
        \fi
        ##1}}}
%
\def\ps@plain{\let\@mkboth\@gobbletwo
     \let\@oddhead\@empty
% herrmann     \def\@oddfoot{\lower2pc\hbox{\fbox{\hbox to \hsize{\reset@font\rightnote\hfill {\bf \thepage}}}}}
% herrmann       % \def\@oddfoot{\centerline{\bf \thepage}}
\def\@oddfoot{\lower2pc\hbox{\hbox to \hsize{\reset@font\rightnote}}}

\let\@evenhead\@empty\let\@evenfoot\@oddfoot}
%
\def\ps@myheadings{%
    \let\@oddfoot\@empty\let\@evenfoot\@empty
    \def\@evenhead{\thepage\hfil\slshape\leftmark}%
    \def\@oddhead{{\slshape\rightmark}\hfil\thepage}%
    \let\@mkboth\@gobbletwo
    \let\sectionmark\@gobble
    \let\subsectionmark\@gobble
}%   
%

\newcommand\maketitle{\newpage
  \stepcounter{section}%
  \setcounter{section}{0}%
  \setcounter{subsection}{0}%
  \setcounter{figure}{0}
  \setcounter{table}{0}
  \setcounter{equation}{0}
  \setcounter{footnote}{0}%
  \begingroup
    \parindent=\z@
    \renewcommand\thefootnote{\@fnsymbol\c@footnote}%
    \if@twocolumn
      \ifnum \col@number=\@ne
        \@maketitle
      \else
        \twocolumn[\@maketitle]%
      \fi
    \else
      \newpage
      \global\@topnum\z@   % Prevents figures from going at top of page.
      \@maketitle
    \fi
%    \thispagestyle{empty}%
    \def\\{\unskip\ \ignorespaces}\def\inst##1{\unskip{}}%
    \def\fnmsep{\unskip}%
    \instindent=\hsize
    \advance\instindent by-\headlineindent
   
  \ifthenelse{\boolean{OptPoster}}
    {% 
      \ifnum\value{posternum}<42\relax
              \ifx\TITLETOC\empty {\addtocontents{toc}{\protect\contentsline{title}{\protect\parbox[t]{1cm}{\textbf{A\arabic{posternum}}\strut}\protect\parbox[t]{10cm}{\textbf{\@title}}\strut}{}}}\else%
    {\addtocontents{toc}{\protect\contentsline{title}{\protect\parbox[t]{1cm}{\textbf{A\arabic{posternum}}\strut}\protect\parbox[t]{10cm}{\textbf{\TITLETOC}\strut}}{}}}  
     \fi
        \addtocontents{toc}{\protect\vspace{-1.5ex}}
         \stepcounter{posternum}
              \else
          \ifx\TITLETOC\empty {\addtocontents{toc}{\protect\contentsline{title}{\protect\parbox[t]{1cm}{\textbf{B\arabic{posternumB}}\strut}\protect\parbox[t]{10cm}{\textbf{\@title}}\strut}{}}}\else%
    {\addtocontents{toc}{\protect\contentsline{title}{\protect\parbox[t]{1cm}{\textbf{B\arabic{posternumB}}\strut}\protect\parbox[t]{10cm}{\textbf{\TITLETOC}}\strut}{}}}  
     \fi     
      \addtocontents{toc}{\protect\vspace{-1.5ex}}
         \stepcounter{posternumB}

        \fi

    } %else 
     {
   \ifx\TITLETOC\empty {\addtocontents{toc}{\protect\contentsline{title}{\textbf{\@title}}{}}}\else%
    {\addtocontents{toc}{\protect\contentsline{title}{\textbf{\TITLETOC}}{}}}  
     \fi
   \addtocontents{toc}{\protect\vspace{-1.5ex}}
    }
 \def\and{\noexpand\protect\noexpand\and}%
      \protected@xdef\toc@uthor{\@authortbuffer}%
    \ifthenelse{\boolean{OptPoster}}
{ \ifx\AUTHORTOC\empty {\addtocontents{toc}{\protect\contentsline{title}{\hspace*{1cm}\protect\parbox[t]{10cm}{\itshape{\@authortbuffer}\strut}}{\thepage}} }\else%
{\addtocontents{toc}{\protect\contentsline{title}{\hspace*{1cm}\protect\parbox[t]{10cm}{\itshape{\AUTHORTOC}}\strut}{\thepage}}}%
  \fi%
  }
{ 
\ifx\AUTHORTOC\empty {\addtocontents{toc}{\protect\contentsline{title}{\itshape{\@authortbuffer}}{\thepage}}} \else%
{\addtocontents{toc}{\protect\contentsline{title}{\itshape{\AUTHORTOC}}{\thepage}}}%
\fi%
}
 \endgroup
  \setcounter{footnote}{0}%
   \global\let\@aabuffer\@empty
   \global\let\@authortbuffer\@empty
   \global\let\AUTHORTOC\@empty
   \global\let\TITLETOC\@empty
  }
%anfang
\def\l@title#1#2{\addpenalty{-\@highpenalty}
 \addvspace{8pt plus 1pt}
 \@tempdima \z@
 \begingroup
 \parindent \z@ \rightskip \@tocrmarg
 \parfillskip -\@tocrmarg
 \leavevmode \advance\leftskip\@tempdima \hskip -\leftskip
\nobreak #1 \nobreak
 \leaders\hbox{$\m@th \mkern \@dotsep mu.\mkern
  \@dotsep mu$}\hfill
 \nobreak\hbox to\@pnumwidth{\hss \bf #2}\par
 \penalty\@highpenalty \endgroup}
%ende
\def\title#1{\gdef\@title{#1}}
\def\@title{\@latex@error{No \expand\title  given}\@ehc}
%%
  \ifthenelse{\boolean{OptPoster}}
 {
  \setlength{\parindent}{0pt}
 }{}
\def\@maketitle{\newpage
 \markboth{}{}%

 \def\lastand{\ifnum\value{@inst}=2\relax
                 \unskip{} \andname\
              \else
                 \unskip \lastandname\
              \fi}%
 \def\and{\stepcounter{@auth}\relax
          \ifnum\value{@auth}=\value{@inst}%
             \lastand
          \else
             \unskip,
          \fi}%
 \begin{center}%
 {\Large \bfseries\boldmath
  \pretolerance=10000
  \@title \par}\vskip .8cm
 \setbox0=\vbox{\setcounter{@auth}{1}\def\and{\stepcounter{@auth}}%
 \@authortbuffer}%
 \global\value{@inst}=\value{@auth}%
 \global\value{auco}=\value{@auth}%
 \setcounter{@auth}{1}%
{\lineskip .5em
\noindent\ignorespaces
\bfseries \@authortbuffer  \vskip.05cm}
 {\small\institutename
}
\ifthenelse{\boolean{OptPoster}}
 {
\vskip.5cm
 }{}
 \end{center}%
 }
%  \vskip 1<.5em}
%
\def\@aabuffer{}
\def\@authortbuffer{}
\def\author#1{\expandafter\def\expandafter\@authortbuffer\expandafter
        {\@authortbuffer #1\relax
        \vspace*{2pt}}}%\vspace{0.75em}
%
 \def\lastand{\ifnum\value{auco}=2\relax
                 \unskip{} \andname\
              \else
                 \unskip \lastandname\
              \fi}%
 \def\and{\stepcounter{@auth}\relax
          \ifnum\value{@auth}=\value{auco}%
             \lastand
          \else
             \unskip,
          \fi}%
\newcounter{@inst}
\newcounter{@auth}
\newcounter{auco}
\def\andname{and}
\def\lastandname{\unskip, and}
\newdimen\instindent
\def\institute#1{\gdef\@institute{#1}}
\def\email#1{{\it E-mail: #1}}
\def\url#1{{\tt#1}}
\def\homedir{$\thicksim$}
\def\institutename{\par
 \begingroup
% \parskip=\z@
 \parskip=6\p@
 \parindent=\z@
 \setcounter{@inst}{1}%
 \def\and{\par\stepcounter{@inst}%
 \noindent$^{\the@inst}$\enspace\ignorespaces}%
 \setbox0=\vbox{\@institute}%
 \ifnum\c@@inst=1\relax
 \else
   \setcounter{footnote}{\c@@inst}%
   \setcounter{@inst}{1}%
   \noindent$^{\the@inst}$\enspace
 \fi
 \ignorespaces
 \@institute\par
 \endgroup}
\def\inst#1{\unskip$^{#1}$}


\setcounter{secnumdepth}{3}
\newcounter{section}
\newcounter{subsection}[section]
\newcounter {subsubsection}[subsection]
\renewcommand\thesection      {\@arabic\c@section}
\renewcommand\thesubsection   {\thesection.\@arabic\c@subsection}
\renewcommand\thesubsubsection{\thesubsection .\@arabic\c@subsubsection}

\setlength\leftmargini  {2em}
\leftmargin  \leftmargini
\setlength\leftmarginii  {2.2em}
\setlength\leftmarginiii {1.87em}
\setlength\leftmarginiv  {1.7em}
\setlength\leftmarginv  {.5em}
\setlength\leftmarginvi {.5em}
\setlength  \labelsep  {.5em}
\setlength  \labelwidth{\leftmargini}
\addtolength\labelwidth{-\labelsep}
\@beginparpenalty -\@lowpenalty
\@endparpenalty   -\@lowpenalty
\@itempenalty     -\@lowpenalty
\renewcommand\theenumi{\@arabic\c@enumi}
\renewcommand\theenumii{\@alph\c@enumii}
\renewcommand\theenumiii{\@roman\c@enumiii}
\renewcommand\theenumiv{\@Alph\c@enumiv}
\newcommand\labelenumi{\theenumi.}
\newcommand\labelenumii{(\theenumii)}
\newcommand\labelenumiii{\theenumiii.}
\newcommand\labelenumiv{\theenumiv.}
\renewcommand\p@enumii{\theenumi}
\renewcommand\p@enumiii{\theenumi(\theenumii)}
\renewcommand\p@enumiv{\p@enumiii\theenumiii}
\newcommand\labelitemi{$\m@th\bullet$}
\newcommand\labelitemii{\normalfont\bfseries --}
\newcommand\labelitemiii{$\m@th\ast$}
\newcommand\labelitemiv{$\m@th\cdot$}
%

\newenvironment{abstracts}{%
      \list{}{\advance\topsep by0.35cm\relax\footnotesize
      \leftmargin=1cm
      \labelwidth=\z@
      \listparindent=\z@
      \itemindent\listparindent
      \rightmargin\leftmargin}\item}%[\hskip\labelsep
                                    %\bfseries\abstractname]}
 {\endlist}
\newenvironment{quotation}
               {\list{}{\listparindent 1.5em%
                        \itemindent    \listparindent
                        \rightmargin   \leftmargin
                        \parsep        \z@ \@plus\p@}%
                \item\relax}
               {\endlist}
%
\newenvironment{quote}
               {\list{}{\rightmargin\leftmargin}%
                \item\relax}
               {\endlist}
%
%FOLLOWING THREE COMMANDS ARE FOR `LIST' COMMAND.
\topsep=0in
\parsep=0in
\itemsep=0in
%
\newcounter{arabiclistc}
\newenvironment{arabiclist}
        {\setcounter{arabiclistc}{0}
         \begin{list}{\arabic{arabiclistc}}
        {\usecounter{arabiclistc}
         \setlength{\parsep}{0pt}
         \setlength{\itemsep}{0pt}}}{\end{list}}



\newcommand\appendix{\par
  \setcounter{section}{0}%
  \setcounter{subsection}{0}%
  \renewcommand\thesection{\@Alph\c@section}
        \section*{\appendixname}}
%
\setlength\arraycolsep{1.5\p@}%5pt
\setlength\tabcolsep{6\p@}
\setlength\arrayrulewidth{.4\p@}
\setlength\doublerulesep{2\p@}
\setlength\tabbingsep{\labelsep}
\skip\@mpfootins = \skip\footins
\setlength\fboxsep{3\p@}
\setlength\fboxrule{.4\p@}
\renewcommand\theequation{\@arabic\c@equation}
%%
\InputIfFileExists{psfig.sty}{}{}
\InputIfFileExists{epsf.sty}{}{}
\InputIfFileExists{epsfsafe.tex}{}{}
\InputIfFileExists{epsfig.sty}{}{}
%
\newcounter{figure}
\renewcommand\thefigure{\@arabic\c@figure}
\def\fps@figure{tbp}
\def\ftype@figure{1}
\def\ext@figure{lof}
\def\fnum@figure{\figurename~\thefigure}
\newenvironment{figure}
               {\@float{figure}}
               {\end@float}
\newenvironment{figure*}
               {\@dblfloat{figure}}
               {\end@dblfloat}
%
\newcounter{table}
\renewcommand\thetable{\@arabic\c@table}
\def\fps@table{tbp}
\def\ftype@table{2}
\def\ext@table{lot}
\def\fnum@table{\tablename~\thetable}
\newenvironment{table}
               {\@float{table}}
               {\end@float}
\newenvironment{table*}
               {\@dblfloat{table}}
               {\end@dblfloat}
%
\newlength\abovecaptionskip
\newlength\belowcaptionskip
\setlength\abovecaptionskip{10\p@}
\setlength\belowcaptionskip{6\p@}
\def\captionfont{\normalfont\footnotesize{}}
\long\def\@makecaption#1#2{%
        \captionfont
 \vskip\abovecaptionskip
  \sbox\@tempboxa{#1.\hskip.5em#2}%
  \ifdim \wd\@tempboxa >\hsize
    \noindent #1.\hskip.5em#2\par
  \else
    \global \@minipagefalse
    \hb@xt@\hsize{\hfil\box\@tempboxa\hfil}%
  \fi
  \vskip\belowcaptionskip}
%
\def\figurebox#1#2#3{%
        \def\arg{#3}%
        \ifx\arg\empty
        {\hfill\vbox{\hsize#2\hrule\hbox to #2{\vrule\hfill\vbox to #1{\hsize#2\vfill}\vrule}\hrule}\hfill}%
        \else
        {\hfill\epsfbox{#3}\hfill}% 
        \fi}
%
\def\@figurecaption#1#2{\unskip\vskip10pt{#1.\hskip.5em#2\par}}
%
\DeclareOldFontCommand{\rm}{\normalfont\rmfamily}{\mathrm}
\DeclareOldFontCommand{\sf}{\normalfont\sffamily}{\mathsf}
\DeclareOldFontCommand{\tt}{\normalfont\ttfamily}{\mathtt}
\DeclareOldFontCommand{\bf}{\normalfont\bfseries}{\mathbf}
\DeclareOldFontCommand{\it}{\normalfont\itshape}{\mathit}
\DeclareOldFontCommand{\sl}{\normalfont\slshape}{\@nomath\sl}
\DeclareOldFontCommand{\sc}{\normalfont\scshape}{\@nomath\sc}
\DeclareRobustCommand*\cal{\@fontswitch\relax\mathcal}
\DeclareRobustCommand*\mit{\@fontswitch\relax\mathnormal}

\newcommand\@pnumwidth{1.55em}
\newcommand\@tocrmarg{2.55em}
\newcommand\@dotsep{200}
%
\newenvironment{thebibliography}[1]
        {\section*{{\large\refname}}
        \begin{list}{\arabic{enumi}.}
        {\usecounter{enumi}\setlength{\parsep}{0pt}
         \setlength{\itemsep}{0pt} \settowidth
        {\labelwidth}{#1.}\sloppy} 
        % M.Marx Okt. 2001: zusaetzlicher Abstand innerhalb eines bibitems
        \newcommand\newblock{\hskip .11em\@plus.33em\@minus.07em} 
        }{\end{list}}
%%
\newenvironment{theindex}
               {\if@twocolumn
                  \@restonecolfalse
                \else
                  \@restonecoltrue
                \fi
                \columnseprule \z@
                \columnsep 35\p@
                \twocolumn[\section*{\indexname}]%
                \@mkboth{\MakeUppercase\indexname}%
                        {\MakeUppercase\indexname}%
                \thispagestyle{plain}\parindent\z@
                \parskip\z@ \@plus .3\p@\relax
                \let\item\@idxitem}
               {\if@restonecol\onecolumn\else\clearpage\fi}
\newcommand\@idxitem{\par\hangindent 40\p@}
\newcommand\subitem{\@idxitem \hspace*{20\p@}}
\newcommand\subsubitem{\@idxitem \hspace*{30\p@}}
\newcommand\indexspace{\par \vskip 10\p@ \@plus5\p@ \@minus3\p@\relax}
%
\renewcommand\footnoterule{%
  \kern-3\p@
  \hrule\@width.4\columnwidth
  \kern2.6\p@}
\newcommand\@makefntext[1]{%
    \parindent 1em%
    \noindent
     \@makefnmark#1}
%
\def\thefootnote{\alph{footnote}}
\def\@makefnmark{{$^{\@thefnmark}$}}
%
\newcommand\contentsname{Contents}
\newcommand\listfigurename{List of Figures}
\newcommand\listtablename{List of Tables}
\newcommand\refname{References}
\newcommand\indexname{Index}
\newcommand\figurename{Figure}
\newcommand\tablename{Table}
\newcommand\partname{Part}
\newcommand\appendixname{Appendix}
\newcommand\abstractname{Abstract}
%
\def\rightnote{\makebox[360pt][t]{{\hfill{\bf \thepage}\hfill}}}

%
%%%%%%%%%%%%%%%%%%%%%%%%%%%% definitions retained from the latex2.09 file %%%%%%%%
%
\bibliographystyle{unsrt}    % for BibTeX - sorted numerical labels by order of
                             % first citation.
\arraycolsep1.5pt
% A useful Journal macro
\def\Journal#1#2#3#4{{#1} {\bf #2}, #3 (#4)}

% Some useful journal names
\def\NCA{Nuovo Cimento}
\def\NIM{Nucl. Instrum. Methods}
\def\NIMA{Nucl. Instrum. Methods A}
\def\NPB{Nucl. Phys. B}
\def\PLB{Phys. Lett. B}
\def\PRL{Phys. Rev. Lett.}
\def\PRD{Phys. Rev. D}
\def\ZPC{Z. Phys. C}
% Some other macros used in the sample text
\def\st{\scriptstyle}
\def\sst{\scriptscriptstyle}
\def\mco{\multicolumn}
\def\epp{\epsilon^{\prime}}
\def\vep{\varepsilon}
\def\ra{\rightarrow}
\def\ppg{\pi^+\pi^-\gamma}
\def\vp{{\bf p}}
\def\ko{K^0}
\def\kb{\bar{K^0}}
\def\al{\alpha}
\def\ab{\bar{\alpha}}
\def\be{\begin{equation}}
\def\ee{\end{equation}}
\def\bea{\begin{eqnarray}}
\def\eea{\end{eqnarray}}
\def\CPbar{\hbox{{\rm CP}\hskip-1.80em{/}}}%temp replacement due to no font
%
\arraycolsep1.5pt
%
%%%%%%%%%%%%%%%%
%
\newcommand\today{}
\edef\today{\ifcase\month\or
  January\or February\or March\or April\or May\or June\or
  July\or August\or September\or October\or November\or December\fi
  \space\number\day, \number\year}
\setlength\columnsep{0.25in}
\setlength\columnseprule{0\p@}
\pagestyle{plain}
\pagenumbering{arabic}
\sloppy
\onecolumn
\flushbottom
\endinput
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%End %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%




