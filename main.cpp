#include <iostream>
#include <list>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

using namespace std;

unsigned face_vertex_order[6][4]={{0, 3, 2, 1},
                                  {4, 5, 6, 7},
                                  {0, 1, 5, 4},
                                  {0, 4, 7, 3},
                                  {3, 7, 6, 2},
                                  {1, 2, 6, 5}};

unsigned face_vertex_order_aux[6][4]={{0, 1, 2, 3},
                                      {4, 7, 6, 5},
                                      {0, 4, 5, 1},
                                      {0, 3, 7, 4},
                                      {3, 2, 6, 7},
                                      {1, 5, 6, 2}};

class vertex{
    private:
        double x, y, z;
        int number;
        bool visibility;
    public:
        vertex():x(0),y(0),z(0), number(-1), visibility(true){}
        vertex(const double px, const double py, const double pz):x(px), y(py), z(pz), number(-1), visibility(true){}
        ~vertex(){x=0; y=0; z=0; number=0; visibility=false;}
        bool operator==(const vertex &a) const{return (x==a.x && y==a.y && z==a.z);}
        friend class voxel;
        friend class object;
        friend class Sculptor;
        friend class Matriz3D;
};

class voxel{
private:
    unsigned R, G, B;
    float A;
    vertex **V;
    bool *face_on;
public:
    voxel() : R(255), G(255), B(255), A(1){
        V=new vertex*[8];
        V[0]=new vertex(-0.5, +0.5, -0.5);
        V[1]=new vertex(-0.5, -0.5, -0.5);
        V[2]=new vertex(+0.5, -0.5, -0.5);
        V[3]=new vertex(+0.5, +0.5, -0.5);
        V[4]=new vertex(-0.5, +0.5, +0.5);
        V[5]=new vertex(-0.5, -0.5, +0.5);
        V[6]=new vertex(+0.5, -0.5, +0.5);
        V[7]=new vertex(+0.5, +0.5, +0.5);

        face_on=new bool[6];
        for(int i=0; i<6; i++){
            face_on[i]=true;
        }
    };
    voxel(const double x,const double y,const double z, const unsigned char R, const unsigned char G, const unsigned char B,const float A) : R(R), G(G), B(B), A(A){
        V=new vertex*[8];

        V[0]=new vertex(x-0.5, y-0.5, z+0.5);
        V[1]=new vertex(x-0.5, y-0.5, z-0.5);
        V[2]=new vertex(x-0.5, y+0.5, z-0.5);
        V[3]=new vertex(x-0.5, y+0.5, z+0.5);
        V[4]=new vertex(x+0.5, y-0.5, z+0.5);
        V[5]=new vertex(x+0.5, y-0.5, z-0.5);
        V[6]=new vertex(x+0.5, y+0.5, z-0.5);
        V[7]=new vertex(x+0.5, y+0.5, z+0.5);

        face_on=new bool[6];
        for(int i=0; i<6; i++) face_on[i]=true;
    }
    ~voxel(){
        if(V!=NULL){
            for(int i=0; i<8; i++){
                if(V[i]!=NULL && V[i]->number==(-1)){
                    delete V[i];
                }
            }
            delete[] V;
        }
        if(face_on==NULL) delete[] face_on;
    }
    void share_vertex_faces(voxel &a){
        if(this!=&a){
            for(unsigned i=0; i<8; i++){
                for(unsigned j=0; j<8; j++){
                    if(*V[i]==*a.V[j] && V[i]!=a.V[j]){
                        delete a.V[j];
                        a.V[j]=V[i];
                    }
                }
            }

            for(unsigned i=0, k=0, j=0; i<6; i++, k=0){
                if(i==0) j=1;
                else if(i==1) j=0;
                else if(i==2) j=4;
                else if(i==3) j=5;
                else if(i==4) j=2;
                else if(i==5) j=3;

                for(unsigned l=0; l<4; l++){
                    if(V[face_vertex_order[i][l]]==a.V[face_vertex_order_aux[j][l]])    k++;
                }
                if(k==4){
                    face_on[i]=false;
                    a.face_on[j]=false;
                }
            }
        }
    }
    friend class object;
    friend class Sculptor;
    friend class Matriz3D;
};

class Matriz3D{
    public:
        unsigned n_vertex, n_face;
        voxel ****Space;
        vertex **vertex_order;
        int Xi, Xf, Yi, Yf, Zi, Zf;
        void count_vertex_faces(){
            n_vertex=0;
            n_face=0;

            for(unsigned i=0; i<(unsigned)(Xf-Xi); i++){
                for(unsigned j=0; j<(unsigned)(Yf-Yi); j++){
                    for(unsigned k=0; k<(unsigned)(Zf-Zi); k++){
                        if(Space[i][j][k]!=NULL){
                            for(unsigned l=0; l<6; l++){
                                if(Space[i][j][k]->face_on[l]) n_face++;
                            }
                            for(unsigned l=0; l<8; l++){
                                if(Space[i][j][k]->V[l]->number==-1 && Space[i][j][k]->V[l]->visibility){
                                    Space[i][j][k]->V[l]->number=n_vertex;
                                    n_vertex++;
                                }
                            }
                        }
                    }
                }
            }
        }
        void order_vertex(){
            count_vertex_faces();
            vertex_order=new vertex*[n_vertex];
            for(unsigned i=0; i<(unsigned)(Xf-Xi); i++){
                for(unsigned j=0; j<(unsigned)(Yf-Yi); j++){
                    for(unsigned k=0; k<(unsigned)(Zf-Zi); k++){
                        if(Space[i][j][k]!=NULL){
                            for(unsigned l=0; l<8; l++){
                                if(Space[i][j][k]->V[l]->number>=0){
                                    vertex_order[Space[i][j][k]->V[l]->number]=Space[i][j][k]->V[l];
                                }
                            }
                        }
                    }
                }
            }
        }
        void check_vicinity(){
            for(int i=0; i<(int)(Xf-Xi); i++){
                for(int j=0; j<(int)(Yf-Yi); j++){
                    for(int k=0; k<(int)(Zf-Zi); k++){
                        if(Space[i][j][k]!=NULL){
                            for(int l=0; l<=1; l++){
                                for(int m=-1; m<=1; m++){
                                    for(int n=-1; n<=1; n++){
                                        if(i+l<(int)(Xf-Xi) && j+m>=0 && j+m<(Yf-Yi) && k+n>=0 && k+n<(Zf-Zi) && Space[i+l][j+m][k+n]!=NULL){
                                            Space[i][j][k]->share_vertex_faces(*Space[i+l][j+m][k+n]);
                                        }
                                    }
                                }
                            }
                            if((i+1)<(int)(Xf-Xi) && (j+1)<(int)(Yf-Yi) && (k+1)<(int)(Zf-Zi) &&
                               Space[i][j][k+1]!=NULL && Space[i][j+1][k]!=NULL && Space[i][j+1][k+1]!=NULL &&
                               Space[i+1][j][k+1]!=NULL && Space[i+1][j+1][k]!=NULL && Space[i+1][j+1][k+1]!=NULL && Space[i+1][j][k]!=NULL){
                               Space[i][j][k]->V[7]->visibility=false;
                            }
                        }
                    }
                }
            }
        }

