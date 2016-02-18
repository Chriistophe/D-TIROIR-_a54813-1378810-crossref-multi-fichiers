   1 /********************************************************/
   2 /*                                                      */
   3 /*  C programming utility : variable referencer         */
   4 /*                                                      */
   5 /********************************************************/
   6
   7                   /* See notes above */
   8
   9    #include <stdio.h>
  10    #include <ctype.h>
  11
  12    #define TRUE        1
  13    #define FALSE       0
  14    #define DUMMY       0
  15    #define MAXSTR      512
  16    #define MAXIDSIZE   32
  17    #define WORDTABLE    33
  18
  19    int   LINECOUNT = 1;     /* Contains line no. in file */
  20    char  BUFFER[MAXIDSIZE]; /* Input BUFFER for IDs      */
  21    char  CH;                /* Current input character   */
  22    char  SPECIALCHAR;       /* macro/pointer flag        */
  23
  24 /**********************************************************/
  25 /* TABLE                                                  */
  26 /**********************************************************/
  27
  28    char *WORDTABLE [WORDTABLE] =   /* Table of resvd words */
  29
  30       {
  31       "auto"    ,
  32       "break"   ,
  33       "case"    ,
  34       "char"    ,
  35       "const",
  36       "continue",
  37       "default" ,
  38       "do"      ,
  39       "double"  ,
  40       "else"    ,
  41       "entry"   ,
  42       "enum"    ,
  43       "extern"  ,
  44       "float"   ,
  45       "for"     ,
  46       "goto"    ,
  47       "if"      ,
  48       "int"     ,
  49       "long"    ,
  50       "register",
  51       "return"  ,
  52       "short"   ,
  53       "signed"  ,
  54       "sizeof"  ,
  55       "static"  ,
  56       "struct"  ,
  57       "switch"  ,
  58       "typedef" ,
  59       "union"   ,
  60       "unsigned",
  61       "void"    ,
  62       "volatile",
  63       "while"   ,
  64       };
  65
  66 /********************************************************/
  67 /** STRUCTURES                                         **/
  68 /********************************************************/
  69
  70    struct heap
  71
  72       {
  73       short num;
  74       char spec;
  75       struct heap *next;
  76       };
  77
  78 /**********************************************************/
  79
  80    struct BinaryTree
  81
  82       {
  83       char *name;
  84       struct heap *line;
  85       struct BinaryTree *left;
  86       struct BinaryTree *right;
  87       }
  88
  89       *tree = NULL;
  90
  91 /**********************************************************/
  92 /* LEVEL 0 : main program                                 */
  93 /**********************************************************/
  94
  95    main ()
  96
  97    { FILE *fp;
  98      char *filename();
  99      struct BinaryTree *CloseDataStruct();
 100
 101    printf ("\nIdentifier Cross Reference V 1.0\n\n");
 102    if ((fp = fopen (filename(),"r")) == NULL)
 103       {
 104       printf ("Can't read file .. Aborted!\n\n");
 105       exit(0);
 106       }
 107    CH = getc(fp);
 108
 109    while (!feof(fp))
 110       {
 111       SkipBlanks (fp);
 112       RecordWord (fp);
 113       }
 114
 115    listIDs (tree);
 116    CloseDataStruct(tree);
 117    printf ("\n%d lines in source file\n",LINECOUNT);
 118    }
 119
 120 /**********************************************************/
 121 /* LEVEL 1                                                */
 122 /**********************************************************/
 123
 124    SkipBlanks (fp)          /* Skip irrelevant characters */
 125
 126    FILE *fp;
 127
 128    {
 129
 130    while (!feof(fp))
 131
 132       {
 133       if (iscsymf(CH))
 134          {
 135          return(DUMMY);
 136          }
 137       else
 138          {
 139          ParticularSkip(fp);
 140          }
 141       }
 142    }
 143
 144 /**********************************************************/
 145
 146    RecordWord (fp)  /* get ID in buffer & tube it to data */
 147
 148    FILE *fp;
 149
 150    { int tok;
 151
 152    CopyNextID (fp);
 153
 154    if ((tok = token()) == 0)       /* if not resved word */
 155       {
 156       RecordUserID(isfunction(fp));
 157       }
 158
 159    SPECIALCHAR = ' ';
 160    }
 161
 162 /**********************************************************/
 163
 164    listIDs (p)                       /* List Binary Tree */
 165
 166    struct BinaryTree *p;
 167
 168    { struct heap *h;
 169      int i = 0;
 170
 171    if (p != NULL)
 172       {
 173       listIDs (p->left);
 174       printf ("\n%-20s",p->name);
 175
 176       for (h = p->line; (h != NULL); h = h->next)
 177          {
 178          printf ("%c%-5d",h->spec,h->num);
 179          if ((++i % 8) == 0)
 180             {
 181             printf ("\n                    ");
 182             }
 183          }
 184
 185       printf ("\n");
 186       listIDs (p->right);
 187       }
 188    }
 189
 190 /*********************************************************/
 191
 192    struct BinaryTree *CloseDataStruct (p)  /* Recursive! */
 193
 194    struct BinaryTree *p;
 195
 196    {
 197    if (p->left != NULL)
 198       {
 199       CloseDataStruct(p->left);
 200       }
 201    else if (p->right != NULL)
 202       {
 203       CloseDataStruct(p->right);
 204       }
 205
 206    deleteheap(p->line);
 207    releasetree(p);
 208    return (NULL);
 209    }
 210
 211 /*********************************************************/
 212 /* LEVEL 2                                               */
 213 /*********************************************************/
 214
 215    ParticularSkip (fp)   /* handle particular characters */
 216
 217    FILE *fp;
 218
 219    { char c;
 220
 221    switch (CH)
 222
 223       {
 224       case '/' : if ((c = getc(fp)) == '*')
 225                     {
 226                     skipcomment (fp);
 227                     }
 228                  else
 229                     {
 230                     CH = c;
 231                     return (DUMMY);
 232                     }
 233                  break;
 234
 235       case '"' : if (skiptochar (fp,'"') > MAXSTR)
 236                     {
 237                     printf ("String too long or unterminated ");
 238                     printf ("at line %d\n",LINECOUNT);
 239                     exit (0);
 240                     }
 241                  break;
 242
 243       case '\'': if (skiptochar (fp,'\'') == 1)
 244                     {
 245                     if (CH=='\'') CH = getc(fp);;
 246                     }
 247                  break;
 248
 249       case '#' : skiptochar(fp,' ');
 250                  SPECIALCHAR = '#';
 251                  break;
 252
 253       case '\n': ++LINECOUNT;
 254       default  : CH = getc(fp);
 255                  SPECIALCHAR = ' ';
 256       }
 257    }
 258
 259 /*********************************************************/
 260
 261    CopyNextID (fp)    /* Put next identifier into BUFFER */
 262
 263    FILE *fp;
 264
 265    { int i = 0;
 266
 267    while (!feof(fp) && (iscsym (CH)))
 268       {
 269       BUFFER[i++] = CH;
 270       CH = getc (fp);
 271       }
 272
 273    BUFFER[i] = '\0';
 274    }
 275
 276 /**********************************************************/
 277
 278    token ()                    /* Token: pos in WORDTABLE */
 279
 280    { int i;
 281
 282    for (i = 0; i < WORDTABLE; i++)
 283       {
 284       if (strcmp(&(BUFFER[0]),WORDTABLE[i]) == 0)
 285          {
 286          return(i);
 287          }
 288       }
 289    return(0);
 290    }
 291
 292 /*********************************************************/
 293
 294    RecordUserID (fnflag) /* check ID type & install data */
 295
 296    int fnflag;
 297
 298    { char *strcat();
 299      struct BinaryTree *install();
 300
 301    if (fnflag)
 302       {
 303       strcat (BUFFER,"()");
 304       tree = install (tree);
 305       }
 306    else
 307       {
 308       tree = install (tree);
 309       }
 310    }
 311
 312 /**********************************************************/
 313
 314    isfunction (fp)          /* returns TRUE if ID is a fn */
 315
 316    FILE *fp;
 317
 318    {
 319    while(!feof(fp))
 320       {
 321       if (!(CH == ' ' || CH == '\n'))
 322          {
 323          break;
 324          }
 325       else if (CH == '\n')
 326          {
 327          ++LINECOUNT;
 328          }
 329       CH = getc(fp);
 330       }
 331
 332    if (CH == '(')
 333       {
 334       return (TRUE);
 335       }
 336    else
 337       {
 338       return (FALSE);
 339       }
 340    }
 341
 342 /**********************************************************/
 343
 344    deleteheap (h)     /* Release back to free memory pool */
 345
 346    struct heap *h;
 347
 348    { struct heap *temp = h;
 349
 350    while (h!=NULL && temp!=NULL)
 351       {
 352       temp = h->next;
 353       releaseheap(h);
 354       h = temp;
 355       }
 356    }
 357
 358 /**********************************************************/
 359 /** LEVEL 3                                              **/
 360 /**********************************************************/
 361
 362    skipcomment (fp)         /* skip to char after comment */
 363
 364    FILE *fp;
 365
 366    { char cs = 'x';
 367
 368    for (CH = getc(fp); !feof(fp); CH = getc(fp))
 369       {
 370       switch (CH)
 371          {
 372          case '\n': ++LINECOUNT;
 373                     break;
 374          case '/' : if (cs == '*')
 375                        {
 376                        CH = getc(fp);
 377                        return(DUMMY);
 378                        }
 379          }
 380       cs = CH;
 381       }
 382    }
 383
 384 /*********************************************************/
 385
 386    skiptochar (fp,ch)           /* skip to char after ch */
 387
 388    FILE *fp;
 389    char ch;
 390
 391    { int c=0;
 392
 393    while (((CH =getc(fp)) != ch) && !feof(fp))
 394       {
 395       if (CH == '\n')
 396          {
 397          ++LINECOUNT;
 398          }
 399       c++;
 400       }
 401
 402    CH = getc(fp);
 403    return (c);
 404    }
 405
 406 /*********************************************************/
 407
 408    struct BinaryTree *install (p)  /* install ID in tree */
 409
 410    struct BinaryTree *p;
 411
 412    { struct heap *pushonheap();
 413      struct BinaryTree *newtree();
 414      char *stringin();
 415      int pos;
 416
 417    if (p == NULL)                             /* new word */
 418       {
 419       p = newtree();
 420       p->name = stringin(BUFFER);
 421       p->line = pushonheap (NULL);
 422       p->left = NULL;
 423       p->right = NULL;
 424       return (p);
 425       }
 426
 427    if ((pos = strcmp (BUFFER,p->name)) == 0) /* found word*/
 428       {
 429       p->line = pushonheap(p->line);
 430       return (p);
 431       }
 432
 433    if (pos < 0)                        /* Trace down list */
 434       {
 435       p->left = install(p->left);
 436       }
 437    else
 438       {
 439       p->right = install(p->right);
 440       }
 441
 442    return (p);
 443    }
 444
 445 /*********************************************************/
 446 /* LEVEL 4                                               */
 447 /*********************************************************/
 448
 449    struct heap *pushonheap (h)  /* push nxt ln no.to heap */
 450
 451    struct heap *h;
 452
 453    { struct heap *hp,*newheap();
 454
 455    hp = newheap();
 456    hp->num  = LINECOUNT;
 457    hp->spec = SPECIALCHAR;
 458    hp->next = h;
 459
 460    return (hp);
 461    }
 462
 463 /*********************************************************/
 464 /* TOOLKIT file input                                    */
 465 /*********************************************************/
 466
 467    backone (ch,fp)              /* backspace one in file */
 468
 469    char ch;
 470    FILE *fp;
 471
 472    {
 473    if (ungetc(ch,fp) != ch)
 474       {
 475       printf ("\nDebug: Toolkit file input: backone() FAILED\n");
 476       exit(0);
 477       }
 478    }
 479
 480 /**********************************************************/
 481 /* TOOLKIT stdin                                          */
 482 /**********************************************************/
 483
 484    char *filename ()
 485
 486    { static char *fsp = ".................................";
 487
 488    do
 489       {
 490       printf ("Enter filename of source program: ");
 491       scanf ("%33s",fsp);
 492       skipgarb ();
 493       }
 494    while (strlen(fsp) == 0);
 495    return (fsp);
 496    }
 497
 498 /*********************************************************/
 499
 500    skipgarb ()          /* skip garbage upto end of line */
 501
 502    {
 503    while (getchar() != '\n');
 504    }
 505
 506 /**********************************************************/
 507 /* TOOLKIT data structure                                 */
 508 /**********************************************************/
 509
 510    char *stringin (array)    /* cpy str in arry to ptr loc*/
 511
 512    char *array;
 513
 514    { char *malloc(),*ptr;
 515      int i;
 516
 517    ptr = malloc (strlen(array)+1);
 518    for (i = 0; array[i] != '\0'; ptr[i] = array[i++]);
 519    ptr[i] = '\0';
 520    return(ptr);
 521    }
 522
 523 /**********************************************************/
 524
 525    struct heap *newheap ()
 526
 527    { char *malloc ();
 528    return ((struct heap *) malloc(sizeof(struct heap)));
 529    }
 530
 531 /**********************************************************/
 532
 533    struct BinaryTree *newtree ()
 534
 535    { char *malloc ();
 536    return ((struct BinaryTree *) malloc(sizeof(struct BinaryTree)));
 537    }
 538
 539 /*********************************************************/
 540
 541    releaseheap (ptr)
 542
 543    struct heap *ptr;
 544
 545    {
 546    if (free((char *) ptr) != 0)
 547
 548       {
 549       printf ("TOOLKIT datastruct: link release failed\n");
 550       }
 551    }
 552
 553 /**********************************************************/
 554
 555    releasetree (ptr)
 556
 557    struct BinaryTree *ptr;
 558
 559    {
 560    if (free((char *) ptr) != 0)
 561
 562       {
 563       printf ("TOOLKIT datastruct: link release failed\n");
 564       }
 565    }
 566                               /* end */
 567
 568



