/** File with all the main functions of each command
 * @file proj1.c
 * @author ist1106369 Sophia Alencar
*/

#include "proj1.h"
#include "aux.h"

/** 
 * Lists all parkings in the parking array.
 * @param parking_args Pointer to the args structure 
 * containing parking information.
 */
void list_all_parkings(args *parking_args) {
    int i;
    for (i = 0; i < MAX_PARKINGS; i++) {
        if (parking_args->parkings[i] != NULL) {
            printf("%s %d %d\n", parking_args->parkings[i]->name, 
                parking_args->parkings[i]->max_capacity, 
                parking_args->parkings[i]->free_spaces);
        }
    }
}

/** 
 * Adds a new parking to the parking array.
 * @param parking_args Pointer to the args structure 
 * containing parking information.
 * @param name Name of the new parking.
 * @param max_capacity Maximum capacity of the new parking.
 * @param cost_x Cost for the first 15 minutes periods in the first hour
 * @param cost_y Cost for the first 15 minutes periods after the first hour
 * @param cost_z mas cost for a whole day.
 * @return Returns 0 on success, -1 if parking limit is reached or other errors occur.
 */
int add_new_parking(args *parking_args, char *name, int max_capacity, double cost_x, double cost_y, double cost_z) {

    int i;
    parking *new_parking; 
    /** Check if parking limit has been reached. */
    if (parking_args->park_counter >= MAX_PARKINGS) {
        printf("too many parks.\n");
        return -1;
    }

    /** Check if a parking with the same name already exists. */
    for (i = 0; i < MAX_PARKINGS; i++) {
        if (parking_args->parkings[i] != NULL && 
            strcmp(parking_args->parkings[i]->name, name) == 0) {
            return 1;
        }
    }

    /** Check if maximum capacity and costs are valid. */
    if (max_capacity <= 0 ) {
        printf("%d: invalid capacity.\n", max_capacity);
        return -1;
    } else if (cost_x <= 0 || cost_y <= 0 || cost_z <= 0) {
        printf("invalid cost.\n");
        return -1;
    } else if (cost_x >= cost_y || cost_x >= cost_z || cost_y >= cost_z) {
        printf("invalid cost.\n");
        return -1;
    }

    /** Allocate memory for the new parking. */
    if ((new_parking = (parking *)malloc(sizeof(parking)) )== NULL) {
        printf("Memory allocation of new parking failed.\n");
        return -1;
    }

    /** Initialize new parking details. */
    if ((new_parking->name = (char *)malloc(strlen(name) + 1) )== NULL) {
        printf("Memory allocation of parking name failed.\n");
        return -1;
    }
    strcpy(new_parking->name, name);
    new_parking->max_capacity = max_capacity;
    new_parking->cost_x = cost_x;
    new_parking->cost_y = cost_y;
    new_parking->cost_z = cost_z;
    new_parking->free_spaces = max_capacity;
    new_parking->billing_day_log = NULL;
    new_parking->last_billing_day_log = NULL;

    /** Add the new parking to the parking array. */
    parking_args->parkings[parking_args->park_counter] = new_parking;
    parking_args->park_counter++;

    return 0; 
}

/** 
 * Parses the buffer and either lists all parkings or adds a new parking.
 * @param parking_args Pointer to the args structure containing parking information.
 * @param buffer Input buffer containing command and parameters.
 */
void list_or_add_park(args *parking_args, char *buffer) {
    char *token;
    char *name;
    double cost_x, cost_y, cost_z;

    /** Parse the name from the buffer. */
    name = parse_name(buffer);

    /** If no name is provided, list all parkings. */
    if (name == NULL) {
        list_all_parkings(parking_args);
        return; 
    }

    /** Tokenize the buffer to get max_capacity, 
     * cost_x, cost_y, and cost_z. */
    token = strtok(NULL, DELIMITER);
    if (token == NULL) {
        return;
    }
    int max_capacity = atoi(token);

    token = strtok(NULL, DELIMITER);
    if (token == NULL) {
        return;
    }
    cost_x = atof(token);

    token = strtok(NULL, DELIMITER);
    if (token == NULL) {
        return;
    }
    cost_y = atof(token);

    token = strtok(NULL, DELIMITER);
    if (token == NULL) {
        return;
    }
    cost_z = atof(token);

    /** Add the new parking unless it already exists. */
    if (add_new_parking(parking_args, name, max_capacity, cost_x, cost_y, cost_z) == 1) {
        printf("%s: parking already exists.\n", name);
    }
}


/** 
 * Registers a vehicle entry into the parking system.
 * @param parking_args Pointer to the args structure containing parking information.
 * @param buffer Input buffer containing command and parameters.
 */
