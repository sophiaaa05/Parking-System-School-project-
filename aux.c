/**
 * File with auxiliar functions
 * @file aux.c
 * @author ist1106369 Sophia Alencar
*/

#include "aux.h"


/** 
 * Initializes the parking array in the parking_args structure.
 * @param parking_args Pointer to the args structure containing 
 * parking information.
 */
void initialize_parking_list(args *parking_args) {
    int i;

    /** Set counters to 0. */
    parking_args->park_counter = 0;
    parking_args->num_logs = 0;

    /** Initialize parkings array with NULL values. */
    for (i = 0; i < MAX_PARKINGS; i++) {
        parking_args->parkings[i] = NULL;
    }

}

/** 
 * Initializes the hashtable for storing vehicle information.
 * @param parking_args Pointer to the args structure containing 
 * parking information.
 */
void initialize_hashtable(args *parking_args) {
    /** Allocate memory for the hashtable. */
    if ((parking_args->vehicles = (hashtable*)malloc(sizeof(hashtable))) == NULL) {
        fprintf(stderr,"Memory allocation of hashtable vehicles failed\n");
        return;
    }

    /** Set last_date to -1 for further error checking. */
    date invalid = { -1, -1, -1, -1, -1 };
    parking_args->last_date = invalid;

    /** Set hashtable length and num_entries. */
    parking_args->vehicles->length = INITIAL_LENGTH;
    parking_args->vehicles->num_entries = 0;

    /** Allocate memory for hashtable entries. */
    if ((parking_args->vehicles->entries = (entry*)calloc(sizeof(entry), 
        parking_args->vehicles->length)) == NULL) {
        printf("Memory allocation of entries failed\n");
        free_hashtable(parking_args->vehicles);
        return;
    }
}

/**
 * Extracts the name from the given buffer.
 * @param buffer The input string from which to extract the name.
 * @return The extracted name, or NULL if no name is found.
 */
char *parse_name(char *buffer) {
    char *name; 

    /** Check if the buffer contains a double quote character. */
    if (strchr(buffer, '"')) {
        /** Discard the first part of the string until the first double quote. */
        strtok(buffer, "\""); 
        /** Extract the token enclosed within double quotes. */
        name = strtok(NULL, "\""); 
    } else {
        /** If there are no double quotes in the buffer, tokenize using the regular delimiter. */
        name = strtok(buffer, DELIMITER); 
    }

    return name; 
}

/** 
 * Calculates the hash value for the given key.
 * @param key The key for which to calculate the hash value.
 * @return The calculated hash value.
 */
size_t hash_function(char *key) {

    /** Initial hash value with common prime number used for hashing. */
    size_t hash = 5381; 
    int c;

    /** Iterate over each character in the key. */
    while ((c = *key++) != '\0') {
        /** Update the hash value using an algorithm. */
        hash = ((hash << 5) + hash) + c; 
    }

    return hash; 
}

/** 
 * Inserts a new entry(vehicle) into the hashtable.
 * @param ht Pointer to the hashtable.
 * @param key The key of the entry to be inserted.
 * @param value The value associated with the key.
 */
void insert_new_entry(hashtable *ht, char *key, vehicle *value) {
    entry *current_entry;

    /** Calculate hash index. */
    size_t hash = hash_function(key) % ht->length; 

    /** Get the entry at the hash index. */
    current_entry = &ht->entries[hash]; 

    /** If the entry is not empty, find the last entry in the list. */
    /** Collision handling with lists. */
    if (current_entry->key != NULL) {
        while (current_entry->next != NULL) {
            current_entry = current_entry->next;
        }
        /** Allocate memory for a new entry and move to it. */
        if((current_entry->next = (entry *)malloc(sizeof(entry))) == NULL){
            printf("Memory allocation for new entry failed\n");
        }
        current_entry = current_entry->next;
    }

    /** Initialize the new entry. */
    current_entry->next = NULL;
    if((current_entry->key = (char *)malloc(strlen(key) + 1)) == NULL){
        printf("Memory allocation for new key failed\n");
        return;
    }
    strcpy(current_entry->key, key);
    current_entry->value = value;

    /** Increment the number of entries. */
    ht->num_entries++; 

}


/** 
 * Finds a vehicle in the hashtable by license plate.
 * @param vehicles Pointer to the hashtable containing vehicle information.
 * @param license_plate The license plate of the vehicle to find.
 * @return Pointer to the found vehicle, or NULL if not found.
 */
