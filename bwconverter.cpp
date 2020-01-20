/// @brief This program converts a colored image to gray-scale(like a baws)

#include <iostream>
#include <fstream>

using namespace std;

const int maxWidth  = 1024;  //max height of image
const int maxHeight = 768;   //max width of image

//Setup struct for each pixel using short data type because it uses less space than an int.  struct pixel_t  {
struct pixel_t {
    short red;
    short green;
    short blue;
};

struct image_t {
    int width;      ///< The image width in pixels.
    int height;     ///< The image height in pixels.
    int colors;     ///< The image max color value.
    pixel_t pixelData[maxHeight][maxWidth]; //Create an array of pixel data for image
};


//function prototypes
bool checkArgs(int argc, char* argv[]);
bool openInputFile(ifstream& infile, char* argv[]);
bool checkFileHeader(ifstream& infile);
bool checkImageSize(ifstream& infile, image_t& image);
bool checkFileHeader(ifstream& infile);

void openOutput(ofstream& outfile, char* argv[]);
void readImageData(ifstream& infile, image_t& image);
void grayscale(image_t& image);
void createImage(ofstream& outfile, const image_t& image, const char* name);
void createNewPixel(ofstream& outfile, const pixel_t& pixel);

int main(int argc, char * argv[]) {
    int exitCode = EXIT_FAILURE;
    image_t image;

    ifstream infile;
    ofstream outfile;

    if(checkArgs(argc, argv)) {
        openInputFile(infile, argv);
        checkFileHeader(infile);
        if(checkImageSize(infile, image)) {
            openOutput(outfile, argv);
            readImageData(infile, image);
            grayscale(image);
            createImage(outfile, image, argv[0]);
        }
    }

    return exitCode;
}


/// ---------------------------------------------------------------------------
/// Check if the user passed in the correct set of arguments(filename, desination file)
///
/// @param argc   Number of command line args
/// @param argv   Command line arguments passed in
/// ---------------------------------------------------------------------------

bool checkArgs(int argc, char* argv[]) {
    //state of functio
    //n
    bool validArgs = true;
    //Check if user passed in file name
    if(argc == 1) {
        cerr << "Error: missing input file name" << endl;
    }

    //If User passed in 2 args check if they are the same file
    if (argc > 2) {
        if (strcmp(argv[1], argv[2]) == 0) {
            cerr << "Error: input and output filenames must differ" << endl;
            validArgs = false;
        }
    } else {
        validArgs = false;
    }

    //Check if user passed in output file name
    if (argc < 3) {
        cerr << "Error: missing output filename" << endl;
        validArgs = false;
    }

  return validArgs;
}

/// ---------------------------------------------------------------------------
/// Check if the file has the magic number
///
/// @param infile The image input file
/// ---------------------------------------------------------------------------

bool checkFileHeader(ifstream& infile) {
    //Define magic number function state and store the first line of file
    bool hasMagicNumber = false;
    string line1;

    getline(infile, line1);

    //Check if the header is "P3" and the length of the letter isn't smaller than 2
    //(lines taken from pa08)
    if ((line1.length() < 2) || (line1.substr(0, 2) != "P3")) {
        cerr << "Error: invalid file type" << endl;
        infile.close();
    } else {
        hasMagicNumber = true;
    }

    return hasMagicNumber;
}

/// ---------------------------------------------------------------------------
/// Check if the size of the image does not exceed the max ammounts
///
/// @param infile The image input file
/// @param image  image struct that contains the image values
/// ---------------------------------------------------------------------------

bool checkImageSize(ifstream& infile, image_t& image) {
    bool correctImage = true;

    infile.ignore(UINT_MAX, '\n');
    if (infile.peek() == '#') {
        infile.ignore(UINT_MAX, '\n');
    }

    infile >> image.width >> image.height >> image.colors;

    if((image.width > maxWidth) || (image.height > maxHeight)) {
        cerr << "Image exceeds maximum limit" << endl;
        correctImage = false;
    }

    return correctImage;
}

/// ---------------------------------------------------------------------------
/// Open the output stream and check if the file is not open yet
///
/// @param outfile image output file
/// @param argv    command line arguments
/// ---------------------------------------------------------------------------

void openOutput(ofstream& outfile, char* argv[]) {
    outfile.open(argv[2]);

    if (!outfile.good()) {
        cerr << "Cannot open output file: " << argv[2] << endl;
    }
}

/// ---------------------------------------------------------------------------
/// Function to get an individual pixel from the input file
///
/// @param infile image input file
/// @param pixel   pixel struct that contains pixel data
/// ---------------------------------------------------------------------------

void individualPixel(ifstream& infile, pixel_t& pixel) {
    infile >> pixel.red >> pixel.green >> pixel.blue;
}

/// ---------------------------------------------------------------------------
/// Read the image data into the pixel data array using the individual pixel
/// functio to get each pixel and put it into the pixelData matrix
///
/// @param infile image input file
/// @param image  image struct that contains image data
/// ---------------------------------------------------------------------------

void readImageData(ifstream& infile, image_t& image) {
    for (int i = 0; i < image.height; ++i) {
        for (int j = 0; j < image.width; ++j) {
            individualPixel(infile, image.pixelData[i][j]);      // get pixel into struct
        }
    }
}

/// ---------------------------------------------------------------------------
/// Open file and return true if it is good to go
///
/// @param infile The current ppm file
/// @param argv   Command line arguments passed in
/// ---------------------------------------------------------------------------

bool openInputFile(ifstream& infile, char* argv[]) {
    bool isFileOpen = true;

    infile.open(argv[1]);

    if (!infile.good()) {
        cerr << "Error: file may be corrupt" << endl;
        isFileOpen = false;
    }

    return isFileOpen;
}

/// ---------------------------------------------------------------------------
/// Get each pixel in the image pixelData matrix and change all of them to the
/// average.
///
/// @param image  image struct that contains image data
/// ---------------------------------------------------------------------------

void grayscale(image_t& image) {
    for (int i = 0; i < image.height; i++) {
        for (int j = 0; j < image.width; j++) {
            pixel_t& pixel = image.pixelData[i][j];
            short average = (pixel.red + pixel.green + pixel.blue) / 3;
            pixel.red = average;
            pixel.green = average;
            pixel.blue = average;
        }
    }
}

/// ---------------------------------------------------------------------------
/// Another function to help write the data to the output stream seperated by
/// spaces.
///
/// @param outfile output data file
/// @param pixel pixel struct that contains pixel data using constant to keep
//               immutable.
/// ---------------------------------------------------------------------------

void createNewPixel(ofstream& outfile, const pixel_t& pixel) {
    outfile << pixel.red << ' ' << pixel.green << ' ' << pixel.blue << ' ';
}

/// ---------------------------------------------------------------------------
/// Function to write the new header to the new file and add each value from
/// the image pixeldata matrix using the writing function to get each individual
/// pixel.
///
/// @param outfile output data file
/// @param image image struct that contains image data using cont to keep
//               immutable.
//  @param name name of file executing function
/// ---------------------------------------------------------------------------

void createImage(ofstream& outfile, const image_t& image, const char* name) {
    outfile << "P3\n";
    outfile << "# Created by " << name << '\n';
    outfile << image.width << ' ' << image.height << '\n';
    outfile << image.colors << '\n';
    for (int i = 0; i < image.height; i++) {
        for (int j = 0; j < image.width; j++) {
            createNewPixel(outfile, image.pixelData[i][j]);
        }
        outfile << endl;
    }
}