    public:
        Matriz3D() : Xi(0), Xf(0), Yi(0), Yf(0), Zi(0), Zf(0), n_vertex(0), n_face(0), Space(NULL){}
        Matriz3D(const int Xi, const int Yi, const int Zi, const int Xf, const int Yf, const int Zf) : Xi(Xi), Xf(Xf), Yi(Yi), Yf(Yf), Zi(Zi), Zf(Zf){
            Space=new voxel***[Xf-Xi];
            for(unsigned i=0; i<(Xf-Xi); i++){
                Space[i]=new voxel**[Yf-Yi];
                for(unsigned j=0; j<(Yf-Yi); j++){
                    Space[i][j]=new voxel*[Zf-Zi];
                    for(unsigned k=0; k<(Zf-Zi); k++){
                        Space[i][j][k]=NULL;
                    }
                }
            }
        }
        ~Matriz3D(){
            if(Space!=NULL){
                for(unsigned i=0; i<(Xf-Xi); i++){
                    if(Space[i]!=NULL){
                        for(unsigned j=0; j<(Yf-Yi); j++){
                            if(Space[i][j]!=NULL){
                                for(unsigned k=0; k<(Zf-Zi); k++){
                                    if(Space[i][j][k]!=NULL) delete Space[i][j][k];
                                }
                                delete[] Space[i][j];
                            }
                        }
                        delete[] Space[i];
                    }
                }
            }
            delete[] Space;

            if(vertex_order!=NULL){
                for(unsigned i=0; i<n_vertex; i++){
                    if(vertex_order[i]!=NULL){
                        delete vertex_order[i];
                    }
                }
                delete[] vertex_order;
            }
        }

        void save_OFF(const string a){
            check_vicinity();
            order_vertex();
            if((Xf-Xi)==Xi || (Yf-Yi)==Yi || (Zf-Zi)==Zi || Space==NULL || vertex_order==NULL){
                cout << "Falha ao ao tentar Salvar Objeto\n";
                fflush(stdin); getchar();
                return;
            }
            char *arq=new char[a.size()+5];
            strcpy(arq,a.c_str());
            arq[a.size()]='.';
            arq[a.size()+1]='O';
            arq[a.size()+2]='F';
            arq[a.size()+3]='F';
            arq[a.size()+4]='\0';


            FILE *f=fopen(arq, "w");
            fprintf(f, "OFF\n%d %d %d\n\n", n_vertex, n_face, 0);

            for(unsigned i=0; i<n_vertex; i++){
                fprintf(f, "%.2f %.2f %.2f\n", vertex_order[i]->x, vertex_order[i]->y, vertex_order[i]->z);
            }
            fprintf(f,"\n");

            for(unsigned i=0; i<(Xf-Xi); i++){
                for(unsigned j=0; j<(Yf-Yi); j++){
                    for(unsigned k=0; k<(Zf-Zi); k++){
                        if(Space[i][j][k]!=NULL){
                            for(unsigned l=0; l<6; l++){
                                if(Space[i][j][k]->face_on[l]){
                                    fprintf(f, "%d ", 4);
                                    for(unsigned m=0; m<4; m++){
                                        fprintf(f, "%d ", Space[i][j][k]->V[face_vertex_order[l][m]]->number);
                                    }
                                    fprintf(f, "%d %d %d %.2f\n", Space[i][j][k]->R, Space[i][j][k]->G, Space[i][j][k]->B, Space[i][j][k]->A);
                                }
                            }
                        }
                    }
                }
            }
            fclose(f);
            delete[] arq;
        }

        void create_space(){
            if(Space!=NULL || (Xf-Xi)==0 || (Yf-Yi)==0 || (Zf-Zi)==0){
                cout << "ERROR: Espaco ja criado ou medidas indefinidas\n";
                fflush(stdin); getchar(); system("cls");
                return;
            }

            Space=new voxel***[Xf-Xi];
            for(unsigned i=0; i<(Xf-Xi); i++){
                Space[i]=new voxel**[Yf-Yi];
                for(unsigned j=0; j<(Yf-Yi); j++){
                    Space[i][j]=new voxel*[Zf-Zi];
                    for(unsigned k=0; k<(Zf-Zi); k++){
                        Space[i][j][k]=NULL;
                    }
                }
            }

        };

        void receive_min_limit(const int x, const int y, const int z){Xi=x; Yi=y, Zi=z;}
        void receive_max_limit(const int x, const int y, const int z){Xf=x; Yf=y, Zf=z;}

        friend class Sculptor;
        friend class object;
};

class object{
protected:
    unsigned char R, G, B;
    float A;
    int Xc, Yc, Zc;
    string name;
public:
    object() : Xc(0), Yc(0), Zc(0), R(0), G(0), B(0), A(0), name(){}
    object(const int x, const int y, const int z, const unsigned char R, const unsigned char G, const unsigned char B, const float A, string name) : Xc(x), Yc(y), Zc(z), R(R), G(G), B(B), name(name){
            if(A<0 && A>1) exit(0);

            this->A=A;
    }
    object(object &a) : R(a.R), G(a.G), B(a.B), A(a.A), Xc(a.Xc), Yc(a.Yc), Zc(a.Zc), name(a.name){}
    virtual ~object(){R=0; G=0; B=0; A=0; Xc=0; Yc=0; Zc=0;}
    void operator=(const object&) const;
    virtual void draw(Matriz3D&) const=0;
    virtual void draw_intersection(Matriz3D&, const object&) const=0;
    virtual void draw_minus(Matriz3D&, const object&) const=0;

    virtual bool draw_true(const unsigned &i, const unsigned &j, const unsigned &k, const Matriz3D &M) const=0;
    friend class Sculptor;

    virtual int get_left_limit() const=0;
    virtual int get_right_limit() const=0;
    virtual int get_upper_limit() const=0;
    virtual int get_bottom_limit() const=0;
    virtual int get_front_limit() const=0;
    virtual int get_back_limit() const=0;
    };

