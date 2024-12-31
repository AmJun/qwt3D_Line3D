#ifndef TRACK3D_H
#define TRACK3D_H

#include <QTimer>
#include <qwt3d_surfaceplot.h>
#include <qwt3d_function.h>

using namespace Qwt3D;


class track3D:public Qwt3D::SurfacePlot
{
public:
    track3D();
    void rosenbrockinit();
    void init();
    bool addLine(QString linename,int linewidth);
    bool addPoint(QString pointName, int color,int lineWidth);
    bool updatePointData(QString pointName,Qwt3D::Triple point);
    bool deleteLine(QString linename);
    bool deleteAllLine();
    bool addData(QString linename,Qwt3D::Triple point);
    void setCurMaxMin();
    void replot();
    Qwt3D::RGBA colorToRGBA(const QColor& qtColor);

    Qwt3D::SurfacePlot plot;
    //地图  管理航迹
    QMap<QString,Qwt3D::Line3D*> mLine;
    //航迹  管理大小
    QMap<QString,int> mLineSize;
//    QMap<QString, QVector<Line3D>> mLine;
//    std::vector<Line3D> testData;
    QVector<Line3D> testData;
    int colorindex;
    QVector<QColor> baseColors = {QColor(Qt::red), QColor(Qt::blue), QColor(Qt::black),QColor(Qt::green), /* 其他颜色 */};
    //
    double myCurxMin;
    double myCurxMax;
    double myCuryMin;
    double myCuryMax;
    double myCurzMin;
    double myCurzMax;
    //
    //初始化时的xy轴范围   z轴由函数决定
//    double myxMin=0;
//    double myxMax=0.085*8;
//    double myyMin=0;
//    double myyMax=0.303*2.3;
//    double myxMin=119.2;
//    double myxMax=119.5;
//    double myyMin=32.6;
//    double myyMax=33;
    double myxMin=101.2;
    double myxMax=101.6;
    double myyMin=32.6;
    double myyMax=33;
    //限制绘画框z轴的最大范围
    double myzMin=0;
    double myzMax=1000;


    //数据点上限
    int m_limit=50;
    bool newpoint;
    //
    int defaultWidth=2;

};



#endif // TRACK3D_H
