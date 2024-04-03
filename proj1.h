/** File with all the main functions of each command
 * @file proj1.h
 * @author ist1106369 Sophia Alencar
*/

#ifndef PROJ1_H
#define PROJ1_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** Constants **/

/** Number os max parkings that can exist at the same time. */
#define MAX_PARKINGS 20

/** Delimeter for parsing purposes. */
#define DELIMITER " \t\n"

/** Initial length of the hastable. */
#define INITIAL_LENGTH 2024

/** Number of days in months. */
#define MONTH_31 31
#define MONTH_30 30
#define MONTH_28 28

/** Months of the year. */
#define JANUARY 1
#define FEBRUARY 2
#define MARCH 3
#define APRIL 4
#define MAY 5
#define JUNE 6
#define JULY 7
#define AUGUST 8
#define SEPTEMBER 9
#define OCTOBER 10
#define NOVEMBER 11
#define DECEMBER 12

/** Structs. */


typedef struct date {

    int day;
    int month;
    int year;
    int hour;
    int minutes;

} date;


typedef struct vehicle_log {
    char *license_plate;        /**< License plate of the vehicle. */
    char *parking_name;         /**< Name of the parking. */
    date *entry_date;           /**< Entry date of the vehicle. */
    date *exit_date;            /**< Exit date of the vehicle. */
    double cost;                /**< Cost of parking. */
    struct vehicle_log *next;   /**< Pointer to the next vehicle log. */
} vehicle_log;

typedef struct {
    char *license_plate;        /**< License plate of the vehicle. */
    char *parking_name;         /**< Name of the parking. */
    int is_parked;              /**< Flag indicating whether the vehicle is parked. */
    vehicle_log *my_logs;       /**< Pointer to the first log of the vehicle. */
    vehicle_log *my_last_log;   /**< Pointer to the last log of the vehicle. */
} vehicle;

typedef struct billing_day {
    date *exit_date;                /**< Date of the billing day. */
    double total_cost;              /**< Total cost incurred on this billing day. */
    vehicle_log *last_vehicle_log; /**< Pointer to the last vehicle log of the billing day. */
    vehicle_log *vehicle_log;       /**< Pointer to the first vehicle log of the billing day. */
    struct billing_day *next;       /**< Pointer to the next billing day. */
} billing_day;

typedef struct {
    char *name;                         /**< Name of the parking. */
    int max_capacity;                   /**< Maximum capacity of the parking. */
    double cost_x, cost_y, cost_z;      /**< Costs for different vehicle sizes. */
    int free_spaces;                    /**< Number of free parking spaces. */
    billing_day *billing_day_log;       /**< Pointer to the first billing day of the parking. */
    billing_day *last_billing_day_log; /**< Pointer to the last billing day of the parking. */
} parking;

typedef struct entry {
    char *key;           /**< Key for the hashtable entry. */
    vehicle *value;      /**< Value associated with the key. */
    struct entry *next;  /**< Pointer to the next entry in the hashtable. */
} entry;

typedef struct {
   entry *entries;    /**< Array of hashtable entries. */
   int length;        /**< Length of the hashtable. */
   int num_entries;   /**< Number of entries in the hashtable. */
} hashtable;

typedef struct {
    parking *parkings[MAX_PARKINGS]; /**< Array of parkings. */
    hashtable *vehicles;            /**< Hashtable of vehicles. */
    date last_date;                 /**< Last date recorded. */
    int park_counter;               /**< Counter for the number of parkings. */
    int num_logs;                   /**< Number of vehicle logs. */
} args;


/* Functions to deal with command p*/
int add_new_parking(args *parking_args,char *name, int max_capacity, double cost_x,double cost_y, double cost_z);
void list_all_parkings(args *parking_args);
void list_or_add_park(args *parking_args,char *buffer);


/* Function to deal with command e*/
void register_entry(args *parking_args,char *buffer);


/* Function to deal with command s*/
void register_exit(args *parking_args,char *buffer);


/* Function to deal with command v*/
void list_vehicle_logs(args *parking_args,char *buffer);


/* Functions to deal with command f*/
void show_daily_summary( parking *my_parking);
void show_revenue(args *parking_args, char *buffer);
void show_day_revenue_list(args *parking_args, char *parking_name, date *revenue_date);


/* Functions to deal with command r*/
void remove_parking(args *parking_args, char *buffer);


#endif // PROJ1_H
