 /*
	 +*****************************************************************+
   +    Xcref.cpp et Xcref0.h references croisees des variables
	 +    sur plusieurs fichiers d un projet . configure pour c /c++
	 +    progrmmation fonctionnelle vieux c . le handler de fichier
	 +    est trimbale across toutes les fonctions
	 +    Bug maajeur corrige et rajout de release memoire
	 +
	 +    2015-09-22-09.46
	 +*****************************************************************+

*/


#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h>
#include <iostream>
#include <cstring>
#include <string>
#include <windows.h>

extern struct BinaryTree
{
   char * name;
   struct heap *line;
   struct BinaryTree *left;
   struct BinaryTree *right;
}
*tree = NULL;

extern struct heap
{
   short num;
   char spec;
   struct heap *next;
   char * fileName;
   char toto[100];
}*tref = NULL;


class pubVar
{
public:
   FILE *fp;
   int toto;
   int  xpause();
};

int openModel();
void aide();
int listIDs (struct BinaryTree *p,FILE *lfp,char *fl) ;
struct BinaryTree *CloseDataStruct (struct BinaryTree *p);
int isC(char CH);
int SkipBlanks (FILE *fp);          /* Skip irrelevant characters */
int RecordWord (FILE *fp);  /* get ID in buffer & tube it to data */
int  ParticularSkip (FILE *fp);   /* handle particular characters */
int CopyNextID (FILE *fp);    /* Put next identifier into BUFFER */
int token ();                    /* Token: pos in WORDTABLE */
int RecordUserID (int fnflag); /* check ID type & install data */
int isfunction (FILE *fp);          /* returns TRUE if ID is a fn */
int deleteheap (struct heap *h);     /* Release back to free memory pool */
int skipcomment (FILE *fp);         /* skip to char after comment */

int skiptochar (FILE *fp,char ch);           /* skip to char after ch */
struct BinaryTree *install (struct BinaryTree *p);  /* install ID in tree */
struct heap *pushonheap (struct heap *h);  /* push nxt ln no.to heap */

int backone ( char ch,FILE *fp);              /* backspace one in file */
char *filename ();
int skipgarb ();          /* skip garbage upto end of line */
char *stringin (char *array);    /* cpy str in arry to ptr loc*/
char *sout(char *);

struct heap *newheap ();
struct BinaryTree *newtree ();
int releaseheap (struct heap *ptr);
int releasetree (struct heap *ptr);

int RecordUserID (int fnflag);
int isfunction (FILE *fp);

int skipcomment (FILE *fp);
int skiptochar (FILE *fp,char ch);
int  releasetree (struct BinaryTree *ptr);
int  releaseheap (struct heap *ptr);

int  entete(FILE *);
char *xgetline(FILE *);