class parale : public object{
protected:
    unsigned c, h, l;

public:
    parale(): c(0), h(0), l(0), object(0,0,0,0,0,0,0, "parale"){}
    parale(parale &a) : object(a.Xc, a.Yc, a.Zc, a.R, a.G, a.B, a.A, a.name),c(a.c), h(a.h), l(a.l){}
    parale(const unsigned c, const unsigned h, const unsigned l, const int x, const int y, const int z, const unsigned char R, const unsigned char G, const unsigned char B, const float A) : c(c), h(h), l(l), object(x, y, z, R, G, B, A, "parale"){}
    ~parale(){c=0; h=0; l=0;}

    void draw(Matriz3D &M) const{
        for(unsigned i=ceil(((float)Xc-(float)c/2.0-(float)M.Xi)); i<ceil(((float)Xc+(float)c/2.0-(float)M.Xi)); i++){
            for(unsigned j=ceil(((float)Yc-(float)h/2.0-(float)M.Yi)); j<ceil(((float)Yc+(float)h/2.0-(float)M.Yi)); j++){
                for(unsigned k=ceil(((float)Zc-(float)l/2.0-(float)M.Zi)); k<ceil(((float)Zc+(float)l/2.0-(float)M.Zi)); k++){
                    if(M.Space[i][j][k]==NULL){
                        M.Space[i][j][k]=new voxel((float)M.Xi+(float)i, (float)M.Yi+(float)j, (float)M.Zi+(float)k, R, G, B, A);
                    }
                }
            }
        }
    }
    void draw_intersection(Matriz3D &M, const object &O) const{
        for(unsigned i=ceil(((float)Xc-(float)c/2.0-(float)M.Xi)); i<ceil(((float)Xc+(float)c/2.0-(float)M.Xi)); i++){
            for(unsigned j=ceil(((float)Yc-(float)h/2.0-(float)M.Yi)); j<ceil(((float)Yc+(float)h/2.0-(float)M.Yi)); j++){
                for(unsigned k=ceil(((float)Zc-(float)l/2.0-(float)M.Zi)); k<ceil(((float)Zc+(float)l/2.0-(float)M.Zi)); k++){
                    if(M.Space[i][j][k]==NULL && O.draw_true(i,j,k,M)){
                        M.Space[i][j][k]=new voxel((float)M.Xi+(float)i, (float)M.Yi+(float)j, (float)M.Zi+(float)k, R, G, B, A);
                    }
                }
            }
        }
    }
    void draw_minus(Matriz3D &M, const object &O) const{
        for(unsigned i=ceil(((float)Xc-(float)c/2.0-(float)M.Xi)); i<ceil(((float)Xc+(float)c/2.0-(float)M.Xi)); i++){
            for(unsigned j=ceil(((float)Yc-(float)h/2.0-(float)M.Yi)); j<ceil(((float)Yc+(float)h/2.0-(float)M.Yi)); j++){
                for(unsigned k=ceil(((float)Zc-(float)l/2.0-(float)M.Zi)); k<ceil(((float)Zc+(float)l/2.0-(float)M.Zi)); k++){
                    if(M.Space[i][j][k]==NULL && !O.draw_true(i,j,k,M)){
                        M.Space[i][j][k]=new voxel((float)M.Xi+(float)i, (float)M.Yi+(float)j, (float)M.Zi+(float)k, R, G, B, A);
                    }
                }
            }
        }
    }

    bool draw_true(const unsigned &i, const unsigned &j, const unsigned &k, const Matriz3D &M) const{
        return (i>=ceil(((float)Xc-(float)c/2.0-(float)M.Xi)) && i<ceil(((float)Xc+(float)c/2.0-(float)M.Xi)) &&
                j>=ceil(((float)Yc-(float)h/2.0-(float)M.Yi)) && j<ceil(((float)Yc+(float)h/2.0-(float)M.Yi)) &&
                k>=ceil(((float)Zc-(float)l/2.0-(float)M.Zi)) && k<ceil(((float)Zc+(float)l/2.0-(float)M.Zi)));
    }

    int get_left_limit() const{return (ceil((float)Xc-(float)c/2.0));}
    int get_right_limit() const{return (ceil((float)Xc+(float)c/2.0));}
    int get_upper_limit() const{return (ceil((float)Yc+(float)h/2.0));}
    int get_bottom_limit() const{return (ceil((float)Yc-(float)h/2.0));}
    int get_front_limit() const{return (ceil((float)Zc+(float)l/2.0));}
    int get_back_limit() const{return (ceil((float)Zc-(float)l/2.0));}

};

class kamehameha : public object{
protected:
    float radius;

public:
    kamehameha() : radius(0), object(0,0,0,0,0,0,0, "kamehameha"){}
    kamehameha(kamehameha &a) : object(a.Xc, a.Yc, a.Zc, a.R, a.G, a.B, a.A, a.name), radius(a.radius){}
    kamehameha(const float radius, const int x, const int y, const int z, const unsigned char R, const unsigned char G, const unsigned char B, const float A) : radius(radius), object(x,y,z,R,G,B,A, "kamehameha"){}
    ~kamehameha(){radius=0;}

    void draw(Matriz3D &M) const{
        for(unsigned i=ceil((float)Xc-radius-(float)M.Xi); i<ceil((float)Xc+radius-(float)M.Xi); i++){
            for(unsigned j=ceil((float)Yc-radius-M.Yi); j<ceil((float)Yc+radius-M.Yi); j++){
                for(unsigned k=ceil((float)Zc-radius-M.Zi); k<ceil((float)Zc+radius-M.Zi); k++){
                    if(M.Space[i][j][k]==NULL && draw_true(i,j,k,M)){
                        M.Space[i][j][k]=new voxel((float)M.Xi+(float)i, M.Yi+(float)j, M.Zi+(float)k, R, G, B, A);
                    }
                }
            }
        }
    }
    void draw_intersection(Matriz3D &M, const object &O) const{
        for(unsigned i=ceil((float)Xc-radius-(float)M.Xi); i<ceil((float)Xc+radius-(float)M.Xi); i++){
            for(unsigned j=ceil((float)Yc-radius-M.Yi); j<ceil((float)Yc+radius-M.Yi); j++){
                for(unsigned k=ceil((float)Zc-radius-M.Zi); k<ceil((float)Zc+radius-M.Zi); k++){
                    if(M.Space[i][j][k]==NULL && draw_true(i,j,k,M) && O.draw_true(i,j,k,M)){
                        M.Space[i][j][k]=new voxel((float)M.Xi+(float)i, M.Yi+(float)j, M.Zi+(float)k, R, G, B, A);
                    }
                }
            }
        }
    }
    void draw_minus(Matriz3D &M, const object &O) const{
        for(unsigned i=ceil((float)Xc-radius-(float)M.Xi); i<ceil((float)Xc+radius-(float)M.Xi); i++){
            for(unsigned j=ceil((float)Yc-radius-M.Yi); j<ceil((float)Yc+radius-M.Yi); j++){
                for(unsigned k=ceil((float)Zc-radius-M.Zi); k<ceil((float)Zc+radius-M.Zi); k++){
                    if(M.Space[i][j][k]==NULL && draw_true(i,j,k,M) && !O.draw_true(i,j,k,M)){
                        M.Space[i][j][k]=new voxel((float)M.Xi+(float)i, M.Yi+(float)j, M.Zi+(float)k, R, G, B, A);
                    }
                }
            }
        }
    }