void register_entry(args *parking_args, char *buffer) {
    char *name, *license_plate, *date_input, *time_input;
    date *entry_date;
    parking *current_parking;
    vehicle *my_vehicle;

    /** Parse the name from the buffer */
    name = parse_name(buffer);

    /** Parse license plate, date, and time from the buffer */
    license_plate = strtok(NULL, DELIMITER);
    date_input = strtok(NULL, DELIMITER);
    time_input = strtok(NULL, DELIMITER);

    /** Find the parking corresponding to the provided name */
    current_parking = find_parking(parking_args, name);

    /** Parse entry date */
    entry_date = parse_date(date_input, time_input);

    /** Check if parking exists */
    if (current_parking == NULL) {
        printf("%s: no such parking.\n", name);
        free(entry_date);
        return;
    }

    /** Check if parking is full */
    if (current_parking->free_spaces <= 0) {
        printf("%s: parking is full.\n", name);
        free(entry_date);
        return;
    }

    /** Check if license plate is valid */
    if (license_plate == NULL || !is_license_plate_valid(license_plate)) {
        printf("%s: invalid licence plate.\n", license_plate);
        free(entry_date);
        return;
    }

    /** Find the vehicle by license plate */
    my_vehicle = find_vehicle(parking_args->vehicles, license_plate);

    /** Check if vehicle is already parked */
    if (is_vehicle_parked(my_vehicle)) {
        printf("%s: invalid vehicle entry.\n", license_plate);
        free(entry_date);
        return;
    }

    /** Check if date and time are provided */
    if (date_input == NULL || time_input == NULL) {
        printf("invalid date.\n");
        free(entry_date);
        return;
    }

    /** Validate entry date */
    if (!is_date_valid(entry_date, parking_args)) {
        printf("invalid date.\n");
        free(entry_date);
        return;
    }


    /** If vehicle not found, create a new vehicle and a new entry */
    if (my_vehicle == NULL) {
        if ((my_vehicle = (vehicle *)malloc(sizeof(vehicle))) == NULL) {
            printf("Memory allocation of new vehicle failed.\n");
            return;
        }
        if ((my_vehicle->license_plate = (char *)malloc(strlen(license_plate) + 1)) == NULL) {
            printf("%s :Memory allocation of license failed1.\n", license_plate);
            return;
        }

        strcpy(my_vehicle->license_plate, license_plate);
        my_vehicle->parking_name = NULL;
        my_vehicle->is_parked = 0;
        my_vehicle->my_logs = NULL;
        my_vehicle->my_last_log = NULL;
        insert_new_entry(parking_args->vehicles, license_plate, my_vehicle);
    }

    /** Allocate memory for parking name */
    if ((my_vehicle->parking_name = (char *)malloc(strlen(name) + 1)) == NULL) {
        printf("Memory allocation of parking name failed.\n");
        return;
    }

    /** Set parking name in the vehicle entry */
    strcpy(my_vehicle->parking_name, name);


    /** Update vehicle status and parking availability */
    my_vehicle->is_parked = 1;
    current_parking->free_spaces--;

    /** Add entry log */
    add_new_log(my_vehicle, current_parking, entry_date, name);
}

/** 
 * Registers the exit of a vehicle from the parking system.
 * @param parking_args Pointer to the args structure 
 * containing parking information.
 * @param buffer Input buffer containing command and parameters.
 */
