
// Ulises Olivares
// uolivares@unam.mx
// June 13, 2020.

#include "raster.h"


// Utility method for comparing two cells
static bool operator<(const cellVecinos& a, const cellVecinos& b){
	if (a.distance == b.distance){
		if (a.x != b.x)
			return (a.x < b.x);
		else
			return (a.y < b.y);
	}
	return (a.distance < b.distance);
}



bool Raster::isInsideGrid(int i, int j){
        return (i >= 0 && i < ROWS && j >= 0 && j < COLS);
}


float** Raster::importRaster(string file, bool flag) {
    GDALDataset *dataset;
    //char **MD;
    //char *info;
    GDALAllRegister();
    string ds = file;
    dataset = (GDALDataset *) GDALOpen(ds.c_str(), GA_ReadOnly);
    //double ndv;

    // File not found
    if(dataset == NULL) {
        cout << file << ": No such file or directory, please specify a valid path and name file..." << endl;
        exit(0);
    }

    GDALRasterBand  *poBand;

    poBand = dataset->GetRasterBand(1);

    dataset->GetGeoTransform( adfGeoTransform );
    projection = adfGeoTransform[1];

    proj = dataset->GetProjectionRef();

    string tail = proj.substr(proj.size() - 20);

    int d1, d2;

    d1 = tail.find(",");
    d2 = tail.find("\"", d1 + 2);

    epsg = tail.substr(d1 + 2, d2 - d1 - 2);


    int nXSize = poBand->GetXSize();
    int nYSize = poBand->GetYSize();

    ROWS = nYSize; COLS = nXSize;

    costos = new float*[ROWS];
    for(int i = 0; i< ROWS; ++i)
        costos[i] = new float[COLS];


    //GDALDataType dataType = poBand->GetRasterDataType();
    float *pBuf = new float[nYSize * nXSize];

    poBand->RasterIO(GF_Read, 0, 0, nXSize, nYSize, pBuf, nXSize, nYSize, GDT_Float32, 0, 0);

    noDataValue = poBand->GetNoDataValue();


    float biomass = 0;

    int cCols = 0, cRows = 0;
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            int location = (nXSize * (i)) + j;
            if (cCols == COLS - 1) {
                cCols = 0;
                cRows++;
            }
            else {
                costos[cRows][cCols] = *(pBuf+location);
                //if(costos[cRows][cCols])
                //	cout << costos[cRows][cCols] << endl;
                if(costos[cRows][cCols] > 0 && flag){
                    valid_points++;
                    biomass += costos[cRows][cCols];
                }
                cCols++;
            }
        }
    }
    if(flag){
        this->avgBiomass = biomass / (valid_points);
    }

    return costos;
}

void Raster::writeImage(float** output_raster, int rows, int cols, string heuristic, int stop, string map, string algName) {
    float* arr = new float[rows * cols * 3];
    int n_pixels = rows * cols; //channelDiv = round(stop /255);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            arr[i * cols + j] = output_raster[i][j];
            /*if(outputRaster[i][j] > 0 && outputRaster[i][j] < 0.1)
                arr[i * cols + j] = outputRaster[i][j] * 3;
            else if(outputRaster[i][j] >= 0.1)
                arr[i * cols + j] = outputRaster[i][j];
            else
                arr[i * cols + j] = 0;*/
        }
    }
    Mat matrix = Mat(rows, cols, CV_32FC1, arr);
    Mat outMatrix;
    Mat im_color = Mat::zeros(rows, cols, CV_32FC4);

    vector<Mat> planes;
    vector<Mat>::iterator itMat;
    for(int i = 0; i < 4; i++)
        planes.push_back(matrix);
    merge(planes, im_color);

    for(int i = 0; i < n_pixels; i++) {
        Vec4f &v = im_color.at<Vec4f>(i/cols, i%cols);
        if(v[0] > 0) {
            v.val[0] = v[0] / (projection * projection) * 1050;
            v.val[1] = v[1] / (projection * projection) * 450;// / channelDiv;
            v.val[2] = v[2] / (projection * projection) * 150;// / channelDiv;
            v.val[3] = 255;
            //cout << v.val[2] << endl;
        }
    }

    std::ostringstream ostr;
    ostr << stop;
    string sStop = ostr.str();
    string fileName = "final_route_"+map+"_"+algName+"_"+sStop+"_"+heuristic+".png";
    imwrite(fileName, im_color);
}




