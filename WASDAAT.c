/*
 * WASDAAT: Will and Andy's Super Duper Awesome Audio Thing
 * See github.com/nmgtn/WASDAAT for more details
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "portsf.h"
#include <unistd.h> // For access()


/*===== main =================================================================*/
int main(int argc, char *argv[]) {

    /*----- Get required arguments --------------------------------*/


    // Check number of arguments
    if(argc == 1) {
        printf("\nCracking. Here we go...\n");
    } else {
        printf("\nWASDAAT does not yet support arguments when running the"
        "program. We\'re working on it.\n");
        return EXIT_FAILURE;
    }

    // Select operation
    char enteredOperation[] = "00"; // todo Should probably change this to something like the audio prog cw1 thing, where we used getchar() a lot to catch inputs that were too long.
    memset(enteredOperation, 0, 3);
    char *leftoverChar = '\0';
    int operationToDo = 0;

    printf("Please select an operation:\n"
    "\t1: Split multichannel file to mono files\n"
    "\t2: Convert file type\n");

    scanf("%s", enteredOperation);
    operationToDo = strtol(enteredOperation, &leftoverChar, 10);

    if(*leftoverChar != '\0') {
        printf("That wasn\'t a valid input\n");
        return EXIT_FAILURE;
    } else {
        printf("You selected operation %d.\n", operationToDo);
    }


    // Ask user for an input file
    char inputFilename[101]; //todo optimise this!
    memset(inputFilename, 0, 101);

    printf("Please specify the input file:\n");
    scanf("%s", inputFilename);


    // Check that file exists
    if(access(inputFilename, F_OK) != 0){
        printf("Error: could not find the input file.\n"
        		"Please check the input filename and re-run the program.\n");
        return EXIT_FAILURE;
    }

    // Check file format
    psf_format inputFileType = psf_getFormatExt(inputFilename);
    if(inputFileType == PSF_FMT_UNKNOWN) {
        printf("Error: failed to determine the input file type.\n"
        		"Supported formats are .wav, .aif, .aiff, .aifc, .afc, .amb\n");
        return EXIT_FAILURE;
    } else {
        printf("File valid.\n");
    }



    // Finish up
    return 0;

}

int cleanUp(){

	return 0;
}
