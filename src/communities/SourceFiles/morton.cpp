// sortMortonCodes.cpp : Defines the entry point for the console application.

#include "import_geometry.cpp"
#include <iostream>
//#include <string>
#include <cstdint>
#include <cmath>

class Morton {
//using std::string;

//Object to call a Morton Code class
MortonCode objMorton;



		//void binary( double value);


		public:
		void read_csv(string fileName)
		{
			/*Point test(1.2, 1.3);
			for (float i =0.1; i<1.0; i = i+ 0.1)
			{
				cout<< "x: = " << (100.0+ (i *1000)) << " y:= " <<(100.0- (i *1000)) <<" Mortoncode => "<< test.mortonIndex((100.0+i), (100.0-i))<< endl;
			}*/


			/*for(int i=0; i<argc; i++)
					cout<<"argv[ " << i << " ] => "<< argv[i] << endl;*/
			//if (argc == 2){
				//string fileName = argv[0];
				if(fileName.find("csv")!= -1)
					objMorton.importCoordinates(fileName);
				else
					cout<<"filename is invalid \n Valid extension is .csv file"<<endl;

			}
			/*else
				cout<<"file is invalid \n Valid extension is .csv file"<<endl;*/
};