vehicle *find_vehicle(hashtable *vehicles, char *license_plate) {
    size_t hash_index; 
    entry *current_entry;

    /** Calculate hash index. */
    hash_index =  hash_function(license_plate) % vehicles->length;

    /** Get the entry at the hash index. */
    current_entry = &vehicles->entries[hash_index]; 

    if (current_entry->key == NULL) {
        return NULL; 
    }

    /** Iterate through the entries in the list at the hash index. */
    while (current_entry != NULL) {
        if (strcmp(current_entry->key, license_plate) == 0) {
            /** Return the value if the key matches. */
            return current_entry->value; 
        }
        current_entry = current_entry->next;
    }

    return NULL; 
}


/** 
 * Checks if a license plate is valid.
 * @param license_plate The license plate to be validated.
 * @return 1 if the license plate is valid, otherwise 0.
 */
int is_license_plate_valid(char *license_plate) {
    int letter_count = 0, digit_count = 0;

    /** Check if the license plate has the correct format. */
    if (strlen(license_plate) != 8 || license_plate[2] != '-' 
        || license_plate[5] != '-')
        return 0;

    /** Check each segment of the license plate. */
    for (int i = 0; i < 8; i += 3) {
        /** Check if each segment contains two letters or two digits. */
        if ((isupper(license_plate[i]) && isupper(license_plate[i + 1])) ||
            (isdigit(license_plate[i]) && isdigit(license_plate[i + 1]))) {

            /** Count the number of letters and digits. */
            if (isupper(license_plate[i])) {
                letter_count++;
            } else {
                digit_count++;
            }
        } else {
            return 0; 
        }
    }

    /** Check if there is at least one letter and one digit. */
    if (letter_count < 1 || digit_count < 1)
        return 0;

    return 1; 
}

/**
 * Parses date and time from input strings and puts them in a date structure.
 * @param date_input The string containing date information.
 * @param time_input The string containing time information.
 * @return A pointer to the parsed date structure.
 */
date *parse_date(char *date_input, char *time_input){

    date *entry_date; 

    if((entry_date = (date *)malloc(sizeof(date)) ) == NULL){
        printf("memory allocation for new date failed\n");
        return NULL;
    }

    if(time_input == NULL){
        sscanf(date_input, "%d-%d-%d", &(entry_date->day), 
            &(entry_date->month), &(entry_date->year));
        entry_date->hour = 0;
        entry_date->minutes = 0;
        return entry_date;
    }

    sscanf(date_input, "%d-%d-%d", &(entry_date->day), 
        &(entry_date->month), &(entry_date->year));
    sscanf(time_input, "%d:%d", &(entry_date->hour), 
        &(entry_date->minutes));

    return entry_date;
}

/**
 * Finds a parking structure by its name.
 * @param parking_args Pointer to the arguments structure 
 * containing parking information.
 * @param name The name of the parking to find.
 * @return A pointer to the found parking structure, or NULL if not found.
 */
parking *find_parking(args *parking_args,char *name){

    int i;
    if(name == NULL){
        return NULL;
    }

    for (i =0; i <= parking_args->park_counter;i++){
        if (parking_args->parkings[i] != NULL && 
        strcmp(parking_args->parkings[i]->name, name) == 0) {
            return parking_args->parkings[i];
        }
    }
    return NULL;
}

/**
 * Checks if a vehicle is parked.
 * @param my_vehicle Pointer to the vehicle structure to be checked.
 * @return 1 if the vehicle is parked, otherwise 0.
 */
int is_vehicle_parked(vehicle *my_vehicle){
    if(my_vehicle == NULL){
        return 0;
    }
    return my_vehicle->is_parked;
}

/**
 * Gets the number of days in a given month.
 * @param month The month for which to get the number of days.
 * @return The number of days in the given month.
 */
int get_days_in_month(int month) {
    switch(month) {
        case JANUARY: case MARCH: case MAY: case JULY: 
        case AUGUST: case OCTOBER: case DECEMBER:
            return MONTH_31;
        case APRIL: case JUNE: case SEPTEMBER: case NOVEMBER:
            return MONTH_30;
        case FEBRUARY:
            return MONTH_28;
        default:
            return 0; 
    }
}