void Raster::defineGridSize(int demand, int &xIntervals, int &yIntervals) {

    // Estimate of pixels for each grid given demmand and avg of biomass in the map
    pixels_necesarios = ceil(demand / avgBiomass);
    gridSize = ceil(sqrt(pixels_necesarios));
    yIntervals = ceil(COLS / (double) gridSize);
    xIntervals = ceil(ROWS / (double) gridSize);
}

/*
 *
 */
map<float,Grid> Raster::defineGrids(int rows, int cols, const int &xIntervals, const int &yIntervals, float** biomass, float** friction) {
        cout << "-----------------------------------------" <<endl;
        cout << "Defining grids ... " <<endl;

        int xPosGrid, yPosGrid, id = 1, count2 = 0, cont = 0, contValid = 0;

        Grid** totalGrids = new Grid*[xIntervals];
        // Initialize totalGrids
        for (int i = 0; i< xIntervals; i++) {
            totalGrids[i] = new Grid[yIntervals];
        }

        Point2D tmp;
        for(int i = 0; i < rows; i++)
            for(int j = 0; j < cols; j++) {
                xPosGrid = floor(i / gridSize);
                yPosGrid = floor(j / gridSize);
                //FIXME: Change tmp
                tmp.x = i;
                tmp.y = j;


                if (biomass[i][j] >= 0 && friction[i][j] > 0 && biomass[i][j] > friction[i][j]) {
                    totalGrids[xPosGrid][yPosGrid].noElements++;
                    totalGrids[xPosGrid][yPosGrid].elements.push_back(tmp);
                    totalGrids[xPosGrid][yPosGrid].sum += biomass[i][j] / friction[i][j];
                    totalGrids[xPosGrid][yPosGrid].value = biomass[i][j]; // it overrides previous value
                    totalGrids[xPosGrid][yPosGrid].biomass += biomass[i][j];
                    totalGrids[xPosGrid][yPosGrid].friction += friction[i][j];
                    contValid++;

                }
                else {
                    totalGrids[xPosGrid][yPosGrid].invalidCells = totalGrids[xPosGrid][yPosGrid].invalidCells + 1;
                    // replece for non valid element
                    totalGrids[xPosGrid][yPosGrid].value = noDataValue;
                }

                count2++;

                if (totalGrids[xPosGrid][yPosGrid].elements.size() + totalGrids[xPosGrid][yPosGrid].invalidCells == (gridSize * gridSize)
                    && totalGrids[xPosGrid][yPosGrid].biomass > 0 && totalGrids[xPosGrid][yPosGrid].friction > 0
                    && totalGrids[xPosGrid][yPosGrid].biomass > totalGrids[xPosGrid][yPosGrid].friction
                    /*&& totalGrids[xPosGrid][yPosGrid].elements.size() > totalGrids[xPosGrid][yPosGrid].invalidCells*/ ) {

                        totalGrids[xPosGrid][yPosGrid].id = id;
                        totalGrids[xPosGrid][yPosGrid].biomassAvg = totalGrids[xPosGrid][yPosGrid].biomass / totalGrids[xPosGrid][yPosGrid].elements.size();
                        if(totalGrids[xPosGrid][yPosGrid].biomassAvg >= avgBiomass)
                            totGridsAvg++;
                        id++;
                        cont++;
                        float gridSum = totalGrids[xPosGrid][yPosGrid].biomass / totalGrids[xPosGrid][yPosGrid].friction;
                        gridsMap.insert(pair<float,Grid>(gridSum, totalGrids[xPosGrid][yPosGrid]));
                }
            }
        totValidGrids = cont;
        cout << "-----------------------------------------" <<endl;
        return gridsMap;
    }


    // TODO: REPLACE THIS METHOD FOR THE EM one
