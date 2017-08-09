void printArray(uint16 * arr, uint16 width) {
	uint32 i;
	float n;
	//cout << "I'm in " << times << endl;
	for (i = 0; i < width; i++) {
		//printf("%u ", arr(i));
		n = *(float*) (&arr[i]);
		cout << n << "\t";
	}
	cout << endl;
	//times++;
}

void read_tiff() {
   //int width = 0, height = 0;
   TIFF *tif = TIFFOpen("img.tif", "r");
   if (tif) {
	   uint32 imageLength, height;
	 //tsize_t scanline;
	   tdata_t buf;
	   uint32 row, config;

	   TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &imageLength);
	   TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &height);
	   TIFFGetField(tif, TIFFTAG_PLANARCONFIG, &config);
	   //scanline = TIFFScanlineSize(tif);
	   buf = _TIFFmalloc(TIFFScanlineSize(tif));
	   //cout << imageLength << "   " << height << endl;


	   uint16 s, nsamples;
	   uint16* data;
	   TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &nsamples);
	   for (s = 0; s < nsamples; s++) {
		   for (row = 0; row < imageLength; row++) {
			   TIFFReadScanline(tif, buf, row, s);
			   data = (uint16*) buf;
			   printArray(data, imageLength);
		   }
	   }

	   _TIFFfree(buf);
	   TIFFClose(tif);
   }
   exit(0);
}