/**
 * Checks if a given date is later then the last date.
 * @param entry_date Pointer to the date structure to be checked.
 * @param parking_args Pointer to the arguments structure containing parking information.
 * @return 1 if the date is valid, otherwise 0.
 */
int is_date_valid(date *entry_date, args *parking_args) {
    int days_in_month;
    days_in_month = get_days_in_month(entry_date->month);

    if(days_in_month == 0){
        return 0;
    }

    if (entry_date->day < 1 || entry_date->day > days_in_month){
        return 0;
    }

    if(entry_date->hour < 0 || entry_date->hour > 23  || entry_date->minutes < 0 
    || entry_date->minutes > 59){
        return 0;
    }

    if (parking_args->last_date.minutes == -1) {
        parking_args->last_date = *entry_date;
        return 1;
    }
    
    if (parking_args->last_date.year > entry_date->year)
        return 0;

    if (parking_args->last_date.year == entry_date->year &&
        parking_args->last_date.month > entry_date->month)
        return 0;

    if (parking_args->last_date.year == entry_date->year &&
        parking_args->last_date.month == entry_date->month &&
        parking_args->last_date.day > entry_date->day)
        return 0;

    if (parking_args->last_date.year == entry_date->year &&
        parking_args->last_date.month == entry_date->month &&
        parking_args->last_date.day == entry_date->day &&
        parking_args->last_date.hour > entry_date->hour)
        return 0;

    if (parking_args->last_date.year == entry_date->year &&
        parking_args->last_date.month == entry_date->month &&
        parking_args->last_date.day == entry_date->day &&
        parking_args->last_date.hour == entry_date->hour &&
        parking_args->last_date.minutes > entry_date->minutes)
        return 0;
    
    parking_args->last_date = *entry_date;
    return 1;
}

/**
 * Calculates the total number of minutes between two dates.
 * @param entry_date The entry date.
 * @param exit_date The exit date.
 * @return The total number of minutes between the entry and exit dates.
 */
int get_all_minutes(date *entry_date, date *exit_date){

    int entry_minutes, exit_minutes, total_minutes, i;

    /** Calculate total minutes for entry date. */
    entry_minutes = entry_date->hour * 60 + entry_date->minutes + 
    entry_date->day * 24 * 60 + entry_date->year * 365 * 24 * 60;
    for (i = 1; i < entry_date->month; i++) {
        entry_minutes += get_days_in_month(i) * 24 * 60;
    }

    /** Calculate total minutes for exit date. */
    exit_minutes = exit_date->hour * 60 + exit_date->minutes + 
    exit_date->day * 24 * 60 + exit_date->year * 365 * 24 * 60;
    for (i = 1; i < exit_date->month; i++) {
        exit_minutes += get_days_in_month(i) * 24 * 60;
    }

    /** Calculate the difference between entry and exit dates. */
    total_minutes = exit_minutes - entry_minutes;

    return total_minutes;
}

/**
 * Calculates the parking cost based on the duration of parking and said costs.
 * @param current_parking The current parking.
 * @param current_log The vehicle log containing entry and exit dates.
 * @return The total cost od staying in the park.
 */
double calculate_cost(parking *current_parking, vehicle_log *current_log){

    double X = current_parking->cost_x;
    double Y = current_parking->cost_y;
    double Z = current_parking->cost_z;
    double remaining_cost = 0.0;
    double total_cost = 0.0;
    int minutes, days, remaining_minutes, remaining_periods;

    /** Calculate total minutes parked. */
    minutes = get_all_minutes(current_log->entry_date, current_log->exit_date);

    /** Calculate total days parked. */
    days = minutes / (24 * 60);

    /** Calculate the remaining minutes after the whole days*/
    remaining_minutes = minutes % (24 * 60);

    /** Add the cost of the full days. */
    total_cost += days * Z;

    /** Calculate remaining quarter periods. */
    remaining_periods = (remaining_minutes + 14) / 15;

    /** If there is more then 4 periods (first hour), multiply 
     * the first four by the X cost 
     * and the rest by the Y value(after the first hour).
    */
    if (remaining_periods > 4) {
        remaining_cost = 4 * X + (remaining_periods - 4) * Y;
    } else {
        remaining_cost = remaining_periods * X;
    }

    /** Compare remaining cost with daily cost(Z) so it doesn't exceed the limit. */
    if (remaining_cost < Z) {
        total_cost += remaining_cost;
    } else {
        total_cost += Z;
    }

    return total_cost;
}

