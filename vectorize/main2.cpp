#include <iostream>
#include <math.h>
#include <time.h>

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "shapelib/shapefil.h"
#include "vect.h"
#include "vect_RDP.h"


using namespace cv;
using namespace std;

/*int main(int argc, char** argv) {
	// Cargamos la imagen y obtenemos una versión binaria en fondo blanco
	Mat imgOrigi = imread("D:/muestras/adelgazar.png", CV_LOAD_IMAGE_COLOR);
	Mat imgGrises, imgBinaria;

	if (!imgOrigi.data) {
		cout << " No image data\n";
		return -1;
	}
	
	cvtColor(imgOrigi, imgGrises, CV_BGR2GRAY); // a escala de grises
	threshold(imgGrises, imgBinaria, 200, 255, CV_THRESH_BINARY); // filtro de umbral para obtener la versión binaria
	copyMakeBorder(imgBinaria, imgBinaria, 1, 1, 1, 1, BORDER_CONSTANT, 255); // agregamos borde de 1px
	
	vect::execVectorize(imgBinaria, "vectorizado", 1, 1);
	
	return 0;
}*/
