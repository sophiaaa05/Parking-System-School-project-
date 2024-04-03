/**
 * File with auxiliar functions
 * @file aux.h
 * @author ist1106369 Sophia Alencar
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "proj1.h"

/** Initialize funcitons. ***********************/
void initialize_parking_list(args *parking_args);
void initialize_hashtable(args *parking_args);


/** Parser functions. */
char *parse_name(char *buffer);
date *parse_date(char *date_input, char *time_input);


/** Hastable related functions. */
size_t hash_function(char *key);
void insert_new_entry(hashtable *ht,char *key, vehicle *value);
vehicle *find_vehicle(hashtable *vehicles, char *license_plate);


/** Fucntions to validation and error checking. */
int is_license_plate_valid(char *license_plate);
int is_date_valid( date *entry_date, args *parking_args);
int compare_dates(date *date_1, date *date_2);


/** Funtions to add the nodes to the linked lists. */
void add_new_billing_day(parking *current_parking, date *exit_date, double cost, vehicle *my_vehicle);
void add_new_exit(parking *current_parking, date *exit_date, double cost, vehicle *my_vehicle);
void add_new_log(vehicle *my_vehicle, parking *current_parking, date *entry_date, char *name);


/** Sorting algorithm. */
void insertion_sort(char **array, int num_elements);


/** Auxiliar functions. */
parking *find_parking(args *parking_args,char *name);
int get_days_in_month(int month);
int is_vehicle_parked(vehicle *my_vehicle);
int get_all_minutes(date *entry_date, date * exit_date);
double calculate_cost(parking *current_parking, vehicle_log *current_log);


/** Functions for removal. */
void remove_parking_from_logs(hashtable *vehicles, char *parking_name);
void remove_billing_days(billing_day *head);
void remove_parking_from_list(args *parking_args, char *parking_name);


/** Free functions. */
void free_parking(parking *park);
void free_vehicle_logs(vehicle_log *head);
void free_parking_args(args* parking_args);
void free_vehicle(vehicle *my_vehicle);
void free_hashtable(hashtable *ht);
