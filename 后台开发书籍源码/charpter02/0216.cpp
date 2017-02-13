#include <iostream>
using namespace std;
class CBox{
    int length,width,height;
    static int count;
};
class BBox{
	static int count;
};
int main(){
    CBox boxobj;
    cout<<sizeof(boxobj)<<endl;
    cout<<"abc"<<endl;
    cout<<sizeof(BBox)<<endl;
    return 0;
}
