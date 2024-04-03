/**
 * Main function for the parking management system.
 * @file main.c
 * @author ist1106369 Sophia Alencar
*/

#include "aux.h"

/** Main funtion of the management system that reads
 * commands from stdin and and calls the corresponding functions.
 * @return Returns 0 upon successful completion.
 */
int main() {
	char command, *buffer;
	args *parking_args;

	/** Memory allocations. */
	if((buffer = (char*)malloc(sizeof(char) * BUFSIZ + 1)) == NULL){
        fprintf(stderr,"Memory allocation of buffer failed\n");
        return 0;
	} 

	if((parking_args = (args *)malloc(sizeof(args))) == NULL){
        fprintf(stderr,"Memory allocation of parking args failed\n");
        return 0;
	}
	
	/** Initialize arguments. */
	initialize_parking_list(parking_args);
	initialize_hashtable(parking_args);

	/** Loop until 'q' command is encountered. */
	while((command = getchar()) != 'q') {
		fgets(buffer, BUFSIZ,stdin);
		switch (command) {

			case 'p':
				list_or_add_park(parking_args, buffer);
				break;

			case 'e':
				register_entry(parking_args, buffer);
				break;

			case 's':
				register_exit(parking_args,buffer);
				break;

			case 'v':
				list_vehicle_logs(parking_args, buffer);
				break;

			case 'f':
				show_revenue(parking_args, buffer);
				break;

			case 'r':
				remove_parking(parking_args,buffer);
				break;

			default:
				break;
		}
	}

	/** Free memory of used variables to avoid memory leaks. */
	free_parking_args(parking_args);
	free(buffer);
	return 0;
}
