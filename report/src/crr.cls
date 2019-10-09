%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% crr.cls
%
% Modèle de compte-rendu de réunion
% v0.1, 2 février 2019
% v0.2, 14 mars 2019
% v0.3, 9 octobre 2019
% Écrit par Louis Jacotot
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%% Entête
% Définition du format attendu
\NeedsTeXFormat{LaTeX2e}
% Définition de la classe
\ProvidesClass{crr}[14/03/2019 Compte-rendu de réunion, v0.2]
% Chargement de la classe de base
\LoadClass[a4paper, 10pt]{article}

%% Chargement des paquets supplémentaires
% Chargement des paquets usuels
\RequirePackage[french]{babel}
\RequirePackage[sfdefault]{FiraSans}
\RequirePackage{FiraMono}
\RequirePackage{geometry}
\RequirePackage{changepage}
\RequirePackage{environ}
\RequirePackage{enumitem}
\RequirePackage{lmodern}
\RequirePackage[normalem]{ulem}
\RequirePackage{xcolor}
\RequirePackage{graphicx}
\RequirePackage{fontspec}
\RequirePackage{ifthen}


%% Chargement des polices «FontAwesome»
\defaultfontfeatures{Extension = .otf} % Nécessaire avec XeTeX
\RequirePackage{fontawesome}

%% Définition des variables
\newcommand{\entite}[1]{\def\@entite{#1}}
\newcommand{\equipe}[1]{\def\@equipe{#1}}
\newcommand{\projet}[1]{\def\@projet{#1}}
\renewcommand{\date}[1]{\def\@date{#1}}
\newcommand{\lieu}[1]{\def\@lieu{#1}}
\newcommand{\heure}[1]{\def\@heure{#1}}
\newcommand{\duree}[1]{\def\@duree{#1}}
\newcommand{\motif}[1]{\def\@motif{#1}}

%% Définition de la géométrie
\geometry{
    left=2cm,
    right=8cm,
    marginparwidth=4.8cm,
    marginparsep=1.2cm,
    top=2cm,
    bottom=2cm
}

%% Définition du style des listes à puces
\setlist{
    leftmargin=*,
    labelsep=0.5em,
    nosep,
    itemsep=0.25\baselineskip,
    after=\vskip0.25\baselineskip
}
\setlist[itemize]{label=\small\textbullet}

%% Définition des couleurs
\definecolor{Accent}{HTML}{000080}
\colorlet{accent}{Accent}

%% Définition des méthodes
\newcommand{\important}[1]{{\bfseries\color{accent}#1}}

\renewcommand{\section}[1]{
    \bigskip
    {\color{accent}\bfseries\MakeUppercase{#1}}\\[-1ex]
    {\color{accent}\rule{\linewidth}{1pt}\par}\medskip
}

\renewcommand{\subsection}[1]{
    \smallskip
    {\bfseries{#1}\par}\medskip
}

\newcommand{\element}[2]{
    \mbox{\textcolor{accent}{\normalfont #1}\hspace
    {0.5em}#2\hspace{2em}}
}

\newcommand{\ajouter}[1]{\item #1}

\newcommand{\entete}{
    \begin{adjustwidth}{}{\dimexpr-\marginparwidth-\marginparsep\relax}
    \begingroup
        \begin{flushright}
            {\small\MakeUppercase{\@entite~-~\@equipe}\par}
        \end{flushright}
        \medskip
        \begin{center}
            {\Large\bfseries\color{accent}
            \uline{Projet~\@projet}\par
            \medskip
            \uline{Compte-rendu de réunion}}
        \end{center}
        \bigskip
        \begin{center}
            \element{\faCalendar}{\@date}
            \element{\faMapMarker}{\@lieu}\par
            \element{\faClockO}{\@heure}
            \element{\faHourglassHalf}{\@duree h}\par
            \medskip
            {\large\element{\faBookmark}{\@motif}}
            \par
        \end{center}
    \endgroup\medskip
    \end{adjustwidth}
    \bigskip
}

\newcommand{\present}[2][]{
    \ifthenelse{\equal{#1}{redacteur}}{
        \quad\element{\faPencil}{#2}\par
    }{
        \quad\element{\faUser}{#2}\par
    }
}
\newcommand{\excuse}[1]{\quad\element{\faUserTimes}{#1}\par}
\newcommand{\nonexcuse}[1]{
    \quad\element{\faWarning}{#1}\par
}
\newcommand{\approbation}{
    \section{\faInfo\quad Approbation}
    En l'absence de remarque, ce compte-rendu sera considéré comme
    approuvé le \important{septième jour suivant le \@date}.
}

%% Définition des «environs»
\NewEnviron{contexte}{
    \marginpar{\vspace*{\dimexpr1pt-\baselineskip}
    \raggedright\bigskip\BODY}
}

%% Définition des environnements
\newenvironment{reunion}{
    \document
    \entete
}{\enddocument}

\newenvironment{participants}{\section{\faUsers\quad Participants}}{}

\newenvironment{presents}{\subsection{Présents}}{}
\newenvironment{excuses}{\subsection{Excusés}}{}
\newenvironment{nonexcuses}{\subsection{Non excusés}}{}

\newenvironment{ordredujour}{
    \section{\faBriefcase\quad Ordre du jour}
    \begin{itemize}
}{\end{itemize}}

\newenvironment{echanges}{
    \section{\faComments\quad Informations échangées}
    \begin{itemize}
}{\end{itemize}}

\newenvironment{decisions}{
    \section{\faLegal\quad Décisions}
    \begin{itemize}
}{\end{itemize}}

%% Autre
\AtBeginDocument{
    \pagestyle{empty}
    \raggedright
}
