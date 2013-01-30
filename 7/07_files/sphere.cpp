//sphere.cpp
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define OUTPUT_FILENAME "sphere.obj" // 出力ファイル名
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
  double radius = 50;  // 球の半径
  int divisionNum = 30; // 分割数

  // 点情報の出力
  //---------------------------
  double x, y, z;
 //bottomの1点を出力
  fprintf(fout, "v %f %f %f\n", 0. ,-radius ,0.);
 // 1面の点を出力
  for(i = 1; i < divisionNum; i++) {
     for(j = 0;j < divisionNum; j++){
          double theta = i * PI / divisionNum - (PI / 2); //-pi/2 < theta < pi/2
          x = radius * cos(theta);
          y = radius * sin(theta);
          double thetaz = j * 2 *  PI / divisionNum;
          z = -x * sin(thetaz);
          x = x * cos(thetaz); 
          fprintf(fout, "v %f %f %f\n", x, y ,z);
      }
  }
  //topの1点を出力
   fprintf(fout, "v %f %f %f\n", 0., radius ,0.);

  // 面情報の出力
  //---------------------------
  // 球面を出力
  //bottomの1周完成
   for(i = 1; i < divisionNum; i++){
       int v0 = 1;
       int v1 = i + 2;      
       int v2 = i + 1;
       fprintf(fout, "f %d %d %d\n", v0, v1, v2);
   }
   fprintf(fout, "f %d %d %d\n", 1,2,divisionNum + 1);

    
   for(i = 0; i < divisionNum - 2; i++) {
       for(j = 0;j < divisionNum - 1; j++){
           //正方形のうちの一つの三角形
           int v0 = i*divisionNum + 2 + j;
           int v1 = (i+1)*divisionNum + 3 + j;
           int v2 = (i+1)*divisionNum + 2 + j;
           fprintf(fout, "f %d %d %d\n", v0, v1, v2); 
           //正方形のうちのもう一つの三角形
           v2 = v1;
           v1 = v0 + 1;
           fprintf(fout, "f %d %d %d\n", v0, v1, v2);
       }
       //一周した後の三角形
       fprintf(fout, "f %d %d %d\n", i*divisionNum + 3 + (j-1),(i+1)*divisionNum + 2 ,(i+1)*divisionNum + 3 + (j-1)); 
       fprintf(fout, "f %d %d %d\n", i*divisionNum + 3 + (j-1),i*divisionNum + 2,(i+1)*divisionNum + 2 );
   }

   //topの1周
   for(i =0 ; i < divisionNum - 1 ; i++){
       int v0 = divisionNum*(divisionNum - 1) + 2 ;
       int v1 = divisionNum*(divisionNum - 2) + 3 + i;      
       int v2 = divisionNum*(divisionNum - 2) + 2 + i;
       fprintf(fout, "f %d %d %d\n", v0, v2, v1);
   }
 fprintf(fout, "f %d %d %d\n", divisionNum*(divisionNum - 1) + 2,divisionNum*(divisionNum - 1) + 1,divisionNum*(divisionNum - 2) + 2);
 
  // 出力用ファイルを閉じる
  //---------------------------
  fclose(fout);
  return 0;
}
