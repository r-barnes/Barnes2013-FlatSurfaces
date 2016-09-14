#include "richdem/common/Array2D.hpp"
#include "richdem/common/timer.hpp"
#include "richdem/methods/d8_methods.hpp"
#include "richdem/flats/flat_resolution.hpp"
#include "garbrecht.hpp"
#include <iostream>
#include <string>


//Generate a square DEM with a high wall around its edge and a single outlet in
//the lower left corner. This arrangement is an adversarial input for the
//Garbrecht algorithm.
void GenerateDEM(int size, std::string outputname, std::string analysis){
  Array2D<uint8_t> dem(size,size,6);
  for(unsigned int i=0;i<dem.size();i++)
    if(dem.isEdgeCell(i))
      dem(i) = 9;

  dem(2,size-1) = 5;

  dem.saveGDAL(outputname,analysis);
}








template<class elev_t>
int PerformAlgorithm(char option, std::string filename, std::string output_name, std::string analysis){
  Timer overall,flat_resolution_timer;
  overall.start();

	Array2D<elev_t> elevations(filename,false);

  Array2D<d8_flowdir_t> flowdirs(elevations);

	d8_flow_directions(elevations,flowdirs);

  flat_resolution_timer.start();

  if(option=='1'){
  	Array2D<int32_t> flat_resolution_mask, labels;
  	resolve_flats_barnes(elevations,flowdirs,flat_resolution_mask,labels);
    std::cerr<<"t Seconds used to resolve flats = "<<flat_resolution_timer.lap()<<std::endl;

    //If you want to leave the DEM unaltered, use this command
    d8_flow_flats(flat_resolution_mask,labels,flowdirs);
    std::cerr<<"t Seconds used to resolve flats and determine flow directions = "<<flat_resolution_timer.lap()<<std::endl;
  } else if(option=='2'){
    garbrecht_flat_type flats;
    Garbrecht_FindFlats(flowdirs,flats);
    Array2D<int32_t> inc1;
    Array2D<int32_t> inc2;
    Garbrecht_GradientTowardsLower  (elevations, flowdirs, flats, inc1);
    Garbrecht_GradientAwayFromHigher(elevations, flowdirs, flats, inc2);
    Garbrecht_CombineGradients(elevations, inc1, inc2, 0.001);
    flat_resolution_timer.stop();
    d8_flow_directions(elevations,flowdirs);
  } else if(option=='3'){
    //If you want to alter the DEM to enforce drainage, use these commands instead

    //flowdirs.init(-4);
    //d8_flats_alter_dem(flat_resolution_mask, labels, elevations);
    //d8_flow_directions(elevations,flowdirs);
  }


//  write_arrows("out_barnes_arrows",flowdirs);

  //TODO: Flat resolution time
  flowdirs.saveGDAL(output_name,analysis);
  std::cerr<<"t Wall-time = "<<overall.stop()<<std::endl;




  return 0;
}




template< typename... Arguments >
int Router(std::string inputfile, Arguments ... args){
  switch(peekGDALType(inputfile)){
    case GDT_Byte:
      return PerformAlgorithm<uint8_t >(args...);
    case GDT_UInt16:
      return PerformAlgorithm<uint16_t>(args...);
    case GDT_Int16:
      return PerformAlgorithm<int16_t >(args...);
    case GDT_UInt32:
      return PerformAlgorithm<uint32_t>(args...);
    case GDT_Int32:
      return PerformAlgorithm<int32_t >(args...);
    case GDT_Float32:
      return PerformAlgorithm<float   >(args...);
    case GDT_Float64:
      return PerformAlgorithm<double  >(args...);
    case GDT_CInt16:
    case GDT_CInt32:
    case GDT_CFloat32:
    case GDT_CFloat64:
      std::cerr<<"Complex types are unsupported. Sorry!"<<std::endl;
      return -1;
    case GDT_Unknown:
    default:
      std::cerr<<"Unrecognised data type: "<<GDALGetDataTypeName(peekGDALType(inputfile))<<std::endl;
      return -1;
  }
}

int main(int argc, char **argv){
  std::string analysis = PrintRichdemHeader(argc, argv);

  if(argc!=3 && argc!==4){
    std::cout<<"Syntax: "<<argv[0]<<" gentest <SIZE> <OUTPUT NAME>\n";
    std::cout<<"\tGenerates a test DEM\n\n";
    std::cout<<"Syntax: "<<argv[0]<<" <ALGORITHM> <INPUT DEM> <OUTPUT NAME>\n";
    std::cout<<"Algorithms:\n";
    std::cout<<"\t1: Barnes et al. (2014) Flat Resolution Algorithm\n";
    std::cout<<"\t2: Garbtecht and Martz (1997) Flat Resolution Algorithm\n";
    return -1;
  }

  if(argv[1]==std::string("gentest") && argc==3){
    GenerateDEM(std::stoi(argv[2]),argv[3],analysis);
  } else if(argc==4) {
    Router(argv[2],argv[1][0],argv[2],argv[3],analysis);    
  }

  return 0;
}
