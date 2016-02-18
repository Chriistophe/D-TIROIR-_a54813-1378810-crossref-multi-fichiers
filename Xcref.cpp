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
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <conio.h>
#include <iostream>
#include <cstring>
#include <string>
#include "Xcref0.h"

#define TRUE        1
#define FALSE       0
#define DUMMY       0
#define MAXSTR      800  //int max d une chaine (sring)
#define MAXIDSIZE   256  // int max dun token du texte (pas langage)
#define ALLOCMEM(size)	VirtualAlloc(0, size, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE)
#define nbWord      45   //  nb de token du langage
#define FREEMEM(x)		VirtualFree(x, 0, MEM_RELEASE)

HWND hWnd;
FILE *fp2,*fp3;

char zoul[80];

int yy=0;
char titi[100][100];  // tableau 100*100 des nom_fichier*longueur
int mst=0;
int   LINECOUNT = 1;     /* Contains line no. in file */
char  BUFFER[MAXIDSIZE]; /* Input buffer token texte (pas langage)    */
char  CH;                /* Current input character   */
char  SPECIALCHAR;       /* macro/pointer flag        */
const int MAX_PATH_Tof=11001;   // buffer size mult select cfiledialog chem 1000 0 1 100*100 file
char *WORDTABLE [nbWord] =
{
   "auto"    ,	"break"   ,	"case"    ,	"char"    ,	"const"   ,	"continue",	"default" ,
   "do"      ,	"double"  ,	"else"    ,	"entry"   ,	"enum"    ,	"extern"  ,
   "float"   ,	"for"     ,	"goto"    ,	"if"      ,	"int"     ,	"long"    ,	"register",
   "return"  ,	"short"   ,	"signed"  ,	"sizeof"  ,	"static"  ,	"struct"  ,	"switch"  ,
   "typedef" ,	"union"   ,	"unsigned",	"void"    ,	"volatile",	"while"   ,	"Elisabeth",
   "0","1","2","3","4","5","6","7","8","9","²"
};

using namespace std;  // when use c++ namespace std

main ()
{
   int yy=0;
   int lg2=0;
   char *ecri=new char[256];

   aide();
   // _getch();

   if(openModel()==0)return(0); // escape sur selecteur-> sortie

   strcpy(ecri,"XcRef");   // fichier output nom genere ici
   strcat(ecri,".out");
   fp3 = fopen (ecri,"w+");    // creature ou ecrature
   entete(fp3);

   mst=mst-1;

   for (yy=1; (yy<=mst) && titi[yy]!="" ; ++yy)
   {
      if( strlen(titi[yy])<=1)goto ext;

      //printf(">Numeral %03d next readed file %25s...\n \n",yy,titi[yy]);

      fp2 = fopen (titi[yy],"r") ;
      strcpy(zoul,titi[yy]);

      CH = getc(fp2);

      while (!feof(fp2))   //r0
      {
         SkipBlanks (fp2);  //r1
         RecordWord (fp2);
      }
      LINECOUNT =1;
      lg2=ftell(fp2);
   }
ext:

   listIDs (tree,fp3,"                     ");
   CloseDataStruct(tree);
}

// -------------------- phase 1 rajout crlf ----------------------
int SkipBlanks (FILE *fp)  //r1        /* Skip irrelevant characters */
{
   while (!feof(fp))
   {
      if (isC(CH))
      {
         return(0);
      }
      ParticularSkip(fp);
   }
}

// ------------------------------------------------------
int isC(char CH)
{
   if (((CH >= 'A') && (CH <= 'Z')) ||
         ((CH >= 'a') && (CH <= 'z')) ||
         ((CH >= '0') && (CH <= '9' ))||
         (CH == '_' ) )
   {
      return(1);
   }
   return(0);
}

