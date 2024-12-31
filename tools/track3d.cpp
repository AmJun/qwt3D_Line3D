#include "track3d.h"
#include <QDebug>


class Rosenbrock : public Function
{
public:

  Rosenbrock(SurfacePlot& pw)
  :Function(pw)
  {

  }

  double operator()(double x, double y)
  {
//    return 2*x*y;//z轴初始范围与该函数相关
      return 1.25*(y - 32.6);
  }
};


track3D::track3D()
{

    // 设置图表的标题
    setTitle("GPS");

    // 设置绘图样式为线条三维样式
    setPlotStyle(Qwt3D::LINE3D_STYLE);

    // 创建一个Rosenbrock函数对象，并将其与当前SurfacePlot对象关联
    Rosenbrock rosenbrock(*this);
    // 设置网格的分辨率为10x10
    rosenbrock.setMesh(10,10);
    // 设置函数的定义域为x,y都在0到100之间
    rosenbrock.setDomain(myxMin,myxMax,myyMin,myyMax);
    rosenbrock.setMinZ(myzMin);
    rosenbrock.setMaxZ(myzMax);
    //创建三维表面
    rosenbrock.create();

    // 设置坐标轴的网格线显示，仅在左侧、背面和底面显示
//    coordinates()->setGridLines(true,false,Qwt3D::LEFT|Qwt3D::BACK|Qwt3D::FLOOR);
    // 为每个坐标轴设置主刻度和次刻度数量
    for (unsigned i=0; i!=coordinates()->axes.size(); ++i)
    {
        coordinates()->axes[i].setMajors(7); // 主刻度数量
        coordinates()->axes[i].setMinors(4); // 次刻度数量
    }

    // 设置各坐标轴的标签
    coordinates()->axes[X1].setLabelString("经度/°");
    coordinates()->axes[Y1].setLabelString("纬度/°");
    coordinates()->axes[Z1].setLabelString("高度/km"); // 注意：这里的Omega注释可能是指z轴的标签在Unicode中有特殊字符，但此处直接使用"z-axis"

    // 设置坐标轴样式为盒子样式
    setCoordinateStyle(BOX);
    //
//    setCoordinateStyle(FRAME);
    setZoom(0.9);
    setRotation(20, 0, -10.0); // 设置初始旋转角度
    init();
}

void track3D::rosenbrockinit()
{
    //更新坐标轴样式
    Rosenbrock rosenbrock(*this);
    rosenbrock.setDomain(myxMin,myxMax,myyMin,myyMax);
    rosenbrock.setMinZ(myzMin);
    rosenbrock.setMaxZ(myzMax);
    rosenbrock.create();
}
//
void track3D::init()
{
    //初始化
    addLine("firstLine",defaultWidth);
    replot();
}
//添加轨迹线
bool track3D::addLine(QString linename,int linewidth)
{
    //如果没有，则进行添加
        if(!mLine.contains(linename))
        {
            Qwt3D::Line3D _l3d;
            Qwt3D::Line3D *myLine1 = dynamic_cast<Qwt3D::Line3D *>(this->addEnrichment(_l3d));
            myLine1->configure(linewidth,true);
            colorindex = colorindex % 2;
            myLine1->setLineColor(colorToRGBA(baseColors[colorindex]));  //设置默认颜色为红色
            colorindex++;
            mLine.insert(linename,myLine1);
            return true;
        }
        return false;
}

bool track3D::addPoint(QString pointName,int color, int lineWidth)
{
    //实现方法：按照正常线段添加曲线  更新时只保留一个点位
    //如果没有，则进行添加
        if(!mLine.contains(pointName))
        {
            Qwt3D::Line3D _l3d;
            Qwt3D::Line3D *myLine1 = dynamic_cast<Qwt3D::Line3D *>(this->addEnrichment(_l3d));
            myLine1->configure(lineWidth,true);
            myLine1->setLineColor(colorToRGBA(baseColors[color]));
            mLine.insert(pointName,myLine1);
            return true;
        }
        return false;
}
//更新标识线-动态绘制时每次调用该函数使得线段长度固定
bool track3D::updatePointData(QString pointName, Triple point)
{
    //更新点的坐标
    if(mLine.contains(pointName)){
        mLineSize[pointName]+=1;
        auto it = mLine.find(pointName);
        if(mLineSize[pointName] > m_limit){
            //删除原来的数据
            it.value()->clear();
            mLineSize[pointName] = 0;
        }
        //更新点
        it.value()->add(point);
    }

}
//删除单挑轨迹线
bool track3D::deleteLine(QString linename)
{
    if(mLine.contains(linename))   //如果包含
        {
            auto it = mLine.find(linename);
            it.value()->clear();
            this->degrade(it.value());
            mLine.remove(linename);
            return true;
        }
        return false;
}
//删除所有轨迹线
bool track3D::deleteAllLine()
{
    for(auto line:mLine)
        {
            line->clear();   //移除所有曲线数据
            this->degrade(line);
        }
        mLine.clear();  //移除所有曲线
        return true;
}

bool track3D::addData(QString linename, Triple point)
{
    if(mLine.contains(linename))   //如果包含
        {
            //设置最大和最小
            bool nochange = false;
            if (point.x < myCurxMin)
                myCurxMin = point.x;
            else if (point.x > myCurxMax)
                myCurxMax = point.x;
            else if (point.y < myCuryMin)
                myCuryMin = point.y;
            else if (point.y > myCuryMax)
                myCuryMax = point.y;
            else if (point.z < myCurzMin)
                myCurzMin = point.z;
            else if (point.z > myCurzMax)
                myCurzMax = point.z;
            else
                nochange = true;
            if(!nochange)
                setCurMaxMin();
            auto it = mLine.find(linename);
            it.value()->add(point);
//            testData.append(*it.value());
//            //上限处理
//            if(testData.size()>m_limit)
//            {
//                testData.remove(0);
//            }
            newpoint = true;
            return true;
        }
        else
        {
            //没有这条线  创造新的
            if(addLine(linename,defaultWidth))
                return true;
            return false;
    }
}

void track3D::setCurMaxMin()
{
    //动态调整空间范围
    if (myCurxMin < myxMin) myxMin = myCurxMin;
    if (myCurxMax > myxMax) myxMax = myCurxMax;
    if (myCuryMin < myyMin) myyMin = myCuryMin;
    if (myCuryMax > myyMax) myyMax = myCuryMax;
//    if (myCurzMin < myzMin) myzMin = myCurzMin;
//    if (myCurzMax > myzMax) myzMax = myCurzMax;
    rosenbrockinit();
}

void track3D::replot()
{
    std::vector<Line3D> lineContain;
    for(auto line:mLine)
        {
            lineContain.push_back(*line);
            //方法1  单独为线段更新
//            updateData(*line);
//            updateGL();
        }
    //存在两条以上曲线时，必须使用此方法更新数据（参数为vector<Line3D>）
    updateData(lineContain);
    updateGL();

}


//qColor  2 RGBA
RGBA track3D::colorToRGBA(const QColor &qtColor)
{
    Qwt3D::RGBA qwt3DColor;
    qwt3DColor.r = qtColor.redF();  // 获取红色分量并转换为0.0到1.0的范围
    qwt3DColor.g = qtColor.greenF(); // 获取绿色分量并转换为0.0到1.0的范围
    qwt3DColor.b = qtColor.blueF();  // 获取蓝色分量并转换为0.0到1.0的范围
    qwt3DColor.a = qtColor.alphaF(); // 获取透明度分量并转换为0.0到1.0的范围
    return qwt3DColor;
}