/**
 * Sorts an array of strings in lexicographical order using insertion sort.
 * @param array The array of strings to be sorted.
 * @param num_elements The number of elements in the array.
 */
void insertion_sort(char **array, int num_elements) {
    int i, j;
    char *key;
    for (i = 1; i < num_elements; i++) {
        key = array[i];
        j = i - 1;

        /** Move elements of array[0..i-1], that are greater than key, 
         * to one position ahead of their current position. 
        */
        while (j >= 0 && strcmp(array[j], key) > 0) {
            array[j + 1] = array[j];
            j = j - 1;
        }
        array[j + 1] = key;
    }
}

/**
 * Adds a new log entry for a vehicle parked in a parking.
 * @param my_vehicle The vehicle being parked.
 * @param current_parking The parking where the vehicle is parked.
 * @param entry_date The entry date of the vehicle.
 * @param name The name of the parking.
 */
void add_new_log(vehicle *my_vehicle, parking *current_parking, date *entry_date, char *name){
    vehicle_log *new_log;

    /** Allocate memory for the new vehicle log, */
    if((new_log = (vehicle_log*)malloc(sizeof(vehicle_log))) == NULL){
        fprintf(stderr, "Memory allocation for new log failed\n");
        return;
    }

    /** Initialize it. */
    new_log->entry_date = entry_date;
    new_log->exit_date = NULL;
    new_log->cost = 0.0;

    /** Allocate memory for license plate and copy the value. */
    if((new_log->license_plate = (char *)malloc(strlen(my_vehicle->license_plate) + 1)) == NULL){
        fprintf(stderr, "Memory allocation for license plate failed\n");
        return;
    }
    strcpy(new_log->license_plate, my_vehicle->license_plate);

    /** Allocate memory for parking name and copy the value. */
    if((new_log->parking_name = (char *)malloc(strlen(name) + 1)) == NULL){
        fprintf(stderr, "Memory allocation for parking name failed\n");
        return;
    }
    strcpy(new_log->parking_name, name);

    if (my_vehicle->my_logs == NULL) {
        /** If there are no logs for the vehicle, set the new log as the first and last log. */
        new_log->next = NULL;
        my_vehicle->my_logs = new_log;
        my_vehicle->my_last_log = new_log;

    } else {
        /** If there are existing logs, set the new log as the last log. */
        new_log->next = NULL;
        my_vehicle->my_last_log->next = new_log;
        my_vehicle->my_last_log = new_log;
    }

    printf("%s %d\n", current_parking->name, current_parking->free_spaces);  
}

/**
 * Compares two dates.
 * @param date_1 The first date.
 * @param date_2 The second date.
 * @return 0 if the dates are equal, 1 if date_1 is greater, -1 if date_2 is greater.
 */
int compare_dates(date *date_1, date *date_2) {

    if (date_1 == NULL) {
        /** date_2 is considered greater. */
        return 1; 
    }

    if (date_2 == NULL) {
        /** date_1 is considered greater. */
        return -1; 
    }

    /** Compare year, month, and day. */
    if (date_1->year == date_2->year && date_1->month == date_2->month 
    && date_1->day == date_2->day ){
        return 0; 
    }
    
    return get_all_minutes(date_1, date_2);
}

/**
 * Adds a new exit revenue record for a vehicle in a parking..
 * @param current_parking The parking where the vehicle was parked.
 * @param exit_date The exit date of the vehicle.
 * @param cost The cost of parking for the vehicle.
 * @param my_vehicle The vehicle that exited.
 */
