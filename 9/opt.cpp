//opt.cpp
#include <GLUT/glut.h>
#include <math.h>   
#include <stdlib.h> 
#include <stdio.h> 
#include <algorithm>

// 3次元ベクトル
class Vector3d {
    public:
        double x, y, z;

        Vector3d() {
            x = y = z = 0;
        }

        Vector3d(double _x, double _y, double _z) {
            x = _x;
            y = _y;
            z = _z;
        }

        // 長さを1に正規化する
        void normalize() {
            double len = length();
            x /= len;
            y /= len;
            z /= len;
        }

        // 長さを返す
        double length() {
            return sqrt(x * x + y * y + z * z);
        }

        // 値を設定する
        void set(double _x, double _y, double _z) {
            x = _x;
            y = _y;
            z = _z; 
        }

        void set(Vector3d &vec) {
            set(vec.x, vec.y, vec.z);
        }
};

// 球体
class Sphere {
    public:
        Vector3d center; // 中心座標
        double radius;   // 半径
        double cR, cG, cB;  // Red, Green, Blue 値 0.0～1.0

        Sphere(double x, double y, double z, double r, 
                double cr, double cg, double cb) {
            center.x = x;
            center.y = y;
            center.z = z;
            radius = r;
            cR = cr;
            cG = cg;
            cB = cb;
        }

        // 点pを通り、v方向のRayとの交わりを判定する。
        // 交点が p+tv として表せる場合の t の値を返す。交わらない場合は-1を返す
        double getIntersec(Vector3d &p, Vector3d &v) { 
            // A*t^2 + B*t + C = 0 の形で表す
            double A = v.x * v.x + v.y * v.y + v.z * v.z;
            double B = 2.0 * (p.x * v.x - v.x * center.x +
                    p.y * v.y - v.y * center.y + 
                    p.z * v.z- v.z * center.z);
            double C = p.x * p.x - 2 * p.x * center.x + center.x * center.x + 
                p.y * p.y - 2 * p.y * center.y + center.y * center.y + 
                p.z * p.z - 2 * p.z * center.z + center.z * center.z -
                radius * radius;
            double D = B * B - 4 * A * C; // 判別式

            if (D >= 0) { // 交わる
                double t1 = (-B - sqrt(D)) / (2.0 * A);
                double t2 = (-B + sqrt(D)) / (2.0 * A);
                return t1 < t2 ? t1 : t2; // 小さいほうのtの値を返す
            } else { // 交わらない
                return -1.0;
            }
        }
};

// 板。xz平面に平行な面とする
class Board {
    public:
        double y; // y座標値

        Board(double _y) {
           y = _y;
        }

        // 点pを通り、v方向のRayとの交わりを判定する。
        // 交点が p+tv として表せる場合の t の値を返す。交わらない場合は負の値を返す
        double getIntersec(Vector3d &p, Vector3d &v) { 
            if(fabs(v.y) < 1.0e-10) return -1; // 水平なRayは交わらない

            double t = -1;
            // ★ここで t の値を計算する
            // ★ただしz座標が-3000より小さいなら交わらないものとする
            t = (y-p.y)/v.y;
            if((p.z+t*v.z) < -3000 ||(p.z+t*v.z) > 2000) t = -1;
            return t;
        }

        // x と y の値から床の色を返す（格子模様になるように）
        Vector3d getColorVec(double x, double z) {
            // ★ x, z の値によって(1.0, 1.0, 0.7)または(0.6, 0.6, 0.6)のどちらかの色を返すようにする
         if(x>0){
          if((int)(cos(x/100) + sin(z/100))%2)
                return Vector3d(1., 0., 0.6);
            else 
                return Vector3d(1.0, 0.6, 0.);
         }else{
             if((int)(cos(x/100) + sin(z/100))%2)
                 return Vector3d(1.0, 0.6, 0.);
             else 
                 return Vector3d(1., 0., 0.6);
                }

        }
};

int halfWidth;    // 描画領域の横幅/2
int halfHeight;   // 描画領域の縦幅/2
double screen_z = -1000;  // 投影面のz座標
double Kd = 0.8;  // 拡散反射定数
double Ks = 1.0;  // 鏡面反射定数
double Iin = 0.8; // 入射光の強さ 
double Ia  = 0.5; // 環境光
int    n = 2; //鏡面反射の次数
Vector3d viewPosition(0, 0, 0); // 原点=視点
Vector3d lightDirection(-2, -4, -2); // 入射光の進行方向


// レンダリングする球体
Sphere sphere(0.0, 0.0, -1500, // 中心座標
        50.0,           // 半径
        1., 0., 0.);  // RGB値

// 球体の置かれている床
Board board(-150); // y座標値を -150 にする。（球と接するようにする）

