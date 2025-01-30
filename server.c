#include "catalog.h"

// Dimiourgia katalogou proionton
void createCatalog(catalog k[]) {
   for (int i = 0; i < PRODUCTS; i++) {
       // Dimiourgia perigrafis gia kathe proion
        sprintf(k[i].description, "Product %d", i + 1);  // Tyxaia timi metaksy 1 kai 100 gia to proion
        k[i].price = rand() % 100 + 1;
       k[i].item_count = 2;  // Arxiko diathesimo apothema gia kathe proion
}
}
// Xeirismos paraggelion apo pelati
void handleCustomer(catalog k[], int client_socket, int* profit, int* successful, int* failed, int fail[][CUSTOMERS], int range[], int customer_index) {
    for (int i = 0; i < ORDERS; i++) {
        int product;
        read(client_socket, &product, sizeof(int));// Diavazei ton kodiko tou proiontos apo ton pelati
        usleep(500000);  // Mikri kathysterisi gia na prosomoiothei o xronos epexergasias
    int found = 0;
       char storage[130];
        for (int j = 0; j < PRODUCTS; j++) {
        if (j + 1 == product) {                
                range[j]++;// Ayksisi tou plithous twn zitiseon gia to sygkekrimeno proion               
               if (k[j].item_count > 0) { // Elegxos an to proion einai diathesimo                  
                 k[j].item_count--;// Meiwnetai to apothema tou proiontos                   
            (*successful)++;// Ayksisi twn epitiximenwn paraggelion                  
                *profit += k[j].price;// Prosthiki tis timis tou proiontos sta kerdh
                  sprintf(storage, "Successful order %d for %s\n", product, k[j].description);// Apostoli minimatos epityxias ston pelati
           write(client_socket, storage, strlen(storage) + 1);
                } else {
                    // An den yparxei diathesimo proion, ayksisi ton apotiximenon paraggelion
                    (*failed)++;
                    fail[j][customer_index]++;
                    sprintf(storage, "Failed order %d for %s\n", product, k[j].description);// Apostoli minimatos apotixias ston pelati
                    write(client_socket, storage, strlen(storage) + 1);
                }
                found = 1;
                break;
            }
        }
        // An o pelatis zitise lathos kodiko proiontos
        if (!found) {
            sprintf(storage, "Invalid product code: %d\n", product);
            write(client_socket, storage, strlen(storage) + 1);
        }
    }
}

int main() {
     srand(time(NULL));// Arxikopoihsh tyxaiou arithmou
      catalog k[PRODUCTS];
  createCatalog(k); 
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);// Dimiourgia tou server
    struct sockaddr_in address;
address.sin_family = AF_INET;
address.sin_addr.s_addr = INADDR_ANY;
   address.sin_port = htons(8080);
  bind(server_fd, (struct sockaddr *)&address, sizeof(address));
 listen(server_fd, 5);
    int successful = 0, failed = 0, profit = 0;
int fail[PRODUCTS][CUSTOMERS];
   memset(fail, 0, sizeof(fail));
 int range[PRODUCTS];
     memset(range, 0, sizeof(range));
    // Dimiourgia pelaton me xrisimopoiisi fork
    for (int j = 0; j < CUSTOMERS; j++) {
        pid_t pid = fork();
        if (pid == 0) { // Paidiki diergasia - pelatis
            sleep(1); // Kathysterisi gia na einai etoimos o server
            int sock = socket(AF_INET, SOCK_STREAM, 0);
            struct sockaddr_in serv_addr;
            serv_addr.sin_family = AF_INET;
            serv_addr.sin_port = htons(8080);
            inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

            // Syndesi me ton server
            if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
                perror("connect failed");
                exit(EXIT_FAILURE);
            }

            // O pelatis kanei paraggelies
            for (int i = 0; i < ORDERS; i++) {
                sleep(1); // 1 deyterolepto metaksy paraggelion
              int product = rand() % PRODUCTS + 1; 
                write(sock, &product, sizeof(int));
             char response[130];
                  read(sock, response, sizeof(response));
               printf("Customer %d received: %s", j + 1, response);
            }
            close(sock);
            exit(0);
        } else { // Goniki diergasia - server
            int client_socket;
         struct sockaddr_in client_addr;
        socklen_t client_addrlen = sizeof(client_addr);
         client_socket = accept(server_fd, (struct sockaddr *)&client_addr, &client_addrlen);
          handleCustomer(k, client_socket, &profit, &successful, &failed, fail, range, j);
    close(client_socket);
           wait(NULL); 
        }
    }
    printf("\n");// Ektypwsi apotelesmatwn
    for (int i = 0; i < PRODUCTS; i++) {
     printf("%s was requested: %d times\n", k[i].description, range[i]);
       printf("and was sold: %d times\n", 2 - k[i].item_count);
      for (int j = 0; j < CUSTOMERS; j++) {
         if (fail[i][j] > 0) {
          printf("Customer %d failed %d times for %s\n", j + 1, fail[i][j], k[i].description);
         }
      }
      printf("----------------------------------------------\n");
    }
    printf("Total successful orders: %d\n", successful);
    printf("Total failed orders: %d\n", failed);
    printf("Total profit: %.2f\n", (float)profit);
    printf("----------------------------------------------\n");

    close(server_fd);
    return 0;
}

