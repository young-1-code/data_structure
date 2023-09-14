#include <stdint.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <ctime>

using namespace std;

int main(void)
{
    uint8_t buffer[1024]={0};
    srand(time(NULL));
    ofstream wfile("./src.dat", ios::out|ios::binary|ios::trunc);

    int cnt=1024*500; //500M
    while(cnt--){
        for(int i=0; i<1024; i++){
            buffer[i]=rand()%256;
        }
        wfile.write((char*)buffer, sizeof(buffer));
    }
    
    wfile.close();

    return 0;
}