void add_new_billing_day(parking *current_parking, date *exit_date, double cost, vehicle *my_vehicle){

    billing_day *current_log = current_parking->last_billing_day_log;
    billing_day *new_exit;
    vehicle_log *new_log;

    /** If the exit date is greater than the current latest exit date add 
     * a new billing day . 
    */
    if (compare_dates(current_log->exit_date, exit_date) != 0 ){

        /** Allocate memory for a new billing day entry. */
        if ((new_exit = (billing_day*)malloc(sizeof(billing_day))) == NULL) {
            fprintf(stderr, "Memory allocation for new billing day failed");
            return;
        }
        
        /** Allocate memory for the exit date and copy its value. */
        if ((new_exit->exit_date = (date*)malloc(sizeof(date))) == NULL) {
            fprintf(stderr, "Memory allocation for exit date failed");
            free(new_exit);
            return;
        }
        memcpy(new_exit->exit_date, exit_date, sizeof(date));

        /** Allocate memory for the vehicle log. */
        if (( new_log = (vehicle_log*)malloc(sizeof(vehicle_log)))== NULL) {
            fprintf(stderr, "Memory allocation for new log failed");
            free(new_exit->exit_date);
            free(new_exit);
            return;
        }

        /** Allocate memory for exit_date. */
        if ((new_log->exit_date = (date*)malloc(sizeof(date))) == NULL) {
            fprintf(stderr, "Memory allocation for exit date failed");
            free(new_log);
            free(new_exit->exit_date);
            free(new_exit);
            return;
        }

        memcpy(new_log->exit_date, exit_date, sizeof(date));

        /** Allocate memory for license plate and copy the value. */
        if ((new_log->license_plate = (char *)malloc(strlen(my_vehicle->license_plate) + 1)) == NULL) {
            fprintf(stderr, "Memory allocation for license plate failed");
            free(new_log->exit_date);
            free(new_exit->exit_date);
            free(new_exit);
            free(new_log);
            return;
        }
        strcpy(new_log->license_plate, my_vehicle->license_plate);

        /** Set the cost. */
        new_log->cost = cost;
        new_log->entry_date = NULL;
        new_log->parking_name = NULL;
        new_log->next = NULL;
        new_exit->total_cost = cost;
        new_exit->next = NULL;

        /** Update pointers. */
        current_parking->last_billing_day_log->next = new_exit;
        current_parking->last_billing_day_log = new_exit;
        new_exit->vehicle_log = new_log;
        new_exit->last_vehicle_log = new_log;
    }
    else {
        /** If the exit date is the same as the last recorded billing day we 
         * just add the cost 
         * to the existing billing day and a new vehicle log.
        */

        /** Allocate memory for the vehicle log. */
        if ((new_log = (vehicle_log*)malloc(sizeof(vehicle_log))) == NULL) {
            fprintf(stderr, "Memory allocation for new log failed\n");
            return;
        }
        
        if ((new_log->exit_date = (date*)malloc(sizeof(date))) == NULL) {
            fprintf(stderr, "Memory allocation for exit date failed\n");
            free(new_log);
            return;
        }
        memcpy(new_log->exit_date, exit_date, sizeof(date));

        /** Allocate memory for license plate and copy the value. */
        new_log->license_plate = (char *)malloc(strlen(my_vehicle->license_plate) + 1);
        if (new_log->license_plate == NULL) {
            fprintf(stderr, "%s: Memory allocation for license failed\n",my_vehicle->license_plate );
            free(new_log->exit_date);
            free(new_log);
            return;
        }
        strcpy(new_log->license_plate, my_vehicle->license_plate);

        /** Set the cost and update the total cost for that day. */
        new_log->cost = cost;
        new_log->entry_date = NULL;
        new_log->next = NULL;
        new_log->parking_name = NULL;
        current_log->last_vehicle_log->next = new_log;
        current_log->last_vehicle_log = new_log;
        current_log->total_cost += cost;
    }
}

/**
 * Adds a new exit revenue record for a vehicle in a parking..
 * @param current_parking The parking where the vehicle was parked.
 * @param exit_date The exit date of the vehicle.
 * @param cost The cost of parking for the vehicle.
 * @param my_vehicle The vehicle that exited.
 */