Point2D Raster::findCentroid(map<float,Grid> grids, float** biomass, float** friction) {
    map<float,Grid>::iterator it;
    float xMax = FLT_MIN, xMin = FLT_MAX, yMax = FLT_MIN, yMin = FLT_MAX;
    /*map<float,Grid>::iterator it2;
    for ( it = gridsMap.begin(); it != gridsMap.end(); ++it) {
        float xMax = FLT_MIN, xMin = FLT_MAX, yMax = FLT_MIN, yMin = FLT_MAX;
        cout << it->second.elements.size() + it->second.invalidCells << "\t Relation: " << it->first  << "\t Biomass: " << it->second.biomass << "\t Friction: " << it->second.friction << endl;
    }
    cout << "Finished. " << gridsMap.size() << endl;
    //exit(0);*/
    if (!grids.empty()) {
        it = (--grids.end());
    } else {
        flag = false;
    }

    Point2D centroid;

    if(flag){
        cout << "Relation: " << it->first << endl;
        for (int i = 0; i < it->second.elements.size(); i++) {
            if(it->second.elements.at(i).x > xMax)
                xMax = it->second.elements.at(i).x;

            if(it->second.elements.at(i).x < xMin)
                xMin = it->second.elements.at(i).x;

            if(it->second.elements.at(i).y > yMax)
                yMax = it->second.elements.at(i).y;

            if(it->second.elements.at(i).y < yMin)
                yMin = it->second.elements.at(i).y;
        }

        centroid.x = xMin + round((xMax - xMin) / 2);
        centroid.y = yMin + round((yMax - yMin) / 2);
        //cout << xMin << " - " << xMax << " - " << yMin << " - " << yMax << endl;
        this->xMax = xMax; this->xMin = xMin;
        this->yMax = yMax; this->yMin = yMin;
        if(biomass[centroid.x][centroid.y] < 0 || biomass[centroid.x][centroid.y] < friction[centroid.x][centroid.y]){
            cout << "Invalid Centroid: " << centroid.x << ", " << centroid.y << endl;
            bool found = true;
            set<cellVecinos> vecinos = vecinos2(centroid.x, centroid.y);
            set<cellVecinos> celdas;
            celdas.insert(cellVecinos(centroid.x, centroid.y, 0));
            set <cellVecinos> :: iterator itr;

                while(found) {
                    for (itr = vecinos.begin(); itr != vecinos.end(); ++itr){
                        //cout << (*itr).x << ", " << (*itr).y << endl;
                        if(biomass[(*itr).x][(*itr).y] > 0 &&  biomass[(*itr).x][(*itr).y] > friction[(*itr).x][(*itr).y]){
                            //cout << biomass[(*itr).x][(*itr).y] << " " << friction[(*itr).x][(*itr).y] << endl;
                            centroid.x = (*itr).x;
                            centroid.y = (*itr).y;
                            found = false;
                            break;
                        }
                    }
                    vecinos = vecinos3(vecinos);
                }
        }
    }
    else{
        centroid.x = -1;
        centroid.y = -1;
    }
    return centroid;
}