// -----------------------------------------------------
int ParticularSkip (FILE *fp)   /* handle particular characters */
{
   char c=0;
   switch (CH)
   {
   case '/' :
      c = getc(fp);

      if (c == '*')skipcomment (fp);
      if (c == '/')skipcomment (fp);

      CH = c;

      return (DUMMY);
      break;

   case '"' :
      if (skiptochar (fp,'"') > MAXSTR)
      {
         printf ("Chaine trop longue. Modif constante ou rajout cr.lf avant ");
         printf ("line %d\n",LINECOUNT);
         return (0);
      }
      break;

   case '\'':
      if (skiptochar (fp,'\'') == 1)
      {
         if (CH=='\'') CH = getc(fp);
      }
      break;

   case '#' :
      c= skiptochar(fp,' ');

      SPECIALCHAR = '#';
      break;

   case '\n':
      ++LINECOUNT;
   default  :
      CH = getc(fp);
      SPECIALCHAR = ' ';
   }
}

// -----------------------------------------------------
int skipcomment (FILE *fp)         /* skip to char after comment */
{
   char cs = '/255';
   char clo[100]="";
   int n=0;

   for (CH = getc(fp); !feof(fp); CH = getc(fp))
   {
      clo[n++]=CH;
      switch (CH)
      {
      case '\n' :
         ++LINECOUNT;
         return(DUMMY);
         break;

      case '/' :
         if (cs == '*')
         {
            return(DUMMY);
         }
      }
      cs = CH;
   }
}

// -----------------------------------------------------
int skiptochar (FILE *fp,char ch)           /* skip to char after ch */
{
   int c=0;

   while (((CH =getc(fp)) != ch) && !feof(fp))
   {
      if (CH == '\n')
      {
         ++LINECOUNT;
      }
      c++;
   }
   CH = getc(fp);
   return (c);
}

// ------------------------------------------------------
int RecordWord (FILE *fp)  /* get ID in buffer & tube it to data */
{
   int tok;
   CopyNextID (fp);
  // printf("RecordWord %s \n",BUFFER);
   if ((tok = token()) == 0)       /* if not resved word */
   {
      RecordUserID(isfunction(fp));
   }
   if ((tok = token()) == 1)       /* if not resved word */
   {
      RecordUserID(isfunction(fp));
   }
   SPECIALCHAR = ' ';
   return 0;
}

// -----------------------------------------------------
int CopyNextID (FILE *fp)    /* Put next identifier into BUFFER */
{
   int i = 0;

   while (!feof(fp) && (isC(CH)))
   {
      BUFFER[i++] = CH;
      CH = getc (fp);
   }
   BUFFER[i] = '\0';
   return 0;
}

// ------------------------------------------------------
int token ()                    /* Token: pos in WORDTABLE */
{
   int i;
   for (i = 0; i < nbWord; i++)
   {
      if (strcmp(&(BUFFER[0]),WORDTABLE[i]) == 0)
      {
         return(i);
      }
   }
   return(0);
}

// -----------------------------------------------------
int RecordUserID (int fnflag) /* check ID type & install data */
{
   if (fnflag)
   {
      strcat (BUFFER,"()");
      tree = install (tree);
   }
   else
   {
      tree = install (tree);
   }
return 0;
}
// ------------------------------------------------------
int isfunction (FILE *fp)          /* returns TRUE if ID is a fn */
{
   while(!feof(fp))
   {
      if (!(CH == ' ' || CH == '\n'))
      {
         break;
      }
      else if (CH == '\n')
      {
         ++LINECOUNT;
      }
      CH = getc(fp);
   }

   if (CH == '(')
   {
      return (TRUE);
   }
   else
   {
      return (FALSE);
   }
   return 0;
}
// ------------------------------------------------------
int deleteheap (struct heap *h)     /* Release back to free memory pool */
{
   struct heap *temp = h;
   while (h!=NULL && temp!=NULL)
   {
      temp = h->next;
      releaseheap(h);
      h = temp;
   }
   return 0;
}
// -----------------------------------------------------
struct BinaryTree *install (struct BinaryTree *p)  /* install ID in tree */
{
   int pos;
   if (p == NULL)                             /* new word */
   {
      p = newtree();//ALLOCMEM binarytree
      p->name = stringin(BUFFER);
      p->line = pushonheap (NULL);
      p->left = NULL;
      p->right = NULL;
      return (p);
   }
   if ((pos = strcmp (BUFFER,p->name)) == 0) /* found word*/
   {
      p->line = pushonheap(p->line);
      return (p);
   }
   if (pos < 0)                        /* Trace down list */
   {
      p->left = install(p->left);
   }
   else
   {
      p->right = install(p->right);
   }
   return (p);
}
// -----------------------------------------------------
struct heap *pushonheap (struct heap *h)  /* push nxt ln no.to heap */
{
   struct heap *hp;

