#include "vect.h"

// Obtiene la vecindad de un punto de la imagen binaria (1 solo si es un pixel ocupado)
// Obligatorio recibir por parámetro uchar vecindad[8]; ya que no se puede crear y devolver arreglos
void *vect::vecindad(Mat &img, Point pun, uchar vec[]) {
	vec[0] = (img.at<uchar>(pun.y-1, pun.x) == 0) ? 1 : 0;
	vec[1] = (img.at<uchar>(pun.y-1, pun.x+1) == 0) ? 1 : 0;
	vec[2] = (img.at<uchar>(pun.y, pun.x+1) == 0) ? 1 : 0;
	vec[3] = (img.at<uchar>(pun.y+1, pun.x+1) == 0) ? 1 : 0;
	vec[4] = (img.at<uchar>(pun.y+1, pun.x) == 0) ? 1 : 0;
	vec[5] = (img.at<uchar>(pun.y+1, pun.x-1) == 0) ? 1 : 0;
	vec[6] = (img.at<uchar>(pun.y, pun.x-1) == 0) ? 1 : 0;
	vec[7] = (img.at<uchar>(pun.y-1, pun.x-1) == 0) ? 1 : 0;
}

// Devuelve el código Greenlee de la vecindad de un pixel
uchar vect::codVecindad(Mat &img, Point pun) {
	uchar vecinos[8];
	vecindad(img, pun, vecinos);
	return 128 * vecinos[0] + vecinos[1] + 2 * vecinos[2]
		+ 4 * vecinos[3] + 8 * vecinos[4] + 16 * vecinos[5]
		+ 32 * vecinos[6] + 64 * vecinos[7];
}

// TRUE si es punto final
bool vect::esPunFinal(Mat &img, Point pun) {
	uchar cod = codVecindad(img, pun);
	return (cod == 128 || cod == 2 || cod == 8 || cod == 32 || cod == 1 || cod == 4 || cod == 16 || cod == 64);
}

void vect::puntosFinales(Mat &img, vector<Point> &punFinales) {
	unsigned int x, y, numFilas, numCols;
	uchar* fila;
	uchar intensidad;
	Point pun;

	numFilas = img.rows;
	numCols = img.cols;

	// Recorremos la imagen pixel por pixel (excepto el borde)
	for (y = 1 ; y < (numFilas - 1) ; ++y) {
		fila = img.ptr<uchar>(y);
		for (x = 1 ; x < (numCols - 1) ; ++x) {
			intensidad = fila[x];
			if (intensidad == 0) {
				pun = Point(x, y);
				if (esPunFinal(img, pun)) {
					punFinales.push_back(pun);
				}
			}
		}
	}
}

// Desplaza un punto en la dirección según el código greenlee especificado
void vect::moverPuntoCod(Point &pun, uchar cod) {
	switch (cod) {
		case 1: pun.x++; pun.y--; break;
		case 2: pun.x++; break;
		case 4: pun.x++; pun.y++; break;
		case 8: pun.y++; break;
		case 16: pun.x--; pun.y++; break;
		case 32: pun.x--; break;
		case 64: pun.x--; pun.y--; break;
		case 128: pun.y--; break;
	}
}

SHPObject *vect::toArc(vector<Point2f> vecPuntosTrazo, int imgHeight) {
	int numPuntos = vecPuntosTrazo.size();
	double coordsX[numPuntos], coordsY[numPuntos];

	int i;
	for (i = 0; i < numPuntos; i++) {
		coordsX[i] = vecPuntosTrazo[i].x;
		coordsY[i] = imgHeight - vecPuntosTrazo[i].y;
	}

	SHPObject *objShp = SHPCreateSimpleObject(SHPT_ARC, numPuntos, coordsX, coordsY, NULL);
	return objShp;
}

// A partir del trazo, genera y guarda el vector correspondiente en formato SHAPE
void vect::agregarShape(vector<Point2f> trazo, int altura, SHPHandle sh) {
	SHPObject *objShp = toArc(trazo, altura);
	SHPWriteObject(sh, -1, objShp);
	SHPDestroyObject(objShp);
}


// mueve un punto de posición en una dirección (0:arr, 1:arrder, etc)
void vect::moverPunto(Point &punto, uchar dir) {
	switch (dir) {
		case 0: punto.y--; break;
		case 1: punto.x++; punto.y--; break;
		case 2: punto.x++; break;
		case 3: punto.x++; punto.y++; break;
		case 4: punto.y++; break;
		case 5: punto.x--; punto.y++; break;
		case 6: punto.x--; break;
		case 7: punto.x--; punto.y--; break;
	}
}