set<cellVecinos> Raster::vecinos2(int origen_x, int origen_y){//busca vecinos iniciales nada mas
        set<cellVecinos>distancias;

        int dx[] = { -1, -1, 0, 1, 1, 1, 0,-1 };
        int dy[] = {  0,  1, 1, 1, 0, -1, -1,-1 };

        set<cellVecinos> st;
        Point2D tmp;

        // insert (0, 0) cell with 0 distance
        st.insert(cellVecinos(origen_x, origen_y, 0));

        cellVecinos k = *st.begin();
        st.erase(st.begin());
        //cout << "k.x = " << k.x << " k.y = " << k.y << " k.distance = " << k.distance <<endl;
        // looping through all neighbours
        for (int i = 0; i < 8; i++){
            int x = k.x + dx[i];
            int y = k.y + dy[i];
            //cout << "x = " << x << " y = " << y << endl;
            // if not inside boundry, ignore them
            if (!isInsideGrid(x, y)){
                //cout << "fuera del grid" << endl;
                continue;
            }

            distancias.insert(cellVecinos(x, y, 0));
            tmp.x = x; tmp.y=y;
            active_raster.push_back(tmp);
        }


        return distancias;
    }

set<cellVecinos> Raster::vecinos3(set<cellVecinos> vecinos){//obtiene vecinos que aun no han sido considerados
    set<cellVecinos>distancias;
    //cout << "\t recibo estos:" << endl;
    set <cellVecinos> :: iterator itr2;
    for (itr2 = vecinos.begin(); itr2 != vecinos.end(); ++itr2){
        //cout << "\t " << (*itr2).x << ", " << (*itr2).y << endl;
    }
    int dx[] = { -1, -1, 0, 1, 1, 1, 0,-1 };
    int dy[] = {  0,  1, 1, 1, 0, -1, -1,-1 };

    Point2D tmp;
    vector<Point2D>::iterator it;
    set <cellVecinos> :: iterator itr;
    for (itr = vecinos.begin(); itr != vecinos.end(); ++itr){
        cellVecinos k = *itr;
        //cout << "\t -k" << k.x << ", "<< k.y << endl;
    // looping through all neighbours
        for (int i = 0; i < 8; i++){
            int x = k.x + dx[i];
            int y = k.y + dy[i];
            it = find_if(active_raster.begin(), active_raster.end(), Point2D(x, y));
            //cout << "\t x = " << x << " y = " << y << " activeRaster= " << activeRaster[x][y] << endl;
            // if not inside boundry, ignore them
            if (!isInsideGrid(x, y) || it != active_raster.end()){
                //cout << "\t fuera del grid" << endl;
                continue;
            }
            distancias.insert(cellVecinos(x, y, 0));
            tmp.x = x; tmp.y = y;
            active_raster.push_back(tmp);

        }
    }

    return distancias;
}

void Raster::exportTiff(string path, float** output_raster, int rows, int cols, string heuristic, int stop, string map, string algName) {
    cout << "Exporting "<< map << " tiff ..." <<endl;
    GDALDataset *poDstDS;
    //char **papszOptions = NULL;
    GDALDriver *poDriver;
    OGRSpatialReference oSRS;
    std::ostringstream ostr;
    ostr << stop;
    string sStop = ostr.str();
    string fileName = path + "final_route_"+map+"_"+algName+"_"+sStop+"_"+heuristic+".tiff";
    string proyeccion = "EPSG:" + epsg;
    poDriver = GetGDALDriverManager()->GetDriverByName("Gtiff");
    poDstDS = poDriver->Create( fileName.c_str(), cols, rows, 1, GDT_Float32, NULL);
    poDstDS->SetGeoTransform(adfGeoTransform);
    poDstDS->SetProjection(proyeccion.c_str());
    oSRS.SetWellKnownGeogCS(proyeccion.c_str());

    GDALRasterBand *poBand;
    float *pBuf = new float[rows * cols], maxVal = 0;
    for(int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            pBuf[i * cols + j] = output_raster[i][j];
            /*if(outputRaster[i][j] <= 2000)
                pBuf[i * cols + j] = outputRaster[i][j];
            else
                pBuf[i * cols + j] = -9999;*/
            if(output_raster[i][j] > maxVal)
                maxVal = output_raster[i][j];
        }
    }

    poBand = poDstDS->GetRasterBand(1);
    poDstDS->GetRasterBand(1)->SetNoDataValue(noDataValue);
    poBand->RasterIO( GF_Write, 0, 0, cols, rows,
                      pBuf, cols, rows, GDT_Float32, 0, 0 );
    GDALClose( (GDALDatasetH) poDstDS );
    //cout << fixed << "Max Val: " << maxVal << endl;
}