void register_exit(args *parking_args, char *buffer) {
    char *name, *license_plate, *date_input, *time_input;
    date *exit_date;
    parking *current_parking;
    vehicle *my_vehicle;
    vehicle_log *current_log;
    double cost = 0.0;

    /** Parse name, license plate, date, and time from the buffer. */
    name = parse_name(buffer);
    license_plate = strtok(NULL, DELIMITER);
    date_input = strtok(NULL, DELIMITER);
    time_input = strtok(NULL, DELIMITER);

    /** Parse exit date. */
    exit_date = parse_date(date_input, time_input);

    /** Find the parking corresponding to the provided name. */
    current_parking = find_parking(parking_args, name);

    /** Find the vehicle by license plate. */
    my_vehicle = find_vehicle(parking_args->vehicles, license_plate);


    /** Validate parking and vehicle. */
    if (current_parking == NULL ) {
        printf("%s: no such parking.\n", name);
        free(exit_date);
        return;
    }

    /** Validate license plate. */
    if (license_plate == NULL || !is_license_plate_valid(license_plate)) {
        printf("%s: invalid licence plate.\n", license_plate);
        free(exit_date);
        return;
    }

    /** Check if vehicle is parked in the specified parking. */
    if ( my_vehicle == NULL  || !is_vehicle_parked(my_vehicle) || 
        strcmp(current_parking->name, my_vehicle->parking_name) != 0) {
        printf("%s: invalid vehicle exit.\n", license_plate);
        free(exit_date);
        return;
    }


    /** Validate date and time inputs. */
    if (date_input == NULL || time_input == NULL) {
        free(exit_date);
        return;
    }

    /** Check if exit date is valid. */
    if (!is_date_valid(exit_date, parking_args)) {
        printf("invalid date.\n");
        free(exit_date);
        return;
    }

    /** Get the last log entry for the vehicle. */
    current_log = my_vehicle->my_last_log;

    /** Update the exit date in the vehicle vehiclelog entry. */
    current_log->exit_date = exit_date;

    /** Update vehicle status. */
    my_vehicle->is_parked = 0;
    free(my_vehicle->parking_name);
    my_vehicle->parking_name = NULL;

    /** Increase free spaces in the parking. */
    current_parking->free_spaces++;

    /** Calculate cost. */
    cost = calculate_cost(current_parking, current_log);
    current_log->cost = cost;

    /** Add exit log entry to the parking. */
    add_new_exit(current_parking, exit_date, cost, my_vehicle);

    /** Print the exit details. */
    printf("%s %02d-%02d-%04d %02d:%02d %02d-%02d-%04d %02d:%02d %.2f\n", 
        my_vehicle->license_plate, current_log->entry_date->day, 
        current_log->entry_date->month, 
        current_log->entry_date->year, current_log->entry_date->hour, 
        current_log->entry_date->minutes, 
        current_log->exit_date->day, current_log->exit_date->month, 
        current_log->exit_date->year,
        current_log->exit_date->hour, current_log->exit_date->minutes, cost);
}


/** 
 * Lists all the log entries for a given vehicle from all parkings.
 * @param parking_args Pointer to the args structure containing parking information.
 * @param buffer Input buffer containing command and parameters.
 */
void list_vehicle_logs(args *parking_args, char *buffer) {
    int i, entries_found = 0, num_parkings = 0;
    char *license_plate, *parking_names[MAX_PARKINGS];;
    vehicle *my_vehicle;
    parking *current_parking; 
    vehicle_log *current_log;

    /** Initialize the current log to null. */
    current_log = NULL;

    /** Parse license plate from the buffer. */
    license_plate = strtok(buffer, DELIMITER);

    /** Validate license plate. */
    if (license_plate == NULL || !is_license_plate_valid(license_plate)) {
        printf("%s: invalid licence plate.\n", license_plate);
        return;
    }

    /** Add all the parking names to an array to later sort. */
    for (i = 0; i < parking_args->park_counter; i++) {
        if (parking_args->parkings[i] != NULL) {
            parking_names[num_parkings++] = parking_args->parkings[i]->name;
        }
    }

    /** Sort parking names by alphabetical order. */
    insertion_sort(parking_names, num_parkings);
    

    /** Find vehicle by license plate. */
    my_vehicle = find_vehicle(parking_args->vehicles, license_plate);

    /** Check if vehicle exists. */
    if (my_vehicle == NULL) {
        printf("%s: no entries found in any parking.\n", license_plate);
        return;
    }

    /** Iterate through sorted parking array. */
    for (i = 0; i < num_parkings; i++) {
        current_parking = find_parking(parking_args, parking_names[i]);
        current_log = my_vehicle->my_logs;

        /** Iterate through the vehicle log entries. */
        while (current_log != NULL) {
            if (strcmp(current_log->parking_name, current_parking->name) == 0) {
                entries_found = 1; 
                printf("%s %02d-%02d-%04d %02d:%02d", current_log->parking_name, 
                    current_log->entry_date->day, current_log->entry_date->month,
                    current_log->entry_date->year, current_log->entry_date->hour,
                    current_log->entry_date->minutes);
                /** If there is an exit date print it, if not don´t. */       
                if (current_log->exit_date != NULL) {
                    printf(" %02d-%02d-%04d %02d:%02d\n", current_log->exit_date->day,
                        current_log->exit_date->month, current_log->exit_date->year,
                        current_log->exit_date->hour, current_log->exit_date->minutes);
                } else {
                    printf("\n");
                }
            }
            /** Update current_log. */
            current_log = current_log->next;
        }
    }

    /** Check if any log entries were found. */
    if (!entries_found) {
        printf("%s: no entries found in any parking.\n", license_plate);
        return;
    }
}


/** 
 * Displays the daily revenue summary for a specific parking.
 * @param parking_args Pointer to the args structure containing
 *  parking information.
 * @param parking_name Name of the parking for which the daily 
 * revenue summary is to be displayed.
 */