    bool draw_true(const unsigned &i, const unsigned &j, const unsigned &k, const Matriz3D &M) const{
        return (radius==sqrt(pow((float)i-((float)Xc-(float)M.Xi), 2)+pow((float)j-((float)Yc-M.Yi), 2)+pow((float)k-((float)Zc-M.Zi), 2)));
    }


    int get_left_limit() const{return ceil((float)Xc-radius);}
    int get_right_limit() const{return ceil((float)Xc+radius);}
    int get_upper_limit() const{return ceil((float)Yc+radius);}
    int get_bottom_limit() const{return ceil((float)Yc-radius);}
    int get_front_limit() const{return ceil((float)Zc+radius);}
    int get_back_limit() const{return ceil((float)Zc-radius);}
};

class elipsoid : public object{
protected:
    float a, b, c;

public:
    elipsoid() : a(0), b(0), c(0), object(0,0,0,0,0,0,0,"elipsoid"){}
    elipsoid(elipsoid &a) : object(a.Xc, a.Yc, a.Zc, a.R, a.G, a.B, a.A, a.name), a(a.a), b(a.b), c(a.c){}
    elipsoid(const float a, const float b, const float c, const int x, const int y, const int z, const unsigned char R, const unsigned char G, const unsigned char B, const float A) : a(a), b(b), c(c), object(x,y,z,R,G,B,A, "elipsoid"){}
    ~elipsoid(){a=0; b=0; c=0;}

    void draw(Matriz3D &M) const{
        for(unsigned i=ceil((float)Xc-a-(float)M.Xi); i<ceil((float)Xc+a-(float)M.Xi); i++){
            for(unsigned j=ceil((float)Yc-b-M.Yi); j<ceil((float)Yc+b-M.Yi); j++){
                for(unsigned k=ceil((float)Zc-c-M.Zi); k<ceil((float)Zc+c-M.Zi); k++){
                    if(M.Space[i][j][k]==NULL && draw_true(i,j,k,M)){
                        M.Space[i][j][k]=new voxel((float)M.Xi+(float)i, M.Yi+(float)j, M.Zi+(float)k, R, G, B, A);
                    }
                }
            }
        }
    }
    void draw_intersection(Matriz3D &M, const object &O) const{
        for(unsigned i=ceil((float)Xc-a-(float)M.Xi); i<ceil((float)Xc+a-(float)M.Xi); i++){
            for(unsigned j=ceil((float)Yc-b-M.Yi); j<ceil((float)Yc+b-M.Yi); j++){
                for(unsigned k=ceil((float)Zc-c-M.Zi); k<ceil((float)Zc+c-M.Zi); k++){
                    if(M.Space[i][j][k]==NULL && draw_true(i,j,k,M) && O.draw_true(i,j,k,M)){
                        M.Space[i][j][k]=new voxel((float)M.Xi+(float)i, M.Yi+(float)j, M.Zi+(float)k, R, G, B, A);
                    }
                }
            }
        }
    }
    void draw_minus(Matriz3D &M, const object &O) const{
        for(unsigned i=ceil((float)Xc-a-(float)M.Xi); i<ceil((float)Xc+a-(float)M.Xi); i++){
            for(unsigned j=ceil((float)Yc-b-M.Yi); j<ceil((float)Yc+b-M.Yi); j++){
                for(unsigned k=ceil((float)Zc-c-M.Zi); k<ceil((float)Zc+c-M.Zi); k++){
                    if(M.Space[i][j][k]==NULL && draw_true(i,j,k,M) && !O.draw_true(i,j,k,M)){
                        M.Space[i][j][k]=new voxel((float)M.Xi+(float)i, M.Yi+(float)j, M.Zi+(float)k, R, G, B, A);
                    }
                }
            }
        }
    }

    bool draw_true(const unsigned &i, const unsigned &j, const unsigned &k, const Matriz3D &M) const{
        return (1>=sqrt(pow(((float)i-((float)Xc-(float)M.Xi))/a, 2)+pow(((float)j-((float)Yc-M.Yi))/b, 2)+pow(((float)k-((float)Zc-M.Zi))/c, 2)));
    }


    int get_left_limit() const{return ceil((float)Xc-a);}
    int get_right_limit() const{return ceil((float)Xc+a);}
    int get_upper_limit() const{return ceil((float)Yc+b);}
    int get_bottom_limit() const{return ceil((float)Yc-b);}
    int get_front_limit() const{return ceil((float)Zc+c);}
    int get_back_limit() const{return ceil((float)Zc-c);}
};

class cone : public object{
protected:
    float Ri, Rf;
    unsigned tipo, h;

public:
    cone() : Ri(0), Rf(0), tipo(0), h(0), object(0,0,0,0,0,0,0, "cone"){}
    cone(cone &a) : object(a.Xc, a.Yc, a.Zc, a.R, a.G, a.B, a.A, a.name), Ri(a.Ri), Rf(a.Rf), h(a.h), tipo(a.tipo){}
    cone(const unsigned tipo, const unsigned h, const float Ri, const float Rf, const int x, const int y, const int z, const unsigned char R, const unsigned char G, const unsigned char B, const float A) : object(x,y,z,R,G,B,A, "cone"), Ri(Ri), Rf(Rf), tipo(tipo), h(h){}
    ~cone(){tipo=0; Rf=0; Ri=0; h=0;}

