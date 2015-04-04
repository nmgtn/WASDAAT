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

	// Testing - this variable should be replaced later
    char outputFilename[2][101];
    memset(outputFilename[0], 0, 101);
    memset(outputFilename[1], 0, 101);
    strcpy(outputFilename[0], "testOutL.wav");
    strcpy(outputFilename[1], "testOutR.wav");

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

    // Check that file exists
    if(access(inputFilename, F_OK) != 0){
        printf("Error: could not find the input file.\n"
        		"Please check the input filename and re-run the program.\n");
        return EXIT_FAILURE;
    }

    // Initialise portsf library
	if(psf_init()){
		printf("Error: Unable to start portfsf library\n");
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
	int inputfd = INVALID_PORTSF_FID;
	int outputfd[2] = {INVALID_PORTSF_FID, INVALID_PORTSF_FID};
	PSF_PROPS input_audio_properties;
	PSF_PROPS output_audio_properties;
	float *inBlock = NULL, **outBlock = NULL;
	long num_frames_read = 0;

    // Open the input file
	if((inputfd = psf_sndOpen(inputFilename, &input_audio_properties, DO_NOT_AUTO_RESCALE)) < 0){
		printf("Error: Unable to open input file. Please check %s and retry\n", inputFilename);
		return EXIT_FAILURE;
	}

    const int num_channels = input_audio_properties.chans;	// Store the number of channels in the input file
    long sampleFrequency = input_audio_properties.srate;	// Store sample frequency of input file (and by extension output file)
    // Store the input file traits in the output file traits except the number of channels
    output_audio_properties.chans = 1;
    output_audio_properties.chformat = input_audio_properties.chformat; // todo check this one - I don't understand it fully (not sure how to tell it that we're dealing with mono)
	output_audio_properties.format = input_audio_properties.format;
	output_audio_properties.samptype = input_audio_properties.samptype;
	output_audio_properties.srate = input_audio_properties.srate;



    // Open the relevant number of output files
    // todo put this in a for loop so that loads of output files can be made. At the moment we're limiting outselves to stereo (it'll mess up if a seriously multichannel file is entered)
    if((outputfd[0] = psf_sndCreate(outputFilename[0], &output_audio_properties, CLIP_FLOATS, DO_NOT_MINIMISE_HDR, PSF_CREATE_RDWR)) < 0){
		printf("Error: Unable to open output file 0. Please check %s and retry\n", outputFilename[0]);
		return EXIT_FAILURE;
	}
    if((outputfd[1] = psf_sndCreate(outputFilename[1], &output_audio_properties, CLIP_FLOATS, DO_NOT_MINIMISE_HDR, PSF_CREATE_RDWR)) < 0){
		printf("Error: Unable to open output file 1. Please check %s and retry\n", outputFilename[1]);
		return EXIT_FAILURE;
	}



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

	// Check that sample frequency is greater than 0 (very unlikely to happen, probably impossible but it doesn't hurt to
	// check as if it was 0 there could be divide by 0 errors.
	if(sampleFrequency <= 0){
		printf("Error: The sample frequency of your file is 0. Either the program has gone "
				"wrong or you've done something super sneaky to try and break it. \n"
				"Either way, please try again with a file that definitely has a sample "
				"frequency of more than 0.\n");
		return EXIT_FAILURE;
	}



	/*------There now follows a few loops (while and for) which split the audio file into smaller chunks until we have individual samples------*/
	    printf("\nFiles are being processed. Please wait . . .\n\n");
	    // Read frames from input file a block at a time
	    while((num_frames_read=psf_sndReadFloatFrames(inputfd, inBlock, nFrames)) > 0){
	    	// Now read through each frame within the block individually - note that it is now referred to in samples else it gets confusing to follow
	    	for(int sampleNum=0; sampleNum<(num_frames_read*num_channels); sampleNum+=num_channels){ //
	    		// Finally we split the file down even further to the samples contained by each frame (one sample for each channel)
	    		for(int channelIndex = 0; channelIndex<num_channels;channelIndex++){
					// This line stores the correct channel of the input block in the element of the array that corresponds to the relevant mono output file
					outBlock[channelIndex][sampleNum] = inBlock[sampleNum+channelIndex];
	    		}
			}

	    	// todo change this for loop so it deals with moar channels (specifically the number of channels of the input file we're looking at)
	    	for(int monoOutChannel = 0; monoOutChannel < 2; monoOutChannel++){
				// Write the filtered output block to the output file
				if((psf_sndWriteFloatFrames(outputfd[monoOutChannel], outBlock[monoOutChannel], num_frames_read))!=num_frames_read){
					printf("Error: Couldn't write to output file.\n");
					printf("Please check the input file \"%s\" and delete the incomplete output file \"%s\" then retry\n", inputFilename, outputFilename[monoOutChannel]);
					return EXIT_FAILURE;
				}
	    	}
	    }
	    // Display error if input file couldn't be read
	    if(num_frames_read < 0){
	    	printf("Error: Couldn't read the input file.\n ");
			printf("Please check the input file \"%s\" then retry\n", inputFilename);
			return EXIT_FAILURE;
	    }





	/*----- Finish up --------------------------------*/
	// Free memory for the input block
	if(inBlock){
		free(inBlock);
	}
	// Free memory for the output block
	if(outBlock){
		free(outBlock);
	}
	// Close the output files
	if(outputfd[0] >= 0){
		psf_sndClose(outputfd[0]);
	}
	if(outputfd[1] >= 0){
		psf_sndClose(outputfd[1]);
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
