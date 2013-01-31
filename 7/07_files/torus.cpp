//torus.cpp
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define OUTPUT_FILENAME "torus.obj" // 出力ファイル名
#define PI 3.1415926535

int main(int argc, char** argv) {
  int i,j;
  FILE *fout;

  // 出力用ファイルを開く
  //---------------------------
  fout = fopen(OUTPUT_FILENAME, "w");

  // ファイルを作成できなかった場合のエラー処理
  if(fout == NULL) {
    printf("File can't create\n");
    exit(0);
  }

  // 出力するモデルの基本情報
  double bradius = 100;  // 大半径bigradius
  double sradius = 50;   // 小半径smallradius
  int divisionNum = 20; // 分割数

  // 点情報の出力
  //---------------------------
  double x, y, z;

  //torusの点を出力
  //i = t,j = p
  for(i = 0; i < divisionNum; i++) {
      for(j = 0;j < divisionNum; j++){
          double thetat = i * 2 * PI / divisionNum;
          double thetap = j * 2 * PI / divisionNum;
          x = bradius * cos(thetat) + sradius * cos(thetap) * cos(thetat);
          y = bradius * sin(thetat) + sradius * cos(thetap) * sin(thetat);
          z = sradius * sin(thetap);
          fprintf(fout, "v %f %f %f\n", x, y ,z);
      }
  }

  // 面情報の出力
  //---------------------------
  // torusを出力
  for(i = 0; i < divisionNum - 1; i++) {
      for(j = 0;j < divisionNum - 1; j++){
          int v0 = i*divisionNum + 1 + j;
          int v1 = v0 + 1;
          int v2 = (i + 1)*divisionNum + 1 + j;
          fprintf(fout, "f %d %d %d\n", v0, v2, v1);
          v0 = v2;
          v1 = i*divisionNum + 2 + j;
          v2 = v2 + 1;
          fprintf(fout, "f %d %d %d\n", v0, v2, v1);
      }
      fprintf(fout, "f %d %d %d\n",i*divisionNum + 1, (i + 1)*divisionNum, (i + 2)*divisionNum);
      fprintf(fout, "f %d %d %d\n",i*divisionNum + 1, (i + 2)*divisionNum ,(i + 1)*divisionNum + 1); 
  }
  //最後の接続部分       
  for(i = 0; i < divisionNum - 1; i++) {                     
      int v0 = divisionNum*(divisionNum-1)+1+i; 
      int v1 = v0 + 1;
      int v2 = i+1;
      fprintf(fout, "f %d %d %d\n",v0,v2,v1);    
      v0 = v1;
      v1 = i + 2;
      v2 = i + 1;
      fprintf(fout, "f %d %d %d\n",v0,v2,v1);    
  }
  fprintf(fout, "f %d %d %d\n",divisionNum*(divisionNum-1)+1,divisionNum*divisionNum,divisionNum);    
  fprintf(fout, "f %d %d %d\n",divisionNum*(divisionNum-1)+1,divisionNum,1);    

  // 出力用ファイルを閉じる
  //---------------------------
  fclose(fout);
  return 0;
}