    void draw(Matriz3D &M) const{
        float radius, Tg=(Rf-Ri)/(float)h;
        if(tipo==1){
            for(unsigned i=ceil((float)Xc-(float)h/2.0-(float)M.Xi); i<ceil((float)Xc+(float)h/2.0-(float)M.Xi); i++){
                radius=Ri+Tg*(float)(i-((float)Xc-(float)h/2.0-(float)M.Xi));
                for(unsigned j=ceil((float)Yc-radius-(float)M.Yi); j<ceil((float)Yc+radius-(float)M.Yi); j++){
                    for(unsigned k=ceil((float)Zc-radius-(float)M.Zi); k<ceil((float)Zc+radius-(float)M.Zi); k++){
                        if(M.Space[i][j][k]==NULL && draw_true(i,j,k,M)){
                            M.Space[i][j][k]=new voxel((float)M.Xi+(float)i, M.Yi+(float)j, M.Zi+(float)k,R,G,B,A);
                        }
                    }
                }
            }
        }else if(tipo==2){
            for(unsigned j=ceil((float)Yc-(float)h/2.0-(float)M.Yi); j<ceil((float)Yc+(float)h/2.0-(float)M.Yi); j++){
                radius=Ri+Tg*(float)(j-((float)Yc-(float)h/2.0-(float)M.Yi));
                for(unsigned i=ceil((float)Xc-radius-(float)M.Xi); i<ceil((float)Xc+radius-(float)M.Xi); i++){
                    for(unsigned k=ceil((float)Zc-radius-(float)M.Zi); k<ceil((float)Zc+radius-(float)M.Zi); k++){
                        if(M.Space[i][j][k]==NULL && draw_true(i,j,k,M)){
                            M.Space[i][j][k]=new voxel((float)M.Xi+(float)i, M.Yi+(float)j, M.Zi+(float)k,R,G,B,A);
                        }
                    }
                }
            }
        }else if(tipo==3){
            for(unsigned k=ceil((float)Zc-(float)h/2.0-(float)M.Zi); k<ceil((float)Zc+(float)h/2.0-(float)M.Zi); k++){
                radius=Ri+Tg*(float)(k-((float)Zc-(float)h/2.0-(float)M.Zi));
                for(unsigned j=ceil((float)Yc-radius-(float)M.Yi); j<ceil((float)Yc+radius-(float)M.Yi); j++){
                    for(unsigned i=ceil((float)Xc-radius-(float)M.Xi); i<ceil((float)Xc+radius-(float)M.Xi); i++){
                        if(M.Space[i][j][k]==NULL && draw_true(i,j,k,M)){
                            M.Space[i][j][k]=new voxel((float)M.Xi+(float)i, M.Yi+(float)j, M.Zi+(float)k,R,G,B,A);
                        }
                    }
                }
            }
        }
    }
    void draw_intersection(Matriz3D &M, const object &O) const{
        float radius, Tg=(Rf-Ri)/(float)h;
        if(tipo==1){
            for(unsigned i=ceil((float)Xc-(float)h/2.0-(float)M.Xi); i<ceil((float)Xc+(float)h/2.0-(float)M.Xi); i++){
                radius=Ri+Tg*(float)(i-((float)Xc-(float)h/2.0-(float)M.Xi));
                for(unsigned j=ceil((float)Yc-radius-(float)M.Yi); j<ceil((float)Yc+radius-(float)M.Yi); j++){
                    for(unsigned k=ceil((float)Zc-radius-(float)M.Zi); k<ceil((float)Zc+radius-(float)M.Zi); k++){
                        if(M.Space[i][j][k]==NULL && draw_true(i,j,k,M) && O.draw_true(i,j,k,M)){
                            M.Space[i][j][k]=new voxel((float)M.Xi+(float)i, M.Yi+(float)j, M.Zi+(float)k,R,G,B,A);
                        }
                    }
                }
            }
        }else if(tipo==2){
            for(unsigned j=ceil((float)Yc-(float)h/2.0-(float)M.Yi); j<ceil((float)Yc+(float)h/2.0-(float)M.Yi); j++){
                radius=Ri+Tg*(float)(j-((float)Yc-(float)h/2.0-(float)M.Yi));
                for(unsigned i=ceil((float)Xc-radius-(float)M.Xi); i<ceil((float)Xc+radius-(float)M.Xi); i++){
                    for(unsigned k=ceil((float)Zc-radius-(float)M.Zi); k<ceil((float)Zc+radius-(float)M.Zi); k++){
                        if(M.Space[i][j][k]==NULL && draw_true(i,j,k,M) && O.draw_true(i,j,k,M)){
                            M.Space[i][j][k]=new voxel((float)M.Xi+(float)i, M.Yi+(float)j, M.Zi+(float)k,R,G,B,A);
                        }
                    }
                }
            }
        }else if(tipo==3){
            for(unsigned k=ceil((float)Zc-(float)h/2.0-(float)M.Zi); k<ceil((float)Zc+(float)h/2.0-(float)M.Zi); k++){
                radius=Ri+Tg*(float)(k-((float)Zc-(float)h/2.0-(float)M.Zi));
                for(unsigned j=ceil((float)Yc-radius-(float)M.Yi); j<ceil((float)Yc+radius-(float)M.Yi); j++){
                    for(unsigned i=ceil((float)Xc-radius-(float)M.Xi); i<ceil((float)Xc+radius-(float)M.Xi); i++){
                        if(M.Space[i][j][k]==NULL && draw_true(i,j,k,M) && O.draw_true(i,j,k,M)){
                            M.Space[i][j][k]=new voxel((float)M.Xi+(float)i, M.Yi+(float)j, M.Zi+(float)k,R,G,B,A);
                        }
                    }
                }
            }
        }
    }
    void draw_minus(Matriz3D &M, const object &O) const{
        float radius, Tg=(Rf-Ri)/(float)h;
        if(tipo==1){
            for(unsigned i=ceil((float)Xc-(float)h/2.0-(float)M.Xi); i<ceil((float)Xc+(float)h/2.0-(float)M.Xi); i++){
                radius=Ri+Tg*(float)(i-((float)Xc-(float)h/2.0-(float)M.Xi));
                for(unsigned j=ceil((float)Yc-radius-(float)M.Yi); j<ceil((float)Yc+radius-(float)M.Yi); j++){
                    for(unsigned k=ceil((float)Zc-radius-(float)M.Zi); k<ceil((float)Zc+radius-(float)M.Zi); k++){
                        if(M.Space[i][j][k]==NULL && draw_true(i,j,k,M) && O.draw_true(i,j,k,M)){
                            M.Space[i][j][k]=new voxel((float)M.Xi+(float)i, M.Yi+(float)j, M.Zi+(float)k,R,G,B,A);
                        }
                    }
                }
            }
        }else if(tipo==2){
            for(unsigned j=ceil((float)Yc-(float)h/2.0-(float)M.Yi); j<ceil((float)Yc+(float)h/2.0-(float)M.Yi); j++){
                radius=Ri+Tg*(float)(j-((float)Yc-(float)h/2.0-(float)M.Yi));
                for(unsigned i=ceil((float)Xc-radius-(float)M.Xi); i<ceil((float)Xc+radius-(float)M.Xi); i++){
                    for(unsigned k=ceil((float)Zc-radius-(float)M.Zi); k<ceil((float)Zc+radius-(float)M.Zi); k++){
                        if(M.Space[i][j][k]==NULL && draw_true(i,j,k,M) && O.draw_true(i,j,k,M)){
                            M.Space[i][j][k]=new voxel((float)M.Xi+(float)i, M.Yi+(float)j, M.Zi+(float)k,R,G,B,A);
                        }
                    }
                }
            }
        }else if(tipo==3){
            for(unsigned k=ceil((float)Zc-(float)h/2.0-(float)M.Zi); k<ceil((float)Zc+(float)h/2.0-(float)M.Zi); k++){
                radius=Ri+Tg*(float)(k-((float)Zc-(float)h/2.0-(float)M.Zi));
                for(unsigned j=ceil((float)Yc-radius-(float)M.Yi); j<ceil((float)Yc+radius-(float)M.Yi); j++){
                    for(unsigned i=ceil((float)Xc-radius-(float)M.Xi); i<ceil((float)Xc+radius-(float)M.Xi); i++){
                        if(M.Space[i][j][k]==NULL && draw_true(i,j,k,M) && O.draw_true(i,j,k,M)){
                            M.Space[i][j][k]=new voxel((float)M.Xi+(float)i, M.Yi+(float)j, M.Zi+(float)k,R,G,B,A);
                        }
                    }
                }
            }
        }
    }

