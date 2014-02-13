#ifndef SMMAP_H
#define SMMAP_H
#include <grid/map.h>
#include <grid/harray2d.h>
#include <utils/point.h>
#include <stdio.h>
#define SIGHT_INC 1
// Change value to int to include glass case Jack Wang 21/10/2013
// true = 1, false = 0, glass =-1
namespace GMapping {

  struct PointAccumulator{
    typedef point<float> FloatPoint;
    /* before
    PointAccumulator(int i=-1): acc(0,0), n(0), visits(0){assert(i==-1);}
    */
    /*after begin*/
    PointAccumulator(): acc(0,0), n(0), visits(0), isglass(false) {}
    PointAccumulator(int i): acc(0,0), n(0), visits(0), isglass(false) {assert(i==-1);}
    /*after end*/
  inline void update(int value, const Point& p=Point(0,0));
  inline Point mean() const {return 1./n*Point(acc.x, acc.y);}
  inline operator double() const {
    if (isglass) {
      printf("glass detected n = %d v = %d \n", n, visits);
//      if (n != visits)  printf("glass detected n = %d v = %d \n", n, visits);
      return -0.3;
    }
    else {
      return visits?(double)n*SIGHT_INC/(double)visits:-1;
    }
  }
  inline void add(const PointAccumulator& p) {acc=acc+p.acc; n+=p.n; visits+=p.visits;
  if (p.isglass) isglass = p.isglass; }
  static const PointAccumulator& Unknown();
  static PointAccumulator* unknown_ptr;
  FloatPoint acc;
  int n, visits;
  bool isglass;
  inline double entropy() const;
};

void PointAccumulator::update(int value, const Point& p)
{
    if (value == 1) {
      acc.x+= static_cast<float>(p.x);
      acc.y+= static_cast<float>(p.y);
      n++;
      visits += SIGHT_INC;
      isglass = false;
      double x = (double) n / (double)visits;
      if (visits>0 && x < 0.25 && x > 0.0) {
        isglass = true;
      }
    }
/*    else if (value == -1) { // glass cases registration
      double x = (double) n / (double)visits;
      if (n < 2 || x < 0.25) {
//        if (n != visits)  printf("???glass detected n = %d v = %d \n", n, visits);
//        printf("???glass detected n = %d v = %d x = %f\n", n, visits, x);
        isglass = true;
      }
    }
*/    else { // no object
      visits++;
    }
  }

  double PointAccumulator::entropy() const{
          if (!visits)
                  return -log(.5);
          if (n==visits || n<1)
                  return 0;
          double x=(double)n*SIGHT_INC/(double)visits;
          return -( x*log(x)+ (1-x)*log(1-x) );
  }


  typedef Map<PointAccumulator,HierarchicalArray2D<PointAccumulator> > ScanMatcherMap;

};

#endif 
