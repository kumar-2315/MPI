#include <stdio.h>
#include <stdlib.h>


// Function to blur the image
void blur_image(unsigned char *input, unsigned char *output, int width, int height) {
    for (int y = 0; y < height; y++) { 
        for (int x = 0; x < width; x++) { 
            int sum = 0;

            
            for (int y1 = y; y1 <= y + 2; y1++) {
                for (int x1 = x; x1 <= x + 2; x1++) {
                    sum += input[y1 * (width + 2) + x1]; 
                }
            }
            
            output[y * width + x] = sum / 9; 
        }
    }
}


//Function to create padding image

unsigned char* create_padding_image(unsigned char *input, int width, int height){
    int padding_width = width + 2;
    int padding_height = height +2;
    unsigned char *padding_image = (unsigned char *) malloc (padding_width*padding_height);

    if(padding_image == NULL){
        printf("Memory couldn't be allocated for padding image\n");
        exit (EXIT_FAILURE);
    }

    //Copy the orginal image to the center of the padded image

    for (int y = 0; y < height; y++){
        for (int x = 0; x <width; x++){
            padding_image[(y+1)*padding_width + (x+1)] = input [y*width +x];
        }

    }

    // (top and bottom)

    for (int y = 0; y < width; y++){
        //top
        for (int x = 0; x <1; x++){
            padding_image[x*padding_width + (y+1)] = input [0+y];
            padding_image[(height+1+x)* padding_width + (x + 1)] = input[(height-1)*width+x];
        }


    }

    // Fill the padding (left and right)
    for (int y = 0; y < height; y++) {
        // Left padding
        for (int px = 0; px < 1; px++) {
            padding_image[(y + 1) * padding_width + px] = input[y * width];
        }
        // Right padding
        for (int px = 0; px < 1; px++) {
            padding_image[(y + 1) * padding_width + (width + 1 + px)] = input[y * width + (width - 1)];
        }


}

// Fill the corners
    for (int py = 0; py < 1; py++) {
        for (int px = 0; px < 1; px++) {
            // Top-left corner
            padding_image[py * padding_width + px] = input[0 * width + 0];
            // Top-right corner
            padding_image[py * padding_width + (width + 1 + px)] = input[0 * width + (width - 1)];
            // Bottom-left corner
            padding_image[(height + 1 + py) * padding_width + px] = input[(height - 1) * width + 0];
            // Bottom-right corner
            padding_image[(height + 1 + py) * padding_width + (width + 1 + px)] = input[(height - 1) * width + (width - 1)];
        }
    }

    return padding_image;

}


int main(){

    char file_name[256]; //orignal image
    int height;  // image heigth
    int width;  // image width
    char output_file_name[256];  // output file name
    

    printf("Please enter the file name!\n");
    scanf("%255s", file_name);

    printf("Please enter the output file name!\n");
    scanf("%255s", output_file_name);

    printf("Please enter the height of the image\n");
    scanf("%d", &height);

    printf("Please enter the width of the image\n");
    scanf("%d", &width);

    //Allocate memory for images

    unsigned char *input_image = (unsigned char *)malloc(width * height);
    unsigned char *output_image = (unsigned char *)malloc(width * height);

    if (input_image == NULL || output_image == NULL){
        printf("Memory couldn't be allocated!\n");
        return EXIT_FAILURE;
    }

    //Read the image 

    FILE *input_file = fopen(file_name, "rb");
    if (input_file == NULL){
        printf("File couldn't be open");
        free(input_image);
        free(output_image);
        return EXIT_FAILURE;
    }

    // storing the image in the allocated memory

    fread(input_image, 1, width*height, input_file);
    fclose(input_file);

    //create padded image
    unsigned char *padding_image = create_padding_image(input_image, width, height);

    //Apply blur to the padded image
    blur_image(padding_image, output_image, width, height);

    FILE *output_file = fopen(output_file_name, "wb");
    if (output_file == NULL) {
        perror("Error opening output file");
        free(input_image);
        free(output_image);
        free(padding_image);
        return EXIT_FAILURE;
    }

    fwrite(output_image, 1, width * height, output_file);
    fclose(output_file);

    free(input_image);
    free(output_image);
    free(padding_image);

    printf("Blurring complete. The output image is saved as %s\n", output_file_name);

    return EXIT_SUCCESS;

}
