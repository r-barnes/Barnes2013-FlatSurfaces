#ifndef _d8_methods_included
#define _d8_methods_included

#include "data_structures.h"
#include <queue>
#ifdef _OPENMP
  #include <omp.h>
#endif

//234
//105
//876
//d8_FlowDir
/**
  @brief  Calculates the D8 flow direction of a cell
  @author Richard Barnes (rbarnes@umn.edu)

  This calculates the D8 flow direction of a cell using the D8
  neighbour system, as defined in utility.h. Cells on the edge
  of the grid flow off the nearest edge.

  Helper function for d8_flow_directions().

  @param[in]  &elevations  A DEM
  @param[in]  x            x coordinate of cell
  @param[in]  y            y coordinate of cell

  @returns The D8 flow direction of the cell
*/
template<class T>
static int d8_FlowDir(const array2d<T> &elevations, const int x, const int y){
  T minimum_elevation=elevations(x,y);
  int flowdir=NO_FLOW;

  if (elevations.edge_grid(x,y)){
    if(x==0 && y==0)
      return 2;
    else if(x==0 && y==elevations.height()-1)
      return 8;
    else if(x==elevations.width()-1 && y==0)
      return 4;
    else if(x==elevations.width()-1 && y==elevations.height()-1)
      return 6;
    else if(x==0)
      return 1;
    else if(x==elevations.width()-1)
      return 5;
    else if(y==0)
      return 3;
    else if(y==elevations.height()-1)
      return 7;
  }

  /*NOTE: Since the very edges of the DEM are defined to always flow outwards,
  if they have defined elevations, it is not necessary to check if a neighbour
  is IN_GRID in the following
  NOTE: It is assumed that the no_data datum is an extremely negative
  number, such that all water which makes it to the edge of the DEM's region
  of defined elevations is sucked directly off the grid, rather than piling up
  on the edges.*/
  for(int n=1;n<=8;n++)
    if(
      elevations(x+dx[n],y+dy[n])<minimum_elevation
      || (elevations(x+dx[n],y+dy[n])==minimum_elevation
            && flowdir>0 && flowdir%2==0 && n%2==1)
    ){
      minimum_elevation=elevations(x+dx[n],y+dy[n]);
      flowdir=n;
    }

  return flowdir;
}



//d8_flow_directions
/**
  @brief  Calculates the D8 flow directions of a DEM
  @author Richard Barnes (rbarnes@umn.edu)

  This calculates the D8 flow directions of a DEM. Its argument
  'flowdirs' will return a grid with flow directions using the D8
  neighbour system, as defined in utility.h. The choice of data type
  for array2d must be able to hold exact values for all neighbour
  identifiers (usually [-1,7]).

  Uses d8_FlowDir() as a helper function.

  @todo                    Combine dinf and d8 neighbour systems

  @param[in]  &elevations  A DEM
  @param[out] &flowdirs    Returns the flow direction of each cell
*/
template<class T, class U>
void d8_flow_directions(
  const array2d<T> &elevations,
  array2d<U> &flowdirs
){
  ProgressBar progress;

  diagnostic("Setting up the flow directions matrix...");
  flowdirs.copyprops(elevations);
  flowdirs.init(NO_FLOW);
  flowdirs.no_data=d8_NO_DATA;
  diagnostic("succeeded.\n");

  diagnostic("%%Calculating D8 flow directions...\n");
  progress.start( elevations.width()*elevations.height() );
  #pragma omp parallel for
  for(int x=0;x<elevations.width();x++){
    progress.update( x*elevations.height() );
    for(int y=0;y<elevations.height();y++)
      if(elevations(x,y)==elevations.no_data)
        flowdirs(x,y)=flowdirs.no_data;
      else
        flowdirs(x,y)=d8_FlowDir(elevations,x,y);
  }
  diagnostic_arg(SUCCEEDED_IN,progress.stop());
}

#endif
