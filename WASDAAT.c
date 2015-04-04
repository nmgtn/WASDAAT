/*
 * WASDAAT: Will and Andy's Super Duper Awesome Audio Thing
 * See github.com/nmgtn/WASDAAT for more details
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "portsf.h"
#include <unistd.h> // For access()

/*===== Pre-processor defines =================================================================*/

#define NUM_SAMPLES_IN_FRAME 1024
#define FILE_SAMPLE_FREQ 44100.0
// This ensures that we don't close a file in portsf that wasn't opened in the first place!
#define INVALID_PORTSF_FID -1

/*===== Enums for portsf =================================================================*/

enum float_clipping { DO_NOT_CLIP_FLOATS, CLIP_FLOATS };
enum minheader { DO_NOT_MINIMISE_HDR, MINIMISE_HDR };
enum auto_rescale { DO_NOT_AUTO_RESCALE, AUTO_RESCALE };

/*===== List of functions =================================================================*/
int splitStereoToMono();


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
    char *leftoverChar = NULL;
    int operationToDo = 0;

    printf("Please select an operation:\n"
    "\t1: Split stereo file to two mono files\n" // Change this to "Split multichannel file to mono files" when we've check we can do it with stereo
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

    // Initialise portsf library
	if(psf_init()){
		printf("Error: Unable to start portfsf library\n");
		return EXIT_FAILURE;
	}


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

    // Declare and initialise variables for the audio processing
	DWORD nFrames = NUM_SAMPLES_IN_FRAME;
	int inputfd = INVALID_PORTSF_FID, outputfd = INVALID_PORTSF_FID;
	PSF_PROPS audio_properties;
	float *inBlock = NULL, **outBlock = NULL;
	long num_frames_read = 0;

    // Open the input file
	if((inputfd = psf_sndOpen(inputFilename, &audio_properties, DO_NOT_AUTO_RESCALE)) < 0){
		printf("Error: Unable to open input file. Please check %s and retry\n", inputFilename);
		return EXIT_FAILURE;
	}

    const int num_channels = audio_properties.chans;	// Store the number of channels in the input file

    /*----- Allocate memory for in and out blocks --------------------------------*/
    // Allocate memory for input block. Using calloc so that the memory is initialized to NULL
	if((inBlock = (float*) calloc(nFrames * num_channels, sizeof(float))) == NULL){
		printf("Error: Could not allocate memory for inBlock. Please try running again. If error persists please contact the programmer\n");
		return EXIT_FAILURE;
	}

	// This will allocate memory for a 2d array called outBlock. This means that multiple output files can have their data stored in this array
	/* num_channels is the number of rows  */
	if ((outBlock = (float**) calloc(num_channels, sizeof(float*))) == NULL){
		printf("Error: Could not allocate memory for first dimension of outBlock. Please try running again. If error persists please contact the programmer\n");
		return EXIT_FAILURE;
	}
	for (int i = 0; i < num_channels; i++){
	  if (( outBlock[i] = (float*) calloc(nFrames, sizeof(float))) == NULL ){
		  printf("Error: Could not allocate memory for second dimension of outBlock. Please try running again. If error persists please contact the programmer\n");
		  return EXIT_FAILURE;
	  }
	}

	/* NOTE:
	 * access matrix elements: outBlock[i] give you a pointer
	 * to the row array, outBlock[i][j] indexes an element
	 */





	/*----- Finish up --------------------------------*/
	// Free memory for the input block
	if(inBlock){
		free(inBlock);
	}
	// Free memory for the output block
	if(outBlock){
		free(outBlock);
	}
	// Close the output file
	if(outputfd >= 0){
		psf_sndClose(outputfd);
	}
	// Close the input file
	if(inputfd >= 0){
		psf_sndClose(inputfd);
	}
	//Close the portsf library
	psf_finish();

    return EXIT_SUCCESS;

}

int splitStereoToMono(){


	return EXIT_SUCCESS;
}
