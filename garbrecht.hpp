#ifndef _barnes2013_garbrecht_hpp_
#define _barnes2013_garbrecht_hpp_

#include "richdem/common/grid_cell.hpp"
#include <deque>

typedef std::deque<GridCell> garbrecht_flat_type;

void Garbrecht_FindFlats(const Array2D<d8_flowdir_t> &flowdirs, garbrecht_flat_type &flats){
  for(int x=0;x<flowdirs.width();++x)
  for(int y=0;y<flowdirs.height();++y)
    if(flowdirs(x,y)==NO_FLOW)
      flats.emplace_back(x,y);
}

template<class T>
void Garbrecht_GradientTowardsLower(const Array2D<T> &elevations, const Array2D<d8_flowdir_t> &flowdirs, garbrecht_flat_type &flats, Array2D<int32_t> &inc1){
  int loops     = 0;
  int number_incremented = -1;
  std::cerr<<"Setting up the inc1 matrix..."<<std::endl;
  inc1.resize(elevations);
  inc1.setAll(0);

  while(number_incremented!=0){
    number_incremented=0;
    for(int i=0;i<(int)flats.size();i++){
      bool increment_elevation=true;
      int x=flats[i].x;
      int y=flats[i].y;
      for(int n=1;n<=8;n++){
        if( elevations(x+dx[n],y+dy[n])<elevations(x,y) &&
          flowdirs(x+dx[n],y+dy[n])!=NO_FLOW && !flowdirs.isNoData(x+dx[n],y+dy[n])
        ){
          increment_elevation=false;
          break;
        }
        else if(inc1(x+dx[n],y+dy[n])<loops &&
            elevations(x+dx[n],y+dy[n])==elevations(x,y)
        ){
          increment_elevation=false;
          break;
        }
      }
      if(increment_elevation){
        inc1(x,y)++;
        number_incremented++;
      }
    }
    loops++;
  }
}

template<class T>
void Garbrecht_GradientAwayFromHigher(const Array2D<T> &elevations, const Array2D<d8_flowdir_t> &flowdirs, garbrecht_flat_type &flats, Array2D<int32_t> &inc2){
  int loops              = 0;
  int number_incremented = 0;
  std::cerr<<"Setting up the inc2 matrix..."<<std::endl;
  inc2.resize(elevations);
  inc2.setAll(0);

  while(number_incremented<(int)flats.size()){
    for(int i=0;i<(int)flats.size();i++){
      int x=flats[i].x;
      int y=flats[i].y;
      if(inc2(x,y)>0){
        inc2(x,y)++;
        continue;
      }
    }
    for(int i=0;i<(int)flats.size();i++){
      bool has_higher=false,has_lower=false;
      int x=flats[i].x;
      int y=flats[i].y;
      if(inc2(x,y)>0) continue;
      for(int n=1;n<=8;n++){
        if( !has_higher &&
          (elevations(x+dx[n],y+dy[n])>elevations(x,y) ||
          inc2(x+dx[n],y+dy[n])==2)
        )
          has_higher=true;
        else if( !has_lower &&
            elevations(x+dx[n],y+dy[n])<elevations(x,y)
        )
          has_lower=true;
      }
      if(has_higher && !has_lower){
        inc2(x,y)++;
        number_incremented++;
      }
    }
    loops++;
  }
}

template<class T>
void Garbrecht_CombineGradients(Array2D<T> &elevations, const Array2D<int32_t> &inc1, const Array2D<int32_t> &inc2, float epsilon){
  std::cerr<<"Combining the gradients..."<<std::endl;
  for(unsigned int i=0;i<elevations.size();i++)
    elevations(i) += (inc1(i)+inc2(i))*epsilon;
}




#endif