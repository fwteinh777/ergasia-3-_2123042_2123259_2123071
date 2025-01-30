#ifndef CATALOG_H 
#define CATALOG_H
 #include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h> 
#include <sys/wait.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <time.h>
 #define PRODUCTS 20 
#define ORDERS 10 
#define CUSTOMERS 5
 typedef struct {
 char description[50];
 float price;
 int item_count; }
catalog;
 void createCatalog(catalog k[]); 
void handleCustomer(catalog k[], int client_socket, int* profit, int* successful, int* failed, int fail[][CUSTOMERS], int range[], int customer_index); 
#endif // CATALOG_H

