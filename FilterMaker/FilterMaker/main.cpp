#include "mainwindow.h"
#include <QApplication>
//#include "filter.h"
//using namespace ars;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
/*
    unsigned int len(10);
    AverageFilter averFilter(20,len);
    DifferentialFilter    difFilter(20,len);
    vector<double> in_vec,out_vec(len);
    in_vec.push_back(1.0f);
    for(unsigned int i=0;i<len-1;++i)
        in_vec.push_back(0.0f);


    //transform(in_vec.cbegin(),in_vec.cend(),out_vec.begin(),averFilter);
    transform(in_vec.cbegin(),in_vec.cend(),out_vec.begin(),difFilter);

    cout<<"Input:\t\t";
    copy(in_vec.cbegin(),in_vec.cend(),ostream_iterator<double>(cout," "));
    cout<<endl;
    cout<<"Output:\t\t";
    copy(out_vec.cbegin(),out_vec.cend(),ostream_iterator<double>(cout," "));
    cout<<endl;
*/
    return a.exec();
}