void Raster::check_npa(float** npa_matrix, float** &biomass_matrix) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (npa_matrix[i][j] > 0) {
                biomass_matrix[i][j] = biomass_matrix[i][j] / 80;
            }
        }
    }
}

void Raster::check_waterbodies(float** water_matrix, float** &biomass_matrix) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (water_matrix[i][j] > 0)
                biomass_matrix[i][j] = biomass_matrix[i][j] / 80;
        }
    }
}

void Raster::reproject_coords(string map_biomass) {
    string coords = "coords.txt", cmd = "gdaltransform " + map_biomass + " -t_srs EPSG:4326 -s_srs EPSG:" + epsg + " < " + coords;
    array<char, 128> buffer;
    string result;
    shared_ptr<FILE> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
            result += buffer.data();
    }
    cout << "Source = " << result << endl;
    //memset(&buffer[0], 0, sizeof(buffer));
    //pipe.reset();
    //exit(0);
}

/*
 * Method to generate a raster for the selected grid
 * option: 1: Biomass 2: Friction 3: quotient
 */
float ** Raster::generateGridRaster(Grid grid, float** biomass, float** friction, int option) {

    // Initialize matrix
    float ** gridRaster;
    gridRaster = new float*[ROWS];
    for(int i = 0; i< ROWS; ++i)
        gridRaster[i] = new float[COLS];

    int x = grid.elements.at(0).x, y= grid.elements.at(0).y;
    int count =0;

    for(int i = 0; i< ROWS; i++){
        for (int j = 0; j < COLS; j++){
            if (i == x && j == y && count < grid.noElements-1) {
                //cout << "Enter if count: " << count <<   endl ;
                switch (option) {
                    case 1:
                        gridRaster[i][j] = biomass[x][y];
                        break;
                    case 2:
                        gridRaster[i][j] = friction[x][y];
                        break;
                    case 3:
                        gridRaster[i][j] = biomass[x][y] / friction[x][y];
                }
                count++;
                x = grid.elements.at(count).x;
                y = grid.elements.at(count).y;
                //cout << " If i: " << i << " j: " << j << " Elements: " << grid.noElements << " count: " << count<< endl;

            }
            else {
                //cout<< "Nodata"  << noDataValue <<endl;
                gridRaster[i][j] = (float) noDataValue;
                //cout << " Else i: " << i << " j: " << j <<endl;
            }

        }
    }
    return gridRaster;
}