void show_daily_summary( parking *my_parking) {

    billing_day *current_day;

    /** Update current day with the first billing log of the park. */
    current_day = my_parking->billing_day_log;
    
    /** Iterate through each billing day and display the total revenue. */
    while(current_day != NULL){
        printf("%02d-%02d-%04d %.2f\n", current_day->exit_date->day, 
            current_day->exit_date->month,
            current_day->exit_date->year, current_day->total_cost);
            current_day = current_day->next;
    }
}

/** 
 * Displays the revenue details for a specific parking on a given date.
 * @param parking_args Pointer to the args structure containing parking information.
 * @param parking_name Name of the parking for which the revenue details are to be displayed.
 * @param revenue_date Pointer to the date structure representing the revenue date.
 */
void show_day_revenue_list(args *parking_args, char *parking_name, date *revenue_date) {

    parking *current_parking;
    billing_day *current_day = NULL;
    billing_day *last_valid_date = NULL; 
    vehicle_log *current_vehicle_log;

    /** Check if revenue_date is after last action. */
    if (compare_dates(&parking_args->last_date, revenue_date) > 0){
        printf("invalid date.\n");
        return;
    }
    
    /** Find the parking with the input name. */
    current_parking = find_parking(parking_args, parking_name);

    /** Update current day with the first billing log of the park. */
    current_day = current_parking->billing_day_log;

    /** Find the billing day corresponding to the revenue date. */
    while (current_day != NULL) {
        /** Check if the dates are equal and update the valid date if they are. */
        if (compare_dates(current_day->exit_date, revenue_date) == 0) {
            last_valid_date = current_day;
            break;
        }

        current_day = current_day->next;
    }

    /** Check if a valid billing day was found. */
    if (last_valid_date == NULL) {
        return;
    }
    
    current_vehicle_log = last_valid_date->vehicle_log; 

    /** Display revenue details for each vehicle log entry on the revenue date. */
    while (current_vehicle_log != NULL) {
        printf("%s %02d:%02d %.2f\n", current_vehicle_log->license_plate, 
            current_vehicle_log->exit_date->hour,
            current_vehicle_log->exit_date->minutes, current_vehicle_log->cost);
            current_vehicle_log = current_vehicle_log->next;
    }

}

/** 
 * Displays the revenue summary for a parking either daily or on a specific date.
 * @param parking_args Pointer to the args structure containing parking information.
 * @param buffer Input buffer containing command and parameters.
 */
void show_revenue(args *parking_args, char *buffer) {
    char *parking_name, *date_input;
    date *revenue_date;
    parking *my_parking;

    /** Initialize a revenue date. */
    revenue_date = NULL;

    /** Parse parking name from the buffer. */
    parking_name = parse_name(buffer);

    my_parking = find_parking(parking_args, parking_name);

    if(my_parking == NULL){
        printf("%s: no such parking.\n", parking_name);
        return;
    }

    /** Parse date input from the buffer. */
    date_input = strtok(NULL, DELIMITER);

    /** If date input is null, display daily summaries of the park. */
    if (date_input == NULL) {
        show_daily_summary(my_parking);
        return;
    }

    /** Parse revenue date. */
    revenue_date = parse_date(date_input, NULL);

    /** Display revenue details for the parking on the specified date. */
    show_day_revenue_list(parking_args, parking_name, revenue_date);
    free(revenue_date);

}

/** 
 * Removes a parking from the parking list and all its associated vehicle logs.
 * @param parking_args Pointer to the args structure containing parking information.
 * @param buffer Input buffer containing command and parameters.
 */
void remove_parking(args *parking_args, char *buffer) {
    char *parking_name,*parking_names[MAX_PARKINGS];
    int i, num_parkings = 0;
    parking *my_parking;

    /** Parse parking name from the buffer. */
    parking_name = parse_name(buffer);

    my_parking = find_parking(parking_args,parking_name);

    if (my_parking == NULL) {
        printf("%s: no such parking.\n", parking_name);
        return;
    }
    
    /** Remove the parking from the parking list and its associated vehicle logs. */
    remove_parking_from_logs(parking_args->vehicles, parking_name);
    remove_parking_from_list(parking_args, parking_name);
    
    
    /** Collect remaining parking names after removal for sorting. */
    for (i = 0; i < parking_args->park_counter; i++) {
        if (parking_args->parkings[i] != NULL) {
            parking_names[num_parkings++] = parking_args->parkings[i]->name;
        }
    }

    /** Sort and display remaining parking names array for output. */
    insertion_sort(parking_names, num_parkings);
    for (i = 0; i < num_parkings; i++) {
        printf("%s\n", parking_names[i]);
    }
}