    bool draw_true(const unsigned &i, const unsigned &j, const unsigned &k, const Matriz3D &M) const{
        if(tipo==1) return (Ri+((Rf-Ri)/(float)h)*((float)i-((float)Xc-(float)h/2.0-(float)M.Xi))>=sqrt(pow((float)j-((float)Yc-(float)M.Yi),2.0)+pow(((float)k-((float)Zc-(float)M.Zi)), 2.0)));
        else if(tipo==2) return (Ri+((Rf-Ri)/(float)h)*((float)j-((float)Yc-(float)h/2.0-(float)M.Yi))>=sqrt(pow((float)i-((float)Xc-(float)M.Xi),2.0)+pow(((float)k-((float)Zc-(float)M.Zi)), 2.0)));
        else if(tipo==3) return (Ri+((Rf-Ri)/(float)h)*((float)k-((float)Zc-(float)h/2.0-(float)M.Zi))>=sqrt(pow((float)j-((float)Yc-(float)M.Yi),2.0)+pow(((float)i-((float)Xc-(float)M.Xi)), 2.0)));
    }


    int get_left_limit() const{
        if(tipo==1)                             return (ceil((float)Xc-(float)h/2.0));
        else if((tipo==2 || tipo==3) && Ri>=Rf) return (ceil((float)Xc-Ri));
        else if((tipo==2 || tipo==3) && Rf>Ri)  return (ceil((float)Xc-Rf));
    }
    int get_right_limit() const{
        if(tipo==1)                             return (ceil((float)Xc+(float)h/2.0));
        else if((tipo==2 || tipo==3) && Ri>=Rf) return (ceil((float)Xc+Ri));
        else if((tipo==2 || tipo==3) && Rf>Ri)  return (ceil((float)Xc+Rf));
    }
    int get_upper_limit() const{
        if(tipo==2)                             return (ceil((float)Yc+(float)h/2.0));
        else if((tipo==1 || tipo==3) && Ri>=Rf) return (ceil((float)Yc+Ri));
        else if((tipo==1 || tipo==3) && Rf>Ri)  return (ceil((float)Yc+Rf));
    }
    int get_bottom_limit() const{
        if(tipo==2)                             return (ceil((float)Yc-(float)h/2.0));
        else if((tipo==1 || tipo==3) && Ri>=Rf) return (ceil((float)Yc-Ri));
        else if((tipo==1 || tipo==3) && Rf>Ri)  return (ceil((float)Yc-Rf));
    }
    int get_front_limit() const{
        if(tipo==3)                             return (ceil((float)Zc+(float)h/2.0));
        else if((tipo==2 || tipo==1) && Ri>=Rf) return (ceil((float)Zc+Ri));
        else if((tipo==2 || tipo==1) && Rf>Ri)  return (ceil((float)Zc+Rf));
    }
    int get_back_limit() const{
        if(tipo==3)                             return (ceil((float)Zc-(float)h/2.0));
        else if((tipo==2 || tipo==1) && Ri>=Rf) return (ceil((float)Zc-Ri));
        else if((tipo==2 || tipo==1) && Rf>Ri)  return (ceil((float)Zc-Rf));
    }
};


class Sculptor{
    private:
        int Xi, Xf, Yi, Yf, Zi, Zf;
        Matriz3D M;
        std::list<object*> X;
        std::list<object*>::iterator it;

