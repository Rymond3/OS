#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define NR_STOPS    5   // Number of stops on the route
#define ON_ROUTE    0   // Bus on route (on the move)
#define AT_STOP     1   // Bus at a certain stop
#define MAX_USERS   40  // Bus max capacity
#define USERS       4   // Number of users

// Initial state
int state        = ON_ROUTE;
int current_stop = 0;     // Stop where the bus is located
int nr_occupants = 0;     // Number of occupants on the bus

//int MAX_USERS = 0;
//int USERS = 0;
//int NR_STOPS = 0;

// Number of users waiting to get on the bus at each stop
int waiting_stop[NR_STOPS]; //= {0,0,...0};

// Number of passengers waiting to get off the bus at each stop
int waiting_inside[NR_STOPS];  //= {0,0,...0};

// Global definitions for communication / synchronization mechanisms ...
pthread_mutex_t mutex;
pthread_cond_t c_up[NR_STOPS];
pthread_cond_t c_down[NR_STOPS];
pthread_cond_t bus;

/*
Update system state and block the bus until no users want to get on or off the bus
at the current stop. Then the bus resumes its route
*/
void bus_at_stop(void){
	pthread_mutex_lock(&mutex);
	while(waiting_stop[current_stop] > 0 || waiting_inside[current_stop] > 0) {
		if(waiting_stop[current_stop] > 0) {
			pthread_cond_broadcast(&bus);
			pthread_cond_wait(&c_up[current_stop], &mutex);
		}
		if(waiting_inside[current_stop] > 0) {
			pthread_cond_broadcast(&bus);
			pthread_cond_wait(&c_down[current_stop], &mutex);
		}
	}
	pthread_mutex_unlock(&mutex);
}

/*
The value of current_stop will be updated after a random delay
*/
void drive_to_next_stop(){
	pthread_mutex_lock(&mutex);
	state = ON_ROUTE;
	pthread_mutex_unlock(&mutex);
	sleep(3);
	pthread_mutex_lock(&mutex);
	current_stop = (current_stop + 1) % NR_STOPS;
	state = AT_STOP;
	pthread_mutex_unlock(&mutex);
}

/*
The user will indicate that he/she wants to get on the bus at the ’origin’ stop,
and then will wait for the bus to stop there to get on the bus.
*/
void get_on_bus(int user_id, int origin){
	pthread_mutex_lock(&mutex);
	waiting_stop[origin]++;
	while(current_stop != origin || state == ON_ROUTE) {
		pthread_cond_wait(&bus, &mutex);
	}
	nr_occupants++;
	waiting_stop[origin]--;
	if(waiting_stop[origin] == 0){
		pthread_cond_signal(&c_up[current_stop]);
	}
	pthread_mutex_unlock(&mutex);
}

/*
The user will indicate that he/she wants to get off the bus at the ’destination’
stop, and will wait for the bus to stop there to get off.
*/
void get_off_bus(int user_id, int destination){
	pthread_mutex_lock(&mutex);
	waiting_stop[destination]++;
	while(current_stop != destination || state == ON_ROUTE) {
		pthread_cond_wait(&bus, &mutex);
	}
	nr_occupants--;
	waiting_stop[destination]--;
	if(waiting_stop[destination] == 0){
		pthread_cond_signal(&c_down[current_stop]);
	}
	pthread_mutex_unlock(&mutex);
}

void* bus_thread(void* args) {
	while (1) {
		// Wait for the passengers to
		// to get on and off the bus
		bus_at_stop();
		// Drive to the following bus stop
		// (insert "sleep(1)" to mimic the delay)
		drive_to_next_stop();
	}
}

void __user(int user_id, int origin, int destination) {
	// Wait for the bus to be
	// at the origin stop to get on the bus
	if(current_stop == origin)
		get_on_bus(user_id, origin);

	// Get off at destination stop
	else if(current_stop == destination)
		get_off_bus(user_id, destination);
}

void* user_thread(void* arg) {
	int user_id;
	int a = 0;
	int b = 0;
	// Get user_id from arg ...
	user_id = pthread_self();
	while (1) {
		a=rand()%NR_STOPS;
		do{
			b=rand()%NR_STOPS;
		} while(a==b);
		__user(user_id,a,b);
	}
}

int main(int argc, char* argv[]) {
	int i;
	// Definition of local variables
	pthread_t usu[USERS];
	pthread_t autobus;

	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&bus, NULL);
	pthread_cond_init(&c_up[current_stop], NULL);
	pthread_cond_init(&c_down[current_stop], NULL);
	// (Optional part)
	// Parse arguments:
	// Usage: ./simulator <bus_capacity> <user_count> <number_of_stops>
	//MAX_USERS = (int*)argv[1];
	//USERS = (int*)argv[2];
	//NR_STOPS = (int*)argv[3];

	// Create the bus thread
	pthread_create(&autobus, NULL, bus_thread, NULL);
	for (i = 0; i < USERS; ++i){
		// Create thread for user i
		pthread_create(&usu[i], NULL, user_thread, &i);
		pthread_join(usu[i], NULL);
	}
	// Wait for each thread to complete
	while(1);
	return 0;
}