   hp = newheap();
   hp->num  = LINECOUNT;
   hp->spec = SPECIALCHAR;
   hp->fileName =zoul;

   int i;
   char *ptr;

   ptr = (char*)malloc (strlen(zoul)+1);
   for (i = 0; zoul[i] != '\0'; hp->toto[i] = zoul[i++]);

   hp->toto[i] = '\0';
   hp->next = h;
   return (hp);
}

// ------------------------------------------------------
struct heap *newheap ()
{

   return ((struct heap *) ALLOCMEM (sizeof(struct heap)+1));
}

// ------------------------------------------------------
struct BinaryTree *newtree ()
{
   return ((struct BinaryTree *)ALLOCMEM(sizeof(struct BinaryTree)+1));
}

// ------------------------------------------------------
int backone (char ch,FILE *fp)              /* backspace one in file */
{
   if (ungetc(ch,fp) != ch)
   {
      return(0);
   }
}

// ------------------------------------------------------
int skipgarb ()          /* skip garbage upto end of line */
{
   while (getchar() != '\n');
   return 0;
}

// ------------------------------------------------------
char *stringin (char *array)    /* cpy str in arry to ptr loc*/
{
   char *ptr;
   int i;
   ptr = (char*)ALLOCMEM(strlen(array)+1);
   for (i = 0; array[i] != '\0'; ptr[i] = array[i++]);
   ptr[i] = '\0';
   return(ptr);
}

// ------------------------------------------------------
int releaseheap (struct heap *ptr)
{
   {
   //  printf("heap %d\n",*ptr);
      FREEMEM(ptr);
   }
   return 0;
}

// ------------------------------------------------------
int releasetree (struct BinaryTree *ptr)

{
   {
     //printf("tree %d\n",*ptr);
      FREEMEM(ptr);
   }
   return 0;

}

int entete(FILE *lfp)
{
   fprintf (lfp," \n ");
   fprintf (lfp," Fichier de references croisees genere par XcRef. A gauche nom var \n");
   fprintf (lfp," En dessous: le ou les noms des fichiers ou se trouve cette var    \n");
   fprintf (lfp," suivi des numeros de ligne (naturel) des implantations. Plusieurs \n");
   fprintf (lfp," numero identiques, plusieurs fois le mot cle dans la ligne.       \n");
   fprintf (lfp," Pour debugger des projets multi sources, je suis le champion.     \n");
   fprintf (lfp,"                       ccgousset@gmail.com 2013 & sept 2015 (c)  \n\n");
   return 0;
}

void aide()
{
   printf("\n  XcRef genere les refs croisees sur plusieurs sources \n");
   printf("  Selectionnez les fichiers .txt,.cpp ,.h ou tous autres \n");
   printf("  fichiers texte a analyser. XcRef creera le fichier.out  \n");
   printf("  contenant les variables ou references les fichiers mere \n ");
   printf("  et les numero de lignes ou se retrouvent les mots cle.  \n");
   printf("  Limite de traitement a peu pres 1000 mots differents.   \n ");
}