        void check_limits(){
            object* prov;
            unsigned i;
            for(it=X.begin(), i=0; it!=X.end(); it++, i++){
                prov=*it;
                if(i==0){
                    Xi=prov->get_left_limit();
                    Yi=prov->get_bottom_limit();
                    Zi=prov->get_back_limit();
                    Xf=prov->get_right_limit();
                    Yf=prov->get_upper_limit();
                    Zf=prov->get_front_limit();
                }
                if(Xi>prov->get_left_limit()) Xi=prov->get_left_limit();
                if(Yi>prov->get_bottom_limit()) Yi=prov->get_bottom_limit();
                if(Zi>prov->get_back_limit()) Zi=prov->get_back_limit();
                if(Xf<prov->get_right_limit()) Xf=prov->get_right_limit();
                if(Yf<prov->get_upper_limit()) Yf=prov->get_upper_limit();
                if(Zf<prov->get_front_limit()) Zf=prov->get_front_limit();
            }
        }
        void generate_space(){
            if(X.front()==NULL){
                cout << "ERROR: nao ha objetos para criar o espaco...\n";
                fflush(stdin); getchar(); system("cls");
            }
            check_limits();
            M.receive_min_limit(Xi, Yi, Zi);
            M.receive_max_limit(Xf, Yf, Zf);
            M.create_space();
        }
    public:
        void add_object(){
            int prov, prov_Xc, prov_Yc, prov_Zc, R, G, B;
            unsigned char prov_R, prov_G, prov_B;
            float prov_A;
            bool provb;
            object *prov_obj;

            cout << "Qual a posicao central do objeto em x, y e z, respectivamente? (inteiros)\n";
            cin >> prov_Xc >> prov_Yc >> prov_Zc;
            do{
                cout << "\nQual a intensidade do Vermelho, Verde e Azul respectivamente? (0 a 255)\n";
                fflush(stdin); cin >> R;
                fflush(stdin); cin >> G;
                fflush(stdin); cin >> B;
                system("cls");
            }while(R<0 || R>255 || G<0 || G>255 || B<0 || B>255);
            prov_R=R;
            prov_G=G;
            prov_B=B;

            do{
                cout << "\nQual o nivel de opacidade do objeto? (0 a 1)\n";
                fflush(stdin); cin >> prov_A;
            }while(prov_A<0 || prov_A>1);

            do{
                provb=true;
                cout << "Que objeto deseja adicionar?\n" <<
                        "1- Paralelepipedo\t3- Elipsoide\n" <<
                        "2- Kamehameha\t\t4-Cone\n";
                cin >> prov;

                if(prov==1){
                    provb=false;
                    unsigned prov_c, prov_h, prov_l;
                    cout << "\nQual o comprimento, altura e largura, respectivamente?\n";
                    fflush(stdin); cin >> prov_c >> prov_h >> prov_l;
                    prov_obj=new parale(prov_c, prov_h, prov_l, prov_Xc, prov_Yc, prov_Zc, prov_R, prov_G, prov_B, prov_A);
                }else if(prov==2){
                    provb=false;
                    float prov_radius;
                    cout << "\nQual o tamanho do raio?\n";
                    fflush(stdin); cin >> prov_radius;
                    if(prov_radius<0) prov_radius=-prov_radius;
                    prov_obj=new kamehameha(prov_radius, prov_Xc, prov_Yc, prov_Zc, prov_R, prov_G, prov_B, prov_A);
                }else if(prov==3){
                    provb=false;
                    float prov_a, prov_b, prov_c;
                    cout << "Qual o raio no eixo x, y e z respectivamente\n";
                    fflush(stdin); cin >> prov_a >> prov_b >> prov_c;
                    if(prov_a<0) prov_a=-prov_a;
                    if(prov_b<0) prov_b=-prov_b;
                    if(prov_c<0) prov_c=-prov_c;
                    prov_obj=new elipsoid(prov_a, prov_b, prov_c, prov_Xc, prov_Yc, prov_Zc, prov_R, prov_G, prov_B, prov_A);
                }else if(prov==4){
                    provb=false;
                    unsigned prov_h;
                    float prov_Ri, prov_Rf;
                    unsigned prov_tipo;
                    do{
                        cout << "Qual o tipo do Cone\n" <<
                                "1- altura ao longo do X\n" <<
                                "2- altura ao longo do Y\n" <<
                                "3- altura ao longo do Z\n";
                        fflush(stdin); cin >> prov_tipo;
                    }while(prov_tipo<1 || prov_tipo>3);
                    cout << "Qual o Raio inicial e Raio final respectivamente?\n";
                    fflush(stdin); cin >> prov_Ri;fflush(stdin); cin >> prov_Rf;
                    cout << "Qual a altura do cone?\n";
                    fflush(stdin); cin >> prov_h;
                    if(prov_Ri<0) prov_Ri=-prov_Ri;
                    if(prov_Rf<0) prov_Rf=-prov_Rf;
                    prov_obj=new cone(prov_tipo, prov_h, prov_Ri, prov_Rf, prov_Xc, prov_Yc, prov_Zc, prov_R, prov_G, prov_B, prov_A);
                }else{
                    cout << "Error: Tipo inválido\n";
                    getchar();
                }
                system("cls");
            }while(provb);
            X.push_back(prov_obj);
            cout << "Objeto adicionado com sucesso!!!\n"; getchar();
            system("cls");
        }
        Sculptor() : Xi(0), Xf(0), Yi(0), Yf(0), Zi(0), Zf(0){}
        ~Sculptor(){X.clear(); Xi=0; Xf=0; Yi=0; Yf=0; Zi=0; Zf=0;}

        void print_list(){
            object* prov;
            unsigned i;
            for(it=X.begin(), i=0; it!=X.end(); it++, i++){
                prov=*it;
                cout << i << ") " << prov->name << endl;
                cout << "Center( " << prov->Xc << ", " << prov->Yc << ", " << prov->Zc << ")\n";
                cout << "Colors (RGBA): " << "( " << prov->R << ", "<< prov->G << ", "<< prov->B << ", "<< prov->A << ")\n\n";
            }
            fflush(stdin); getchar();
        }

        void draw(){
            generate_space();

            object *prov, *prov2;
            unsigned i, j;
            int b, c;
            std::list<object*>::iterator it_prov;
            for(it=X.begin(), i=0; it!=X.end(); it++, i++){
                prov=*it;

                do{
                    cout << "Objeto " << i+1 << " (" << prov->name << ")\n";
                    cout << "Deseja:\n1- Desenhar\t\t\t3- Desenhar incompletamente\n2- Desenhar interseccionando\t4- Nada\n";
                    cin >> b;
                }while(b<1 || b>4);

                if(b==1) prov->draw(M);
                else if(b==2 || b==3){
                    do{
                        cout << "\nDeseja fazer ";
                        b==2 ? cout << "interseccionar" : cout << "retirada";
                        cout << " com qual dos objetos abaixo\n";
                        for(it_prov=X.begin(), j=0; it_prov!=X.end(); it++, j++){
                            if(it_prov!=it){
                                prov2=*it_prov;
                                cout << j << ") " << prov2->name << endl;
                            }
                        }
                        cin >> c;
                        system("cls");
                    }while(c<0 || c>X.size() || c==i);
                    it_prov=X.begin(); advance(it_prov,c);
                    prov2=*it_prov;
                    b==2 ? prov->draw_intersection(M,*prov2) : prov->draw_minus(M,*prov2);
                }
                system("cls");
            }

        }
        void save_obj(string a){M.save_OFF(a);}

