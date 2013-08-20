#include "utility.hpp"
#include "data_structures.hpp"
#include "data_io.hpp"
#include "d8_methods.hpp"
#include <deque>
using namespace std;

typedef deque<grid_cell> flat_type;

void FindFlats(const char_2d &flowdirs, flat_type &flats){
	for(int x=0;x<flowdirs.width();++x)
	for(int y=0;y<flowdirs.height();++y)
		if(flowdirs(x,y)==NO_FLOW)
			flats.push_back(grid_cell(x,y));
}

int GradientTowardsLower(const float_2d &elevations, const char_2d &flowdirs, flat_type &flats, int_2d &inc1){
	int loops=0;
	int number_incremented=-1;
	int nx,ny;
  diagnostic("Setting up the inc1 matrix...");
  inc1.copyprops(elevations);
  inc1.init(0);
  diagnostic("succeeded.\n");
	while(number_incremented!=0){
		number_incremented=0;
		for(int i=0;i<flats.size();i++){
			bool increment_elevation=true;
			int x=flats[i].x;
			int y=flats[i].y;
			for(int n=1;n<=8;n++){
				if(	elevations(x+dx[n],y+dy[n])<elevations(x,y) && 
					flowdirs(x+dx[n],y+dy[n])!=NO_FLOW && flowdirs(x+dx[n],y+dy[n])!=flowdirs.no_data
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

int GradientAwayFromHigher(const float_2d &elevations, const char_2d &flowdirs, flat_type &flats, int_2d &inc2){
	int loops=0;
	int number_incremented=0;
	int nx,ny;
  diagnostic("Setting up the inc2 matrix...");
  inc2.copyprops(elevations);
  inc2.init(0);
  diagnostic("succeeded.\n");
	while(number_incremented<flats.size()){
		for(int i=0;i<flats.size();i++){
			int x=flats[i].x;
			int y=flats[i].y;
			if(inc2(x,y)>0){
				inc2(x,y)++;
				continue;
			}
		}
		for(int i=0;i<flats.size();i++){
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

int CombineGradients(float_2d &elevations, const int_2d &inc1, const int_2d &inc2, float epsilon){
  diagnostic("Combining the gradients...");
	for(int x=0;x<elevations.width();++x)
	for(int y=0;y<elevations.height();++y)
		elevations(x,y)+=(inc1(x,y)+inc2(x,y))*epsilon;
  diagnostic("succeeded.\n");
}

int main(int argc, char **argv){
	if(argc!=2){
		printf("%s <INPUT DEM>\n",argv[0]);
		return -1;
	}
  Timer overall,flat_resolution_timer;
  overall.start();

	float_2d elevations;
	load_ascii_data(argv[1],elevations);

	char_2d flowdirs;
	d8_flow_directions(elevations,flowdirs);

  flat_resolution_timer.start();
	flat_type flats;
	FindFlats(flowdirs,flats);
	int_2d inc1;
	int_2d inc2;
	GradientTowardsLower  (elevations, flowdirs, flats, inc1);
	GradientAwayFromHigher(elevations, flowdirs, flats, inc2);
	CombineGradients(elevations, inc1, inc2, 0.001);
  flat_resolution_timer.stop();
	d8_flow_directions(elevations,flowdirs);

	output_ascii_data("out_garbrecht",flowdirs,0);
  write_arrows("out_garbrecht_arrows",flowdirs);

	overall.stop();
	printf("Garbrecht algorithm took %f seconds overall.\n",overall.accumulated());
  printf("%f seconds was used to resolve flats.\n",flat_resolution_timer.accumulated());
}
