//5 Liu Elina
//1/4/2023
//Double Threshold and Hysteresis on Edge Detection in a PPM Image
#include<cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
using namespace std;
int WIDTH;
int HEIGHT;
vector<vector<int>> readppm(string filename, vector<vector<int>> &im){
    //input ppm filename and vector, gives the intensity vector in dimensions [height][width]
    ifstream f;
    f.open(filename);
    string place;
    f >> place;
    int width, height;
    f>> width >> height;
    WIDTH= width; HEIGHT = height;
    f>>place;
    for(int i =0; i<height;i++){
        vector<int> innervec;
        for(int j = 0; j<width; j++){
            int avg= 0;
            for(int k = 0;k<3;k++){
                f >> place;
                avg += stoi(place);
            }
            innervec.push_back(avg/3); //the intensity
        }
        im.push_back(innervec);
    }
    f.close();
    return im;
}
void outppm(string filename, vector<vector<int>> &im){ //greyscale turns im to ppm 3number things
    ofstream f;
    f.open(filename);
    f<<"P3 "<< WIDTH<< " "<<HEIGHT<< " 255\n";
    for(int i = 0;i<HEIGHT;i++){
        for(int j = 0;j<WIDTH;j++){
            f<<im[i][j]<<" "<<im[i][j]<<" "<<im[i][j]<<" ";
        }
        f<<"\n";
    }
    f.close();
    
}
void make_edge_ppm(string filename, vector<vector<int>> &edges){ //for the final edges printing
    ofstream f;
    f.open(filename);
    f<<"P3 "<< WIDTH<< " "<<HEIGHT<< " 1\n";
    for(int i=0;i<HEIGHT;i++){
        for(int j=0;j<WIDTH;j++){
            int val = edges[i][j];
            f<<val<<" "<<val<<" "<<val<<" ";
        }
        f<<"\n";
    }
    f.close();
}
void padzero(vector<vector<int>> &im){
    for(int i = 0; i<HEIGHT;i++){
        for(int j=0;j<WIDTH;j++){
            if(i==0 or i==HEIGHT-1 or j==0 or j==WIDTH-1){im[i][j]=0;}
        }
    }
}
vector<vector<int>> sobel(vector<vector<int>> &im){
    //generates gy and gx, and returns G (all vector of vectors)
    vector<vector<int>> gx = im;
    vector<vector<int>> gy = im;
    vector<vector<int>> g = im;
    for(int i=1;i<HEIGHT-1;i++){ //avoid borders
        for(int j=1;j<WIDTH-1;j++){
            int gyval = im[i-1][j-1] + 2*im[i-1][j] + im[i-1][j+1]  //gy matrix 
                    -im[i+1][j-1] -2*im[i+1][j] -im[i+1][j+1];
            int gxval = im[i-1][j-1] - im[i-1][j+1] + 2*im[i][j-1] - 2*im[i][j+1] +im[i+1][j-1] -im[i+1][j+1]; //gx matrix
            gx[i][j] = gxval;
            gy[i][j] = gyval;
            
            //create g matrix
            g[i][j] = pow(gxval,2) + pow(gyval,2); //didn't sqrt for efficiency
        }
    }
    return g;
}

vector<vector<int>> apply_thresh(int thresh, vector<vector<int>> &g){
    vector<vector<int>> edges = g;
    for(int i=1;i<HEIGHT-1;i++){ //avoid borders
        for(int j=1;j<WIDTH-1;j++){
            if(g[i][j] >= thresh){
                edges[i][j] = 1;
            }
            else{
                edges[i][j] = 0;
            }
        }
    }
    return edges;
}