void add_new_exit(parking *current_parking, date *exit_date, double cost, vehicle *my_vehicle){

    billing_day *new_exit; 
    vehicle_log *new_log;

    /** Check if any of the parameters is NULL. */
    if (current_parking == NULL || exit_date == NULL || my_vehicle == NULL) {
        return;
    }

    /** If there are no existing billing day logs for the parking. */
    if (current_parking->billing_day_log == NULL) {

        /** Allocate memory for a new billing day entry. */
        if((new_exit = (billing_day*)malloc(sizeof(billing_day)))==NULL){
            fprintf(stderr,"Memory allocation for billing log failed\n");
            return;
        }

        /** Allocate memory for the exit date and copy its value. */
        if ((new_exit->exit_date = (date*)malloc(sizeof(date))) == NULL) {
            fprintf(stderr,"Memory allocation for exit date failed\n");
            free(new_exit);
            return;
        }

        memcpy(new_exit->exit_date, exit_date, sizeof(date));

        /** Allocate memory for the vehicle log. */
        if ((new_log = (vehicle_log*)malloc(sizeof(vehicle_log)))== NULL) {
            fprintf(stderr,"Memory allocation for new log failed\n");
            free(new_exit->exit_date);
            free(new_exit);
            return;
        }

        /** Allocate memory for exit_date. */
        if ((new_log->exit_date = (date*)malloc(sizeof(date))) == NULL) {
            free(new_exit->exit_date);
            free(new_exit);
            free(new_log);
            return;
        }
        memcpy(new_log->exit_date, exit_date, sizeof(date));

        /** Allocate memory for license plate and copy the value. */
        if ((new_log->license_plate = (char *)malloc(strlen(my_vehicle->license_plate) + 1)) == NULL) {
            free(new_exit->exit_date);
            free(new_exit);
            free(new_log);
            return;
        }
        strcpy(new_log->license_plate, my_vehicle->license_plate);

        /** Set the cost. */
        new_log->cost = cost;
        new_log->entry_date = NULL;
        new_log->parking_name = NULL;
        new_log->next = NULL;
        new_exit->total_cost = cost;
        new_exit->next = NULL;

        /** Update pointers. */
        current_parking->billing_day_log = new_exit;
        current_parking->last_billing_day_log = new_exit;
        new_exit->vehicle_log = new_log;
        new_exit->last_vehicle_log = new_log;

    } else {

        /** Add new billing day. */
        add_new_billing_day(current_parking, exit_date, cost, my_vehicle);
    }
        
}

/**
 * Frees the vehicle log
 * @param head The vehicle log to free.
 */
void free_vehicle_logs(vehicle_log *head) {
    /** Check if the head pointer is NULL, indicating an empty list. */
    if (head == NULL) {
        /** If the list is empty, there's nothing to free, so return. */
        return; 
    }
    /** Iterate through the list until reaching the end. */
    while (head != NULL) {
        vehicle_log *temp = head; 
        head = head->next; 
        /** Free memory of the current node. */
        free(temp->exit_date);
        free(temp->license_plate);
        free(temp->parking_name);
        free(temp->entry_date);
        /** Free the current node. */
        free(temp); 
    }
}

/**
 * Removes vehicle logs associated with a specific parking name from a vehicle's log list.
 * @param current_vehicle The vehicle from which logs are to be removed.
 * @param parking_name The name of the parking to remove logs for.
 */
void remove_vehicle_logs(vehicle *current_vehicle, char *parking_name){

    vehicle_log *current, *first, *prev, *temp;
    current = current_vehicle->my_logs;
    prev = NULL;
    first = NULL;
    temp = NULL;

    /** iterate through the vehicle log list. */
    while (current != NULL) {
        /** Check if the current log is from the specified parking name. 
         * and remove the log.
        */
        if (strcmp(current->parking_name, parking_name) == 0) {
            /** Free the logs of the current vehicle. */
            free(current->parking_name); 
            free(current->entry_date);   
            free(current->exit_date);    
            free(current->license_plate);
            temp = current;  
            current = current->next;     
            free(temp);                   
        }
        else {
            /** If the log is not associated with the parking name.
             * update the pointers;
             */
            if(first == NULL)
                /** Store the first for later update of the vehicle logs list. */
                first = current;         
            
            /** Link the previous log to the next valid log. */
            if (prev != NULL)
                prev->next = current;

            /** Update the previous log. */
            prev = current;

            /** Move to the next log. */            
            current = current->next;     
        }   
    }
    /** If there's a previous log, set its next pointer to NULL. */
    if (prev != NULL)
        prev->next = NULL;

    /** Update the vehicle's log pointers and state. */
    current_vehicle->my_last_log = prev;  
    current_vehicle->my_logs = first;   
    current_vehicle->is_parked = 0;       
}

/**
 * Removes vehicle logs associated with a specific parking name.
 * @param vehicles The hashtable of vehicles.
 * @param parking_name The name of the parking to remove logs for.
 */
void remove_parking_from_logs(hashtable *vehicles, char *parking_name) {
    int i;
    entry *current_entry;
    vehicle *current_vehicle;
    /** Iterate through each slot in the hashtable. */
    for (i = 0; i < vehicles->length; i++) {
        current_entry = &(vehicles->entries[i]); 
        /** iterate the linked list of entries in the current slot */
        while (current_entry != NULL) {
            /** Get the vehicle associated with the current entry. */
            current_vehicle = current_entry->value; 
            if (current_vehicle != NULL) {
                /** Remove all logs from that vehicle associated with the parking. */
                remove_vehicle_logs(current_vehicle, parking_name);
            }
            current_entry = current_entry->next; 
        }
    }
}