int openModel()
{
   int cpt_i=0;
   OPENFILENAME ofn;
   CHAR szFile[MAX_PATH_Tof]= {0};       // retient son nom complet : nomModel
   ZeroMemory(&ofn, sizeof(OPENFILENAME));
   ofn.lStructSize = sizeof(OPENFILENAME);
   ofn.hwndOwner = hWnd;
   ofn.lpstrFile = szFile;   //LPTSTR  to buffer to string on drive path and files for   multiselect case
   ofn.nMaxFile = 64786;//MAX_PATH;
   ofn.lpstrTitle=" Selectionnez les fichiers a traiter (touche Ctrl enfoncee)     Ver 2.0 (c) ccgousset@gmail.com 2015 ";
   ofn.lpstrFilter = "Fichier a analyser  (*.*)\0*.*";
   ofn.nFilterIndex = 1;
   ofn.Flags =OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY|OFN_ALLOWMULTISELECT|OFN_EXPLORER;
   GetOpenFileName(&ofn);
  //printf("   nFileOffset %d \n",ofn.nFileOffset);

   char tmpa[5000][50];
   int nb_u=0;
   int flag=0;
   int flagout=0;
   char toto[80];
   char tata[80];
   char zero[80];
   int u=0;

   int kk=0;

   for ( u=0  ; u< MAX_PATH_Tof && flagout !=1; u++)
   {
      if (szFile[u]==255) szFile[u]=69;

      if ((szFile[u]=='\0'))
      {
         tmpa[cpt_i][nb_u]='\0';
         nb_u=0;
         cpt_i++;
         if (flag==0) flagout=1;
         flag=0;
         toto[kk++]='\0';
         strcpy(titi[cpt_i-1],toto);
         kk=0;
      }
      if (szFile[u]!='\0' && flagout !=1)
      {
         tmpa[cpt_i][nb_u]=szFile[u];
         toto[kk++]=szFile[u];
         nb_u++;
         flag=1;
      }
   }

   if (cpt_i==2)
   {
      yy=1;
      mst=2;
      strcpy(titi[1],szFile);
      return ofn.nFileOffset;          // sortie premat telemetrie == un;
   }

   for (u=0  ; u< cpt_i-1 ; u++)
   {
      {
         if (u==0)
            strcpy(zero,titi[u]);
      }
      if (u!=0)
      {
         strcpy(tata,zero);
         strcat(tata,"\\");
         strcat(tata,titi[u]);
      }
   }

   printf("\n");
   mst=cpt_i;
   return ofn.nFileOffset;
}

/* ------------------------------------------------------ */

int listIDs (struct BinaryTree *p,FILE *lfp, char *fl)                       /* List Binary Tree */
{
   static int tof;
   struct heap *h;
   int i = 0;
   char flag[256];
   char *rese="     ";

   if (p != NULL)
   {
      strcpy(flag,"");
      listIDs (p->left,lfp,flag);
      fprintf (lfp,"\n%-25s",p->name );
      printf ("\n%-25s",p->name);

      for (h = p->line; (h != NULL); h = h->next)
      {
         if (strcmp(flag,h->toto)!=0)

         {

            printf ("\n");
            fprintf (lfp,"\n");

            printf ("%s" ,rese);
            fprintf (lfp,"%s",rese);
            printf ("%-30s",h->toto);
            fprintf (lfp,"%-30s ",h->toto);

            printf ("\n         ");
            fprintf (lfp,"\n         ");

            strcpy(flag,h->toto);
         }

         printf ("%-5d ",h->num);
         fprintf    (lfp,"%-5d ",h->num);

         if ((++i % 10) == 0)
         {

            printf ("\n         ");
            fprintf (lfp,"\n         ");
         }
      }
      printf ("\n");
      fprintf (lfp,"\n");
      listIDs (p->right,lfp,flag);
   }
   return 1;
}



struct BinaryTree *CloseDataStruct (struct BinaryTree *p)  /* Recursive! */
{
   static int tof;
 //  printf( "static dans close datastruct %d %d\n",tof++,int(p->line));
   if (p->left != NULL)
   {
      CloseDataStruct(p->left);
   }
   else if (p->right != NULL)
   {
      CloseDataStruct(p->right);
   }
   deleteheap(p->line);
   releasetree(p); // 207
   return (NULL);
}