vector<vector<int>> apply_double_thresh(int lt, int ht, vector<vector<int>> &g){
    vector<vector<int>> edges = g;
    for(int i=1;i<HEIGHT-1;i++){ //avoid borders
        for(int j=1;j<WIDTH-1;j++){
            if(g[i][j] <= lt){
                edges[i][j] = 0;
            }
            else if(g[i][j] <= ht and g[i][j]>lt){
                edges[i][j] = 1;
            }
            else{
                edges[i][j] = 2;
            }
        }
    }
    return edges;
}
void floodit(int i, int j, vector<vector<int>> &threshed){ //recursive part of hysteresis
    if(threshed[i-1][j]==1){
        threshed[i-1][j]=2;
        floodit(i-1, j, threshed);
    }
    if(threshed[i-1][j-1]==1){
        threshed[i-1][j-1]=2;
        floodit(i-1, j-1, threshed);
    }
    if(threshed[i-1][j+1]==1){
        threshed[i-1][j+1]=2;
        floodit(i-1, j+1, threshed);
    }
    if(threshed[i][j-1]==1){
        threshed[i][j-1]=2;
        floodit(i, j+1, threshed);
    }
    if(threshed[i+1][j-1]==1){
        threshed[i+1][j-1]=2;
        floodit(i+1, j-1, threshed);
    }
    else if(threshed[i+1][j]==1){
        threshed[i+1][j]=2;
        floodit(i+1, j, threshed);
    }
    else if(threshed[i+1][j+1]==1){
        threshed[i+1][j+1]=2;
        floodit(i+1, j+1, threshed);
    }
}
void hysteresis(vector<vector<int>> &threshed){
    for(int i=1;i<HEIGHT-1;i++){
        for(int j=1;j<WIDTH-1;j++){
            int val = threshed[i][j];
            if(val==2){
                floodit(i, j, threshed); //recur 
            }
        }
    }
}
void make_hyst_ppm(string filename, vector<vector<int>> &edges){ //for the final edges printing
    ofstream f;
    f.open(filename);
    f<<"P3 "<< WIDTH<< " "<<HEIGHT<< " 1\n";
    for(int i=0;i<HEIGHT;i++){
        for(int j=0;j<WIDTH;j++){
            int val = edges[i][j];
            if(val==1){
                val=0;
            }
            else if(val==2){
                val=1;
            }
            f<<val<<" "<<val<<" "<<val<<" ";
        }
        f<<"\n";
    }
    f.close();
}
void part1(){
    vector<vector<int>> ogimage;
    ogimage = readppm("image.ppm", ogimage);
    padzero(ogimage);
    outppm("imageg.ppm", ogimage); //output the greyscaled ppm
    vector<vector<int>> gmatrix = sobel(ogimage);
    vector<vector<int>> edge_matrix = apply_thresh(20800,gmatrix);
    make_edge_ppm("imagem.ppm",edge_matrix);
}

void part2(int argc, const char* argv[]){ //default infile is image.ppm
    string in = "image.ppm";
    string out = "image1.ppm";
    int lt = 9000;
    int ht = 17800;
    for(int i =0; i<argc; i++){ //-f myimg.ppm -lt 150 -ht 200 -of out.ppm
        if(strcmp(argv[i], "-f") == 0){
            in = argv[i+1];
        }
        else if(strcmp(argv[i],"-lt") == 0){
            lt = pow(atoi(argv[i+1]),2);
        }
        else if(strcmp(argv[i],"-ht") == 0){
            ht = pow(atoi(argv[i+1]),2);
        }
        else if(strcmp(argv[i],"-of") == 0){
            out = argv[i+1];
        }
    }
    
    vector<vector<int>> ogimage;
    ogimage = readppm(in, ogimage);
    padzero(ogimage);
    vector<vector<int>> gmatrix = sobel(ogimage);
    vector<vector<int>> edge_matrix = apply_double_thresh(lt,ht,gmatrix);
    hysteresis(edge_matrix); //make all 1s either 0 or 2, recursive
    make_hyst_ppm(out, edge_matrix);//default image1.ppm, applied hysteresis and double thresh 
}
int main(int argc, const char* argv[]){
  //  part1();
    part2(argc, argv);
}