// Recorre un trazo desde pun y almacena sus puntos en trazo
// No removemos puntos colineales para mejorar los resultados del Ramer-Douglas-Peuker
void vect::recorrerTrazo(Mat &imgBase, Point pun, vector<Point2f> &trazo, bool trazoCerrado) {
	uchar cod;
	Point2f punIni;
	trazo.clear();
	
	// Validamos el trazo a recorrer
	if (imgBase.at<uchar>(pun) == 255) return;
	if (codVecindad(imgBase, pun) == 0) return;
	
	punIni = pun;
	trazo.push_back(punIni);
	
	// Si es trazo cerrado, borramos 1 pixel e iniciamos desde el siguiente pixel detectado
	if (trazoCerrado) {
		uchar i = 0;
		uchar vecinos[8];
		
		
		vecindad(imgBase, pun, vecinos);
		imgBase.at<uchar>(pun) = 255;
		while (vecinos[i] != 1) i++;
		moverPunto(pun, i);
	}
	
	
	while (imgBase.at<uchar>(pun) != 255) {
		cod = codVecindad(imgBase, pun);
		imgBase.at<uchar>(pun) = 255; // punto recorrido se borra

		if (cod != 0) {
			trazo.push_back(pun);
			moverPuntoCod(pun, cod);
		}
		else {
			if (trazo.size() > 0) trazo.push_back(pun);
		}
	}
	
	// Si es trazo cerrado, unir los últimos 2 puntos
	if (trazoCerrado) trazo.push_back(punIni);
}

/**
 * Vectoriza todos los trazos de una imagen binaria. Los trazos deben estar completamente adelgazados
 * @param imgBase Imagen binaria CV_8U
 * @param filename Nombre del archivo de salida (sin extensión)
 * @param deltaRDP Delta de reducción de puntos Ramen-Douglas-Peucker, entre más grande más agresivo, en 0 no se hace reducción de puntos (valores ejemplo: 0.5, 1, 2...)
 * @param suavizado Nivel de suavizado con interpolación spline Catmull-Rom, en 0 no se suavizan los trazos (valores ejemplo: 0, 1, 2, 4...)
 */
void vect::execVectorize(Mat &imgBase, const char *filename, float deltaRDP, short int suavizado) {
	SHPHandle sh = SHPCreate(filename, SHPT_ARC);
	int imgAncho = imgBase.cols;
	int imgAlto = imgBase.rows;
	
	// recorremos toda la imagen buscando puntos finales
	vector<Point> punFinales;
	vect::puntosFinales(imgBase, punFinales);
	
	// para cada punto final, recorremos el trazo correspondiente y guardamos el vector
	Point pun;
	vector<Point2f> trazo;
	vector<Point2f> trazoSuave;
	for(vector<Point>::iterator it = punFinales.begin(); it != punFinales.end(); ++it) {
		pun = *it;
		recorrerTrazo(imgBase, pun, trazo, false);
		
		if (trazo.size() > 1) {
			vec_RDP::reducirPuntos(trazo, deltaRDP); // aplicamos reducción de puntos Ramer-Douglas-Peucker
			catmull::interpolar(trazo, trazoSuave, suavizado);
			agregarShape(((suavizado > 0) ? trazoSuave : trazo), imgAlto, sh);
		}
	}
	
	// Buscamos en toda la imagen, si aún hay pixeles sin borrar, son de trayectos cerrados
	unsigned int x, y;
	uchar* fila;
	uchar intensidad;

	for (y = 1 ; y < (imgAlto - 1) ; ++y) {
		fila = imgBase.ptr<uchar>(y);
		for (x = 1 ; x < (imgAncho - 1) ; ++x) {
			intensidad = fila[x];
			if (intensidad == 0) {
				pun = Point(x, y);
				
				recorrerTrazo(imgBase, pun, trazo, true);
				
				if (trazo.size() > 1) {
					vec_RDP::reducirPuntos(trazo, deltaRDP); // aplicamos reducción de puntos Ramer-Douglas-Peucker
					catmull::interpolar(trazo, trazoSuave, suavizado);
					agregarShape(((suavizado > 0) ? trazoSuave : trazo), imgAlto, sh);
				}
			}
		}
	}
	
	SHPClose(sh);
}