// 2つのベクトルの成す角のcos値を計算する
double getCos(Vector3d &v0, Vector3d &v1) {
    return (v0.x * v1.x + v0.y * v1.y + v0.z * v1.z ) / (v0.length() * v1.length());
}
int check=0;
// x, y で指定されたスクリーン座標での色(RGB)を colorVec のxyzの値に格納する
void getPixcelColor(double x, double y, Vector3d &colorVec) {
    // 原点からスクリーン上のピクセルへ飛ばすレイの方向
    Vector3d ray(x - viewPosition.x, y - viewPosition.y, screen_z - viewPosition.z); 

    ray.normalize(); // レイの長さの正規化
    // レイを飛ばして球と交差するか求める
    double t_sphere = sphere.getIntersec(viewPosition, ray);

    if(t_sphere > 0) { // 球との交点がある
        // ★前回の課題を参考に、球体の表面の色を計算で求め、colorVecに設定する
        //法線ベクトル
        Vector3d normal(viewPosition.x + t_sphere*ray.x - 0.,
                viewPosition.y + t_sphere*ray.y - 0.,
                viewPosition.z +  t_sphere*ray.z - (-1500));
        normal.normalize();
        double Is = 0; // 鏡面反射光
        double Id = 0; // 拡散反射光
        //diffuse用のcosを求める Nと-Lの内積
        double cosdiffuse = -getCos(lightDirection,normal);
        if(cosdiffuse<0)cosdiffuse=0;
        Id = Iin * Kd * cosdiffuse;

        //specularを作成する
        //まず正反射方向のベクトルRを求める
        double LN = getCos(lightDirection,normal);
        Vector3d R(lightDirection.x - 2*LN*normal.x,
                lightDirection.y - 2*LN*normal.y,
                lightDirection.z - 2*LN*normal.z);
        double coss = -getCos(R,ray);
        if(coss<0)coss=0;

        //cosのn乗を求める。
        for(int i = 0;i < n-1; i++)
            coss = coss*coss;

        Is = Iin * Ks * coss;

        double I = Id + Is + Ia;
        double r = std::min(I * sphere.cR, 1.0);
        double g = std::min(I * sphere.cG, 1.0);
        double b = std::min(I * sphere.cB, 1.0);
        colorVec.set(r, g, b);
        if(check++ == 0)
            getPixcelColor(x,y,colorVec);
        return;
    } 
check=0;
    // レイを飛ばして床と交差するか求める
    double t_board = board.getIntersec(viewPosition, ray);

    if(t_board > 0) { // 床との交点がある
        // ★床の表面の色を設定する
        double z_board = t_board * ray.z + viewPosition.z; 
        double x_board = t_board * ray.x + viewPosition.x;
        double y_board = t_board * ray.y + viewPosition.y;
        Vector3d tmp;
        tmp = board.getColorVec(x_board,z_board);
        double r = tmp.x;
        double g = tmp.y;
        double b = tmp.z;
        // ★球の影になる場合は、RGBの値をそれぞれ0.5倍する
        //rayと板の交点
        Vector3d viewboard(x_board,y_board,z_board);
        //交点から光の方向を見たときの方向
        Vector3d lightDire(-lightDirection.x,-lightDirection.y,-lightDirection.z);
        lightDire.normalize();
        double t_shadow = sphere.getIntersec(viewboard,lightDire);
        if(t_shadow > 0){
            r = r*0.5;
            g = g*0.5;
            b = b*0.5;
        }
        colorVec.set(r, g, b);
        return;
    }

    // 何とも交差しない
     colorVec.set(0, 0, 0); // 背景色（黒）を設定する
} 

// 描画を行う
void display(void) {

    glClear(GL_COLOR_BUFFER_BIT); // 描画内容のクリア

    // ピクセル単位で描画色を決定するループ処理
    for(int y = (-halfHeight); y <= halfHeight; y++ ) {
        for(int x = (-halfWidth); x <= halfWidth; x++ ) {
            Vector3d colorVec;
            double tmpx=0,tmpy=0,tmpz=0;
            //アンチエイリアシング
            double _x,_y;

            for(_y=y; _y < y+1;_y += 0.35){
                for(_x=x; _x < x+1;_x += 0.35){
                    // _x, _y 座標の色を取得する
                    getPixcelColor(_x, _y , colorVec);
                    tmpx += colorVec.x; 
                    tmpy += colorVec.y;
                    tmpz += colorVec.z;
                }
            }
           colorVec.set(tmpx/9,tmpy/9,tmpz/9);
           //以下3行はアンチエイリアシングなし
           /* Vector3d colorVec;
            // x, y 座標の色を取得する
            getPixcelColor(x, y , colorVec);
            */
            //取得した色で、描画色を設定する
            glColor3d(colorVec.x, colorVec.y, colorVec.z); 
            //退避させたx,yを戻す
            // (x, y) の画素を描画
            glBegin(GL_POINTS); 
            glVertex2i( x, y ); 
            glEnd( ); 
        }
    }
    glFlush();
}

void resizeWindow(int w, int h) {
    h = (h == 0) ? 1 : h;
    glViewport(0, 0, w, h);
    halfWidth = w/2;
    halfHeight = h/2;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // ウィンドウ内の座標系設定
    glOrtho( -halfWidth, halfWidth, -halfHeight, halfHeight, 0.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 27: exit(0);  /* ESC code */
    } 
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    lightDirection.normalize();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(400,400);
    glutInitWindowPosition(180,10);
    glutCreateWindow(argv[0]);
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glShadeModel(GL_FLAT);

    glutDisplayFunc(display);
    glutReshapeFunc(resizeWindow);
    glutKeyboardFunc(keyboard);
    glutMainLoop();

    return 0;
}