Point2D Raster::runEM(map<float,Grid> grids, float** biomass, float** friction, string outPath){
    cout << "----------------------------------------" << endl;
    cout << "Runing EM Algortihtm ... " << endl;

    Point2D origin;

    // Start from the last element (greater quotient)
    map<float,Grid>::iterator it;
    it = --grids.end();


    // Export image grid

    //Generate biomass grid raster
    float **gridRaster = generateGridRaster(it->second, biomass, friction, 1);
    exportTiff(outPath, gridRaster, ROWS, COLS, "none", 0, "Biomass-Honduras", "-grid");

    //Generate friction grid raster
    gridRaster = generateGridRaster(it->second, biomass, friction, 2);
    exportTiff(outPath, gridRaster, ROWS, COLS, "none", 0, "Friction-Honduras", "-grid");

    // Generate quotient  grid raster
    gridRaster = generateGridRaster(it->second, biomass, friction, 3);
    exportTiff(outPath, gridRaster, ROWS, COLS, "none", 0, "Quotient-Honduras", "-grid");



    const int numElements = it->second.noElements;
    const int N1 = floor(sqrt((double)numElements));
    const int numClusters = ceil(1 + log2(it->second.noElements));
    Scalar colors[numClusters];

    int increment = floor(255/numClusters);

    // Define colors temp code just to export
    for (int i = 0; i < numClusters; i++){
        Scalar col;
        int rnd = rand() % 3 ;
        for (int j = 0; j<3; j++){
            if (j == rnd)
                col[j] = i*increment;
            else
                col[j] = 128;
        }
        colors[i] = col;
    }

    int i, j;

    // Calculate mean and standard deviation
    float maxNum=  FLT_MIN, minNum = FLT_MAX, range = 0.0;
    int smplMean = 0, smplSd = 0;

    // Calculate mean and stdev
    for (int i =0; i< numElements; i++){
        float current = (float) biomass[it->second.elements.at(i).x][it->second.elements.at(i).y] / friction[it->second.elements.at(i).x][it->second.elements.at(i).y];
        if (current < minNum)
            minNum = current;
        if (current > maxNum)
            maxNum = current;
        smplMean += current;
    }

    for (int i =0; i< numElements; i++)
        smplSd += pow(((biomass[it->second.elements.at(i).x][it->second.elements.at(i).y] / friction[it->second.elements.at(i).x][it->second.elements.at(i).y]) - smplMean) , 2);
    smplSd = (int) sqrt(smplSd / (numElements - 1));

    smplMean = (int) (smplMean / numElements);
    range =  maxNum - minNum;

    Mat samples( numElements, 2, CV_32FC1 );
    Mat labels;
    Mat img = Mat::zeros( Size( N1, N1 ), CV_8UC3 );
    Mat sample( 1, 2, CV_32FC1 );

    Ptr<EM> em_model = EM::create();


    samples = samples.reshape(2, 0);
    for( i = 1; i < numClusters; i++ )
    {
        // form the training samples
        Mat samples_part = samples.rowRange(i*numElements/numClusters, (i+1)*numElements/numClusters );
        int test1 = i*numElements/range;
        int test2 = (i+1)*numElements/range;
        Scalar mean(((i%N1)+1)*img.rows/(N1+1),
                    ((i/N1)+1)*img.rows/(N1+1));
        Scalar sigma(30,30);
        randn( samples_part, mean, sigma );
    }
    samples = samples.reshape(1, 0);


    // initialize model parameters
    em_model->setClustersNumber(numClusters);
    em_model->setCovarianceMatrixType(EM::COV_MAT_SPHERICAL);
    em_model->setTermCriteria(TermCriteria(TermCriteria::COUNT+TermCriteria::EPS, 500, 0.1));

    // cluster the data
    em_model ->trainEM(samples, noArray(), labels, noArray());

#if 0
    // the piece of code shows how to repeatedly optimize the model
    // with less-constrained parameters
    //(COV_MAT_DIAGONAL instead of COV_MAT_SPHERICAL)
    // when the output of the first stage is used as input for the second one.
    CvEM em_model2;
    params.cov_mat_type = CvEM::COV_MAT_DIAGONAL;
    params.start_step = CvEM::START_E_STEP;
    params.means = em_model.get_means();
    params.covs = (const CvMat**)em_model.get_covs();
    params.weights = em_model.get_weights();

    em_model2.train( samples, Mat(), params, &labels );
    // to use em_model2, replace em_model.predict()
    // with em_model2.predict() below
#endif
    //classify every pixel of the original data
    for (int i = 0; i<numElements; i++){
        //cout<< "Number of elements " << sqrt(it->second.elements.size())<< endl;
        float current = biomass[it->second.elements.at(i).x][it->second.elements.at(i).y] / friction[it->second.elements.at(i).x][it->second.elements.at(i).y];
        int x = it->second.elements.at(i).x;
        int y =  it->second.elements.at(i).x;
        sample.at<float>(0) = x;
        sample.at<float>(1) = y;
        int response = cvRound(em_model->predict( sample ));
        Scalar c = colors[response];
        circle( img, Point(x, y), 1, c*0.75, FILLED );
    }

    //draw the clustered samples
    cout << "X," <<"Y," <<"tmpX," << "tmpY," << "quotient," << "cluster" << endl;
    for( i = 0; i < numElements; i++ ){
        float current = biomass[it->second.elements.at(i).x][it->second.elements.at(i).y] / friction[it->second.elements.at(i).x][it->second.elements.at(i).y];
        Point pt(cvRound(samples.at<float>(i, 0)), cvRound(samples.at<float>(i, 1)));
        circle( img, pt, 1, colors[labels.at<int>(i)], FILLED );
        //cout  << it->second.elements.at(i).x << " , " << it->second.elements.at(i).y << " , "<<current <<" , " << labels.at<int>(i)   << endl;
        cout  << it->second.elements.at(i).x << "," << it->second.elements.at(i).y <<"," << samples.at<float>(i, 0) << "," << samples.at<float>(i, 1) << ","<<current <<"," << labels.at<int>(i)   << endl;
    }

    //imshow( "EM-clustering result", img );
    //waitKey(0);






    // TODO:evaluate a defined number of best quotients.
    /*const int numElements = it->second.noElements;
    const int N1 = floor(sqrt((double)numElements));
    Mat img = Mat::zeros( Size(N1, N1), CV_8UC3 );
    Mat sample( 1, 2, CV_32FC1 );

    // Create an opencv float mat
    Mat inputSamples(numElements, 2, CV_32F);
    Mat labels;

    // Use the Sturges formula to define the number of clusters
    const int numClusters = ceil(1 + log2(it->second.noElements));

    cout <<"Clustering data ..." << endl;

    // Calculate mean and standard deviation
    float maxNum=  FLT_MIN, minNum = FLT_MAX, range = 0.0;
    int smplMean = 0, smplSd = 0;

    // Calculate mean and stdev
    for (int i =0; i< numElements; i++){
        float current = (float) biomass[it->second.elements.at(i).x][it->second.elements.at(i).y] / friction[it->second.elements.at(i).x][it->second.elements.at(i).y];
        if (current < minNum)
            minNum = current;
        if (current > maxNum)
            maxNum = current;
        smplMean += current;
    }
    smplMean = (int) (smplMean / numElements);
    range =  maxNum - minNum;
    // Calculate Std dev
    for (int i =0; i< numElements; i++)
        smplSd += pow(((biomass[it->second.elements.at(i).x][it->second.elements.at(i).y] / friction[it->second.elements.at(i).x][it->second.elements.at(i).y]) - smplMean) , 2);
    smplSd = (int) sqrt(smplSd / (numElements - 1));
    cout << "Mean: " << smplMean << " Sd: " << smplSd << endl;

    // Reshape inputSamples
    inputSamples = inputSamples.reshape(2, 0);

    // Create Mixed Gaussian model
    for(int i = 0; i < numClusters; i++){
        // form the training samples
        Mat samples_part = inputSamples.rowRange(i * numElements / numClusters, (i + 1) * numElements / numClusters );
        Scalar mean(((i%N1)+1)*img.rows/(N1+1),
                    ((i/N1)+1)*img.rows/(N1+1));
        Scalar sigma(smplSd,smplSd);
        randn(samples_part, mean, sigma);
        cout << " ELement: " << " => " << samples_part.at<float>(0)<< endl;
    }

    inputSamples = inputSamples.reshape(1, 0);

    cout<<"Training model EM..." << endl;

    int iterations = 300, epsilon = 0.1;

    Ptr<EM> em_model = EM::create();
    em_model->setClustersNumber(numClusters);
    em_model->setCovarianceMatrixType(EM::COV_MAT_SPHERICAL);
    em_model->setTermCriteria(TermCriteria(TermCriteria::COUNT+TermCriteria::EPS, iterations, epsilon));
    em_model ->trainEM(inputSamples, noArray(), labels, noArray());

    cout << "Model training finished successfully!" <<endl;
    // classify every image pixel
    cout << "clasifying pixels into clusters" << endl;

    for( int i = 0; i < img.rows; i++ ){
        for( int j = 0; j < img.cols; j++ ){
            float current = biomass[it->second.elements.at(i).x][it->second.elements.at(i).y] / friction[it->second.elements.at(i).x][it->second.elements.at(i).y];
            sample.at<float>(0) = (float)j;
            sample.at<float>(1) = (float)i;
            int response = cvRound(em_model->predict( sample ));
            //Scalar c = colors[response];
            cout  << it->second.elements.at(i).x << " , " << it->second.elements.at(i).y << " , "<<current <<" , " << response << endl;
            //circle( img, Point(j, i), 1, c*0.75, CV_FILLED );
        }
    }*/

    /*for (int i = 0; i<numElements; i++){
        float current = biomass[it->second.elements.at(i).x][it->second.elements.at(i).y] / friction[it->second.elements.at(i).x][it->second.elements.at(i).y];
        //sample.at<float>(0) = i;
        sample.at<float>(0) = current;
        int response = cvRound(em_model->predict2(sample, noArray())[1]);
        cout  << it->second.elements.at(i).x << " , " << it->second.elements.at(i).y << " , "<<current <<" , " << response << endl;
        //Scalar c = colors[response];
        //circle(img, Point(it->second.elements.at(i).x, it->second.elements.at(i).y), 1, c*0.75, FILLED);
    }*/

    /*//draw the clustered samples
    for( int i = 0; i < numElements; i++ ){
        Point pt(cvRound(inputSamples.at<float>(i, 0)), cvRound(inputSamples.at<float>(i, 1)));
        //circle( img, pt, 1, colors[labels.at<int>(i)], FILLED );
    }

    imshow( "EM-clustering result", img );
    waitKey(0);*/


    cout << "----------------------------------------" << endl;

    return origin;
}

    int Raster::getCols() {
        return COLS;
    }

    int Raster::getRows() {
        return ROWS;
    }

    int Raster::getValidPoints(){
        return valid_points;
    }

    int Raster::getIntervals(){
        return gridSize;
    }

    double Raster::getProjection(){
        return projection;
    }

    int Raster::getTotalGrids(){
        return totGridsAvg;
    }

    int Raster::getXMin(){
        return  xMin;
    }

    int Raster::getYMin(){
        return  yMin;
    }

    int Raster::getXMax(){
        return  xMax;
    }

    int Raster::getYMax(){
        return  yMax;
    }




		/*void divide_biomass(float** &biomass_matrix) {
			for (int i = 0; i < ROWS; i++) {
				for (int j = 0; j < COLS; j++) {
					biomass_matrix[i][j] = biomass_matrix[i][j] / 40;
				}
			}
		}*/

