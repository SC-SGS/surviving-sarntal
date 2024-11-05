# Hinweise zum Template

## Dokumentenklassen
Dieses Template ist sowohl für das Format 16:9 wie auch für das Format 4:3 verwendbar.
Beispiel für 16:9:
```latex
\documentclass[8pt, t,aspectratio=169]{beamer}
```
Beispiel für 4:3:
```latex
\documentclass[8pt, t,aspectratio=43]{beamer}
```

## Optionen
Zunächst muss das entsprechende .sty-file korrekt eingebunden werden:
```latex
\usepackage{style/beamerthemeipvs}
```
Optionen können wie folgt verwendet werden:
```latex
\usepackage[option1,option2]{style/beamerthemeipvs}
```
Verfügbare Optionen:
- `german`: Muss gesetzt werden für die Sprache Deutsch, ansonsten wird als Sprache Englisch verwendet
- `unilogo`: Muss gesetzt werden, damit auf jeder Folie rechts oben das Logo der Uni Stuttgart erscheint
- `guestlogo`: Muss gesetzt werden, wenn neben dem Logo der Uni Stuttgart auch ein fremdes Logo (z.B. bei einem Gastvortrag) verwendet werden soll. Es muss dann eine entsprechende `guest`-Datei im Directory vorhanden sein.
- `department`: Muss gesetzt werden, damit auf der Titelfolie sowie den Endfolien das Logo der jeweiligen Abteilung angezeigt wird. Hierzu muss `\pgfdeclareimage[width=2.5cm]{department}{*file*}` entsprechend angepasst sein.
- `depunilogo`: Muss gesetzt werden, damit neben dem Logo der Uni Stuttgart auch das der jeweiligen Abteilung angezeigt wird. Hierzu muss `\pgfdeclareimage[width=2.5cm]{department}{*file*}` entsprechend angepasst sein.

## Sections
Es gibt sowohl nummerierte Sections wie auch nicht nummerierte Sections.
- Nummerierte Sections können wie folgt erzeugt werden: `\makesection{Section with enumeration}`
- Nicht nummerierte Sections können wie folgt erzeugt werden: `\makesectionwithoutenumeration{Section without enumeration}`
Eine Verlinkung geschieht nur für die Sections mit Nummerierung.

## Itemize
Es sind drei Ebenen verfügbar:
1. Blaue Punkte
2. Graue Punkte
3. Graue Striche

Selbstvertändlich können auch eigene Argumente verwendet werden, die dann in der jeweilligen Farbe der Ebene erscheinen.

Beispiel:
```latex
\begin{frame}{Itemize}
	\begin{itemize}
		\item Item 1
		\item Item 2
		\begin{itemize}
			\item Item 2.1
			\item Item 2.2
			\begin{itemize}
				\item Item 2.2.1
				\item Item 2.2.2
				\item[$\rightarrow$] 2.2.3
			\end{itemize}
		\end{itemize}
		\item[$\rightarrow$] Item 3
	\end{itemize}
\end{frame}
```

## Enumterate
Es sind drei Ebenen verfügbar:
1. Arabische Nummerierung, weiße Zahlen mit blauem Kreis umgeben
2. Graue römische Nummerierung
3. Graue alphabetische Nummerierung

Selbstvertändlich können auch eigene Argumente verwendet werden, die dann in der jeweilligen Farbe der Ebene erscheinen.

Beispiel:
```latex
\begin{frame}{Enumeration}
	\begin{enumerate}
		\item Item 1
		\item Item 2
		\begin{enumerate}
			\item Item 2.1
			\item Item 2.2
			\begin{enumerate}
				\item Item 2.2.1
				\item Item 2.2.2
				\item[$\rightarrow$] 2.2.3
			\end{enumerate}
		\end{enumerate}
		\item[$\rightarrow$] Item 3
  \end{enumerate}
\end{frame}
```

## Hilfslinien
Um die Positionierung von Objekten zu erleichtern, können Hilfelinien verwendet werden. Diese können mit `\setgrid` ein und mit `\turngridoff` ausgeschaltet werden. 

Beispeil:
```latex
\setgrid
\begin{frame}{Title}
	Content
\end{frame}
\turngridoff
```

## Thank You
Dies kann als abschließende Folie verwendet werden. Als Argumente werden die E-Mail, Telefon sowie Fax übernommen. Außerdem muss ein entsprechendes Bild des Vortragenden existieren, das mit `\pgfdeclareimage[width=2.5cm]{speaker}{*file*}` deklariert wurde.

Beispiel:

```latex
\thankyou{dirk.pflueger@ipvs.uni-stuttgart.de}{+49 711 685 88447}{https://www.ipvs.uni-stuttgart.de}
```

## Thanks To
Hiermit kann Personen gedankt werden, welche an dem Vortrag mitgewirkt haben. Dies ist für 1-4 Personen möglich. Die entsprechenden Bilder der Personen müssen hierfür zuvor mit `\pgfdeclareimage[width=2.5cm]{thankstox}{*file*}` deklariert worden sein. Als Argumente werden die Namen sowie ein Text übergeben, der unten zentral auf der Folie erscheint.
Beispiel:

```latex
\thankstothree{Person 1}{Person 2}{Person 3}{Irgendein Text}
```
