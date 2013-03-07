#include "utility.h"
#include "data_structures.h"
#include "data_io.h"
#include "d8_methods.h"
#include "flat_resolution.h"
#include <cstdio>
#include <string>
#include <sys/time.h>

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
	int_2d flat_resolution_mask, groups;
	resolve_flats_barnes(elevations,flowdirs,flat_resolution_mask,groups);
  flat_resolution_timer.stop();
	d8_flow_flats(flat_resolution_mask,groups,flowdirs);

	output_ascii_data("out_barnes",flowdirs,0);
  write_arrows("out_barnes_arrows",flowdirs);

  overall.stop();
  printf("Barnes algorithm took %f seconds overall.\n",overall.accumulated());
  printf("%f seconds was used to resolve flats.\n",flat_resolution_timer.accumulated());

	return 0;
}