        void create_hand(){
            object *prov, *prov2;

            int x,y,z;
            unsigned char R=232, G=70, B=45;
            float hand_size;

            cout << "Tamanho da mao?\n";
            cin >> hand_size;

            ///Base da Mão
            prov=new parale(hand_size*10,hand_size*3,hand_size*14,0,ceil(hand_size*2.8),0,R,G,B,1);
            X.push_back(prov);
            prov=new cone(3,hand_size*14,ceil(hand_size*1.5),ceil(hand_size*1.5),hand_size*(-5),ceil(hand_size*2.8),0,R,G,B,1);
            X.push_back(prov);

            ///Dedão
            prov=new elipsoid(hand_size*1,hand_size*5,hand_size*5,hand_size*5,hand_size*(-1.8),hand_size*(-6),R,G,B,1);
            X.push_back(prov);
            prov=new parale(hand_size*1.2,hand_size*10,hand_size*7,hand_size*5,ceil(hand_size*(-0.7)),hand_size*(-3.5),R,G,B,1);
            X.push_back(prov);
            prov=new elipsoid(hand_size*2.4,hand_size*3.6,hand_size*3.6,hand_size*4,hand_size*0,hand_size*(-2),R,G,B,1);
            X.push_back(prov);


            ///Dedo mindinho
            prov=new elipsoid(hand_size*(1.8),hand_size*(1.8),hand_size*(1.8),hand_size*(-5),hand_size*(2.8),hand_size*(7),R,G,B,1);
            X.push_back(prov);

            prov=new cone(2, hand_size*2.1,hand_size*1.5, hand_size*1.2,hand_size*(-5),hand_size*1,hand_size*7,R,G,B,1);
            X.push_back(prov);
            prov=new cone(2, hand_size*2.1,hand_size*1.5, hand_size*1.5,hand_size*(-5),hand_size*(-1.1),hand_size*7,R,G,B,1);
            X.push_back(prov);
            prov=new cone(2, hand_size*2.1,hand_size*1.2, hand_size*1.5,hand_size*(-5),hand_size*(-3.1),hand_size*7,R,G,B,1);
            X.push_back(prov);

            prov=new elipsoid(hand_size*(1.3),hand_size*(1.2),hand_size*(1.2),hand_size*(-5),hand_size*(-4.1),hand_size*(7),R,G,B,1);
            X.push_back(prov);
            prov=new cone(3,hand_size*4.2,hand_size*1.2,hand_size*1.2,hand_size*(-5),hand_size*(-4.1),hand_size*(5.6),R,G,B,1);
            X.push_back(prov);

            ///Dedo anelar
            prov=new elipsoid(hand_size*(2.1),hand_size*(2.1),hand_size*(2.1),hand_size*(-2.2),hand_size*(2.8),hand_size*(7),R,G,B,1);
            X.push_back(prov);

            prov=new cone(2, hand_size*2.7,hand_size*1.8, hand_size*1.5,hand_size*(-2.2),hand_size*1,hand_size*7,R,G,B,1);
            X.push_back(prov);
            prov=new cone(2, hand_size*2.7,hand_size*1.8, hand_size*1.8,hand_size*(-2.2),hand_size*(-1.7),hand_size*7,R,G,B,1);
            X.push_back(prov);
            prov=new cone(2, hand_size*2.7,hand_size*1.5, hand_size*1.8,hand_size*(-2.2),hand_size*(-4.2),hand_size*7,R,G,B,1);
            X.push_back(prov);

            prov=new elipsoid(hand_size*(1.7),hand_size*(1.5),hand_size*(1.5),hand_size*(-2.2),hand_size*(-5.6),hand_size*(7),R,G,B,1);
            X.push_back(prov);
            prov=new cone(3,hand_size*4.2,hand_size*1.5,hand_size*1.5,hand_size*(-2.2),hand_size*(-5.6),hand_size*(5.5),R,G,B,1);
            X.push_back(prov);


            ///Dedo medio
            prov=new elipsoid(hand_size*(2.2),hand_size*(2.5),hand_size*(2.5),hand_size*(1.2),hand_size*(2.8),hand_size*(7),R,G,B,1);
            X.push_back(prov);

            prov=new cone(2, hand_size*2.9,hand_size*2.1, hand_size*1.8,hand_size*1.2,hand_size*1,hand_size*7,R,G,B,1);
            X.push_back(prov);
            prov=new cone(2, hand_size*2.9,hand_size*2.1, hand_size*2.1,hand_size*1.2,hand_size*(-1.9),hand_size*7,R,G,B,1);
            X.push_back(prov);
            prov=new cone(2, hand_size*2.9,hand_size*1.6, hand_size*2.1,hand_size*1.2,hand_size*(-4.7),hand_size*7,R,G,B,1);
            X.push_back(prov);

            prov=new elipsoid(hand_size*(1.8),hand_size*(1.6),hand_size*(1.6),hand_size*(1.2),hand_size*(-6.2),hand_size*(7),R,G,B,1);
            X.push_back(prov);
            prov=new cone(3,hand_size*4.2,hand_size*1.6,hand_size*1.6,hand_size*1.2,hand_size*(-6.2),hand_size*(5.4),R,G,B,1);
            X.push_back(prov);


            ///Dedo indicador
            prov=new elipsoid(hand_size*(1.9),hand_size*(2.1),hand_size*(2.1),hand_size*(4.2),hand_size*(2.8),hand_size*(7),R,G,B,1);
            X.push_back(prov);

            prov=new cone(2, hand_size*2.7,hand_size*1.8, hand_size*1.5,hand_size*4.2,hand_size*1,hand_size*7,R,G,B,1);
            X.push_back(prov);
            prov=new cone(2, hand_size*2.7,hand_size*1.8, hand_size*1.8,hand_size*4.2,hand_size*(-1.7),hand_size*7,R,G,B,1);
            X.push_back(prov);
            prov=new cone(2, hand_size*2.7,hand_size*1.5, hand_size*1.8,hand_size*4.2,hand_size*(-4.2),hand_size*7,R,G,B,1);
            X.push_back(prov);

            prov=new elipsoid(hand_size*(1.6),hand_size*(1.5),hand_size*(1.5),hand_size*(4.2),hand_size*(-5.6),hand_size*(7),R,G,B,1);
            X.push_back(prov);
            prov=new cone(3,hand_size*4.2,hand_size*1.4,hand_size*1.4,hand_size*4.2,hand_size*(-5.6),hand_size*(5.5),R,G,B,1);
            X.push_back(prov);
            prov=new elipsoid(hand_size*(1.7),hand_size*(3.8),hand_size*(1.7),hand_size*(4.2),hand_size*(2.3),hand_size*(4.7),R,G,B,1);
            X.push_back(prov);




            generate_space();

            for(it=X.begin(); it!=X.end(); it++){
                prov=*it;
                prov->draw(M);
            }
            string smt;
            cout << "\nNome do arquivo:\n";
            cin >> smt;

            save_obj(smt);
        }
};

int main()
{
    Sculptor a;
    int escolha;
    bool sim=true;
    string arq;

    do{
        cout << "O que desejas fazer?\n";
        cout << "1. Adicionar objetos\t4. Salvar arquivo\n";
        cout << "2. Mostrar lista\t5. Desenhar Mao\n";
        cout << "3. Desenhar no Espaco\t6. Sair\n";
        fflush(stdin); cin >> escolha;
        system("cls");

        switch(escolha){
            case 1:
                a.add_object();
                break;
            case 2:
                a.print_list();
                break;
            case 3:
                a.draw();
                break;
            case 4:
                cout << "Qual o nome pro seu arquivo?\n";
                fflush(stdin); cin >> arq;
                a.save_obj(arq);
                break;
            case 5:
                a.create_hand();
                break;
            case 6:
                sim=false;
                break;
            default:
                break;
        }
        system("cls");
    }while(sim);
    cout << "Obrigado por usar meu programa ;D";
    fflush(stdin); getchar();

    return 0;
}
