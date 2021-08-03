/*
 * 'seenya2' joking engine 2 ver 0.70
 */

#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <stdbool.h>
#include  <ctype.h>

#define WL  256
#define RS  4
char  *romaji[]={"a","i","u","e","o",
                "ka","ki","ku","ke","ko",
                "sa","si","su","se","so",
                "ta","ti","tu","te","to",
                "na","ni","nu","ne","no",
                "ha","hi","hu","he","ho",
                "ma","mi","mu","me","mo",
                "ya","yi","yu","ye","yo",
                "ra","ri","ru","re","ro",
                "wa","wi","wu","we","wo",
                "ga","gi","gu","ge","go",
                "za","zi","zu","ze","zo",
                "ja","ji","ju","je","jo",
                "da","di","du","de","do",
                "ba","bi","bu","be","bo",
                "pa","pi","pu","pe","po",
                "fa","fi","fu","fe","fo",
                "ta","ti","tsu","te","to",
                "dza","dzi","dzu","dze","dzo",
                "kya","kyi","kyu","kye","kyo",
                "sha","shi","shu","she","sho",
                "cha","chi","chu","che","cho",
                "nya","nyi","nyu","nye","nyo",
                "hya","hyi","hyu","hye","hyo",
                "mya","myi","myu","mye","myo",
                "rya","ryi","ryu","rye","ryo",
                "gya","gyi","gyu","gye","gyo",
                "dya","dyi","dyu","dye","dyo",
                "bya","byi","byu","bye","byo",
                "pya","pyi","pyu","pye","pyo", };
char  origword[WL][RS];
int   beats;
char  infile[]="/home/gar/dic/je-edict-kanji.txt";
#define SOT (30*5)

bool  isvowel(char c) {
  switch(c) {
    case 'a': case 'i': case 'u':
    case 'e': case 'o': return(true);
    default:
      return(false);
  }
}

int decomposite_word(char *o,char word[WL][RS]) {
    int i,j,l;

    j=0;
    while(1) {

      if (*o=='\0') { // 行末か？
        word[j][0]='\0';
        return(j);
        }

      if (!isvowel(*o)&&(*o==*(o+1))) { // 小さい 'っ'の処理
        word[j][0]='x';
        word[j][1]='\0';
        o++;
        j++;
        continue;
        }

      if (*o=='n') { // "ん"の処理

        if (*(o+1)=='\0') { // 次に行末
          word[j][0]='n';
          word[j][1]='\0';
          o++;
          j++;
          return(j);
          }

        if (*(o+1)=='\'') { // 次に '
          word[j][0]='n';
          word[j][1]='\0';
          o+=2;
          j++;
          continue;
          }

        if (!isvowel(*(o+1))&&(*(o+1)!='y')) { // 次に子音か
          word[j][0]='n';
          word[j][1]='\0';
          o++;
          j++;
          continue;
          }
        }

      for(i=0;i<SOT;i++) {  // ローマ字テーブルにマッチするか？
        l=strlen(romaji[i]);
        if (strncmp(romaji[i],o,l)==0) {
          strcpy(&(word[j][0]),romaji[i]);
          o+=l;
          j++;

          if (*o=='-') { // 長音　−　の処理 (母音を重ねる）
            l=strlen((char *)(&(word[j-1][0])));
            word[j][0]=word[j-1][l-1];
            word[j][1]='\0';
            o++;
            j++;
            } 
          break;
          }
        }

      if (i==SOT) {   // 標準以外の記法の文字の場合
        while(*o!='\0') {
          if (!isvowel(*o++))
            continue;
          word[j][0]=*(o-1);
          word[j][1]='\0';
          j++;
          break;
          }
        }
    }
}

int diff_word(char w[WL][RS]) {
  int eq;
  int i;

  eq=0;
  for(i=0;origword[i][0]!='\0';i++)
    if (strcmp((char *)(&(origword[i][0])),(char *)(&(w[i][0])))==0)
      eq+=1;
  return(i-eq);
}

void  printword(char word[WL][RS]) {
  int i;
  for(i=0;word[i][0]!='\0';i++) {
    printf("%s ",(char *)&(word[i][0]));
    }
  printf("\n");
}

void  printjoke1(char *line) {
  char  d1[1024];

  sscanf(line,"%s",d1);  //最初の単語を飛ばす
  line+=strlen(d1)+1;

  if (!isdigit(*line)) { // ナンバリングされてない（ひとつ）か？
    sscanf(line,"%s",d1);
    printf("%s\n",d1);
    return;
    }
  // ナンバリングされている
  while(1) {
    if (*line=='\0')
       return;
    if (isdigit(*line)&&(*(line+1)=='.')&&(*(line+2)==' ')) {
      line+=3;
      sscanf(line,"%s",d1);
      printf("%s\n",d1);
      line+=strlen(d1)+1;
      while(1) {
        if (isdigit(*line)&&(*(line+1)=='.')&&((*line+2)==' '))
          break;
        if (*line=='\0')
          break;
        line++;
        }
      } else return;
   }
}

void  joke(char *o,FILE *fp) {
  char  lw[WL];
  char  lword[WL][RS];
  char  line[1024];
  int f;
  int b;

  beats=decomposite_word(o,origword);

  while(1) {
      if(fgets(line,1024,fp)==NULL)
        return;
      sscanf(line,"%s",lw);

      b=decomposite_word(lw,lword);

      if (b!=beats)         // 違う拍数
        continue;
      f=diff_word(lword);
      if (f==1)
        printjoke1(line);
    }
}
    
int main(int argc,char *argv[]) {
  char  orig[WL];
  FILE  *fp;

  if (argc==1)
    fscanf((FILE *)stdin,"%s",orig);
  else if (argc==2)
    strcpy(orig,argv[1]);
  else {
    fprintf(stderr,"Usage: %s word\n",argv[0]);
    exit(1);
    }

  fp=fopen(infile,"r");
  if (fp==NULL) {
    fprintf(stderr,"File open error.\n");
    exit(1);
    }
  joke(orig,fp);
  
  fclose(fp);
  exit(0);
}