/*	void exportTiff(float** outputRaster, int rows, int cols) {


			setenv("PYTHONPATH",".",1);
			Py_Initialize();

			PyObject *pName, *pModule, *pDict, *pFunc;


			PyObject* pArgs = PyTuple_New(rows*cols + 2);
			PyTuple_SetItem(pArgs, 0, Py_BuildValue("i", rows));
			PyTuple_SetItem(pArgs, 1, Py_BuildValue("i", cols));

			int c = 2;

			for (int i = 0; i < rows; i++)
				for (int j = 0; j < cols; j++, c++)
					PyTuple_SetItem(pArgs, c, Py_BuildValue("f", outputRaster[i][j]));

			pName = PyString_FromString((char*)"write_array");

			pModule = PyImport_Import(pName);

			pDict = PyModule_GetDict(pModule);

			pFunc = PyDict_GetItemString(pDict, (char*)"writeArray");


		   if (PyCallable_Check(pFunc)){
			   PyErr_Print();
			   PyObject_CallObject(pFunc, pArgs);
			   //cout << "Done" << endl;
			   //PyObject_CallFunctionObjArgs(pFunc, pRows, pCols, pArgs);
			   //PyErr_Print();
		   } else {
			   printf("Err\n");
			   PyErr_Print();
		   }
		   //cout << "Done" << endl;
		}  */