/**
 * Frees memory allocated for a parking structure.
 * @param park The parking structure to free.
 */
void free_parking(parking *park) {
    free(park->name); 
    remove_billing_days(park->billing_day_log); 
    free(park);
}

/**
 * Removes billing days associated with a parking.
 * @param head The head of the billing day linked list to remove.
 */
void remove_billing_days(billing_day *head) {
    /** iterate the linked list of billing days. */
    while (head != NULL) {
        billing_day *temp = head; 
        head = head->next; 
        /** free all the vehicle logs of the billing day. */
        free_vehicle_logs(temp->vehicle_log);

        /** Set the pointers to NULL. */
        temp->vehicle_log = NULL; 
        temp->last_vehicle_log = NULL; 
        free(temp->exit_date);
        temp->exit_date = NULL; 

        /** Free the current node. */
        free(temp); 
    }
}


/**
 * Removes a parking from a array of parkings and everything related.
 * @param parking_args The argument containing the list of parkings.
 * @param parking_name The name of the parking to remove.
 */
void remove_parking_from_list(args *parking_args, char *parking_name) {
    int i;
    for (i = 0; i < parking_args->park_counter; i++) {
        /** Iterate through the array lokking for the parking. */
        if (parking_args->parkings[i] != NULL && 
            strcmp(parking_args->parkings[i]->name, parking_name) == 0) {
    
            /** Remove the billing days of the parking. */
            remove_billing_days(parking_args->parkings[i]->billing_day_log);

            /** Set the pointer to NULL and free the parking. */
            parking_args->parkings[i]->billing_day_log = NULL;
            free_parking(parking_args->parkings[i]);

            /** Shift the elements to fill the gap. */
            for (int j = i; j < MAX_PARKINGS - 1; j++) {
                parking_args->parkings[j] = parking_args->parkings[j + 1];
            }

            /** Set the last element to NULL. */
            parking_args->parkings[MAX_PARKINGS - 1] = NULL;
            parking_args->park_counter--;
            break;
        }
    }
}

/**
 * Frees memory allocated for parking arguments.
 * @param parking_args The parking arguments structure to free.
 */
void free_parking_args(args* parking_args) {
    int i;
    if (parking_args == NULL) {
        return; 
    }

    /** Free each parking in the parkings array. */
    for ( i = 0; i < parking_args->park_counter; ++i) {
        free_parking(parking_args->parkings[i]);
    }

    /** Free the vehicles hashtable if it's not NULL. */
    if (parking_args->vehicles != NULL) {
        free_hashtable(parking_args->vehicles);
    }

    /** Finally, free the parking_args structure itself. */
    free(parking_args);
    parking_args = NULL;
}


/**
 * Frees memory allocated for a vehicle structure.
 * @param my_vehicle The vehicle structure to free.
 */
void free_vehicle(vehicle *my_vehicle){

    free(my_vehicle->license_plate);
    free(my_vehicle->parking_name);
    free_vehicle_logs(my_vehicle->my_logs);
    free(my_vehicle);
}


/**
 * Frees memory allocated for a hashtable structure.
 * @param ht The hashtable structure to free.
 */
void free_hashtable(hashtable *ht) {

    int i;
    entry *current_entry, *next_entry;

    if (ht == NULL) {
        return; 
    }

    /** Free each entry in the hashtable. */
    for (i = 0; i < ht->length; ++i) {
        if(ht->entries[i].key != NULL){
            free_vehicle(ht->entries[i].value);
            free(ht->entries[i].key);
            current_entry = ht->entries[i].next; 
            while (current_entry != NULL) {
                /** Store the next entry before freeing the current one. */
                next_entry = current_entry->next; 
                free_vehicle(current_entry->value);
                /** Free the key string. */
                free(current_entry->key); 
                /** Free the entry itself. */
                free(current_entry); 
                /**Move to the next entry. */
                current_entry = next_entry; 
            }
        }
    }

    /** Free the entries array. */
    free(ht->entries);

    /** Finally, free the hashtable structure itself. */
    free(ht);
}

