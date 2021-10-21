#include <stdio.h>
#include <stdlib.h>

int main()
{
   int num;
   FILE *fptr;

   if ((fptr = fopen("../example/example.txt","r")) == NULL){
       printf("Error! opening file");

       // Program exits if the file pointer returns NULL.
       exit(1);
   }

    char line[100]; 
    while ( fgets( line, 100, fptr ) != NULL ) 
    { 
        fprintf("The line is: %s\n", line); 
    } 
          
   fclose(fptr); 
  
   return 0;
}