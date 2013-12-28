/***************************************************************************
                          main.cpp
                             -------------------
    copyright            : (C) 2013 A. Bulgarelli
    email                : bulgarelli@iasfbo.inaf.it

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/



#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <iomanip>
#include <string>
#include "InputFileFITS.h"
#include "EVTPacket.h"
#include "LOGPacket.h"
#include "LOGFilter.h"
#include <locale>
#include <sstream>
#include "mac_clock_gettime.h"

//file types
#define EVT 1
#define LOG 2

//columns of EVT file
#define EVT_TIME 0
#define EVT_PHI 2
#define EVT_RA 3
#define EVT_DEC 4
#define EVT_ENERGY 5
#define EVT_PH_EARTH 14
#define EVT_THETA 1
#define EVT_PHASE 15
#define EVT_EVSTATUS 13

//columsn of LOG file
#define LOG_TIME 0
#define LOG_PHASE 5
#define LOG_LIVETIME 39
#define LOG_LOG_STATUS 40
#define LOG_MODE 4
#define LOG_ATTITUDE_RA_Y 13
#define LOG_ATTITUDE_DEC_Y 14
#define LOG_EARTH_RA 24
#define LOG_EARTH_DEC 25
#define LOG_Q1 9
#define LOG_Q2 10
#define LOG_Q3 11
#define LOG_Q4 12

using namespace std;
using namespace qlbase;

struct timespec start, stop;
struct timespec startg, stopg;

bool myisnan(double var) {
        double d = var;
        return d != d;
}


int mainQ() {
	/// start clock
    clock_gettime( CLOCK_MONOTONIC, &startg);
        
	LOGFilter f("agilelog.10.poin.raw", 10);
	/*uint32_t index;
	bool ret = f.binary_search(150000013.9, index, true);
	if(ret == false) {
		cout << "index not found" << endl;
		return -1;
	}
	cout << "index " << index << endl;
	*/
	double t1, t2;
	t1 = 150000013.9;
	t2 = t1 - 0.01 + 86400*30;
	cout << "size " << f.time.size() << " capacity " << f.time.capacity() << endl;
	long size = f.query(t1, t2, 18);
	cout << size << endl;
	//f.readTimeInterval(index, t1, t2);
	//cout << t1 << " " << t2 << endl;
	cout << "size " << f.time.size() << " capacity " << f.time.capacity() << endl;
	//for(int i=0 ; i< f.time.size(); i++) cout << f.time[i] << endl;
	
	/// stop the clock
    clock_gettime( CLOCK_MONOTONIC, &stopg);
    double time = timediff(startg, stopg);
    std::cout << "Total time: " << time << std::endl << std::endl;
	
	return 0;
}

///mainR
int mainR() {
	cout << "reader" << endl;
	try
	{
		
		
		int type;
		type = LOG;
		if(type == EVT) {
			/// The Packet containing the FADC value of each triggered telescope
			AGILETelem::EVTPacket* evt = new AGILETelem::EVTPacket("./conf/agile.stream", "agileevt.raw", "");
			///Read a telemetry packet from .raw file. Return 0 if end of file
			byte* b_evt = evt->readPacket();
		
			uint32_t counter = 0;
			while(b_evt != 0) { //if not end of file
				counter++;
				//print the overall content of the packet
				//evt->printPacket_input();

			

				//access the packet header information
			
				cout << "---------------------------------" << endl;
				cout << "---- " << counter <<endl;
				cout << "D: " << evt->getInputPacketDimension(b_evt) << endl;
			
				cout << "TIME: " << setprecision(20) << evt->getTime() << endl;
				/*
				cout << "(RA, DEC) = (" << setprecision(7) << evt->getRA() << ", " << evt->getDEC() << ") " << endl;
				cout << "EVSTATUS " <<  (int) evt->getEvstatus() << " PHASE " << (int) evt->getPhase() << endl;
				cout << "ENERGY " << evt->getEnergy() << " PH_EARTH " << (int)evt->getPh_Earth() << " THETA " << (int)evt->getTheta() << endl;
				cout << "---- " << counter <<endl;
				*/
				///Read a telemetry packet from .raw file
				b_evt = evt->readPacket();
			
			
			}
			cout << counter << endl;
		}
		if(type == LOG) {
			/// The Packet containing the FADC value of each triggered telescope
			AGILETelem::LOGPacket* log = new AGILETelem::LOGPacket("./conf/agile.stream", "agilelog.160.raw", "");
			///Read a telemetry packet from .raw file. Return 0 if end of file
			byte* b_evt = log->readPacket();
		
			uint32_t counter = 0;
			while(b_evt != 0) { //if not end of file
				counter++;
				//print the overall content of the packet
				//evt->printPacket_input();

				cout << "---------------------------------" << endl;
				cout << "---- " << counter <<endl;
				cout << "D: " << log->getInputPacketDimension(b_evt) << endl;
			
				cout << "TIME: " << setprecision(20) << log->getTime() << endl;
				cout << "PHASE: " <<  (int)log->getPhase() << endl;
				cout << "LIVETIME: " << setprecision(7) << log->getLivetime() << endl;
				cout << "ATTITUDE_RA_Y: " << log->getAttitudeRaY() << endl;
				cout << "ATTITUDE_DEC_Y: " << log->getAttitudeDecY() << endl;
				cout << "EARTH_RA: " << setprecision(7) << log->getEarthRa() << endl;
				cout << "EARTH_DEC: " << setprecision(7) << log->getEarthDec() << endl;
				cout << "---- " << counter <<endl;
				
				///Read a telemetry packet from .raw file
				b_evt = log->readPacket();
			
			
			}
			cout << counter << endl;
		}
		return 0;
	}
	catch(PacketExceptionIO* e)
	{
		cout << e->geterror() << endl;;
	}
	catch(PacketException* e)
	{
		cout << e->geterror() << endl;
	}
	
}

///Import AGILE LOG and EVT files into Ice/Freeze/BDB
///mainW
int mainW(string filename, int nrows_end) {

	cout << "gtImporterELpacket" << endl;
	
	/*if(argc == 1) {
		cerr << "Please, provide (1) the fits file to import (2 optional) the last line to be read" << endl;
		return 0;
	}
	*/

	
	InputFileFITS* inputFF;


	//open input file FITS
	try{
		inputFF = new InputFileFITS();
		inputFF->open(filename);
		inputFF->moveToHeader(1); //CHECK base index AZ


		//check input file fits type: EVT or LOG
		int ncols = inputFF->getNCols();
		int nrows_start = 0;
		if(nrows_end == -1)
			nrows_end = inputFF->getNRows();
		int type;
		//cout << nrows_end << endl;

		if(ncols == 19) type = EVT;
		if(ncols == 41) type = LOG;


		if(type == EVT) {
			try
    		{
				/// The Packet containing the FADC value of each triggered telescope
        		AGILETelem::EVTPacket* evt = new AGILETelem::EVTPacket("./conf/agile.stream", "", "agileevt.raw");
        	
        			
			
				//read all columns
				cout << "Read EVT file " << endl;
				std::vector<double> time = inputFF->read64f(EVT_TIME, nrows_start, nrows_end-1);
				//std::vector<float> phi = inputFF->read32f(EVT_PHI, nrows_start, nrows_end-1);
				std::vector<float> ra = inputFF->read32f(EVT_RA, nrows_start, nrows_end-1);
				std::vector<float> dec = inputFF->read32f(EVT_DEC, nrows_start, nrows_end-1);
				std::vector<float> energy = inputFF->read32f(EVT_ENERGY, nrows_start, nrows_end-1);
				std::vector<float> ph_earth = inputFF->read32f(EVT_PH_EARTH, nrows_start, nrows_end-1);
				std::vector<float> theta = inputFF->read32f(EVT_THETA, nrows_start, nrows_end-1);
				std::vector<int16_t> phase = inputFF->read16i(EVT_PHASE, nrows_start, nrows_end-1);
				std::vector< std::vector<char> > status = inputFF->readString(EVT_EVSTATUS, nrows_start, nrows_end-1, 1);
				std::vector<int16_t> status2(nrows_end-nrows_start+1);

				for(uint32_t i  = 0; i<nrows_end; i++) {
					std::string evt;
					evt = &(status[i])[0];
					//cout << evt << endl;
					if(evt.compare("G") == 0) status2[i] = 0;
					if(evt.compare("L") == 0) status2[i] = 1;
					if(evt.compare("S") == 0) status2[i] = 2;

				}
				//write data into file
				uint32_t saved = 0;
				for(uint32_t i  = 0; i<nrows_end; i++) {
				
					//prefiltering
					if( myisnan((double)ra[i]) || myisnan((double)dec[i]) || myisnan((double)energy[i]) || myisnan((double)ph_earth[i]) || myisnan((double)theta[i]) || myisnan((double)phase[i]) ) { //|| myisnan((double)phi[i])  
                    	cout << i << " nan" << endl;
                        continue;
                    }
                    //prefiltering2 -  rimuovere tutti quelli con ph_earth[i] < 70 e con theta[i] > 70
                    if(ph_earth[i] < 70) {
                    	cout <<  " ph_earth["<<i<<"] < 70" << endl;
                    	continue;
                    }
					if(theta[i] > 70) {
                    	cout << " theta["<<i<<"] > 70" << endl;
                    	continue;
                    }
                    
					evt->setTime(time[i]);
					evt->setRA(ra[i]);
					evt->setDEC(dec[i]);
					evt->setEvstatus(status2[i]);
					evt->setPhase(phase[i]);
					evt->setEnergy((word)energy[i]);
					evt->setPh_Earth((word)ph_earth[i]);
					evt->setTheta(theta[i]);
					
					evt->writePacket();
					saved++;
					//evt->printPacket_output();
				}
				cout << "EVT saved " << saved << endl;

			}
			catch(PacketExceptionIO* e)
			{
				cout << e->geterror() << endl;;
			}
			catch(PacketException* e)
			{
				cout << e->geterror() << endl;
			}

		}
		if(type == LOG) {
			try
    		{
				/// timeStep parameter
				uint32_t timeStep = 160;
				
				ostringstream outfilename;
				outfilename << "agilelog." << timeStep << ".spin.raw";
        		AGILETelem::LOGPacket* log = new AGILETelem::LOGPacket("./conf/agile.stream", "", outfilename.str());

				//read all columns
				cout << "Read LOG file " << nrows_end << endl;
				std::vector<double> time = inputFF->read64f(LOG_TIME, nrows_start, nrows_end-1);
				std::vector<int16_t> phase = inputFF->read16i(LOG_PHASE, nrows_start, nrows_end-1);
				std::vector<float> livetime = inputFF->read32f(LOG_LIVETIME, nrows_start, nrows_end-1);
				std::vector<int16_t> log_status = inputFF->read16i(LOG_LOG_STATUS, nrows_start, nrows_end-1);
				std::vector<int16_t> mode = inputFF->read16i(LOG_MODE, nrows_start, nrows_end-1);
				std::vector<double> attitude_ra_y = inputFF->read64f(LOG_ATTITUDE_RA_Y, nrows_start, nrows_end-1);
				std::vector<double> attitude_dec_y = inputFF->read64f(LOG_ATTITUDE_DEC_Y, nrows_start, nrows_end-1);
				std::vector<double> log_earth_ra = inputFF->read64f(LOG_EARTH_RA, nrows_start, nrows_end-1);
				std::vector<double> log_earth_dec = inputFF->read64f(LOG_EARTH_DEC, nrows_start, nrows_end-1);
				//std::vector<float> q1 = inputFF->read32f(LOG_Q1, nrows_start, nrows_end-1);
				//std::vector<float> q2 = inputFF->read32f(LOG_Q2, nrows_start, nrows_end-1);
				//std::vector<float> q3 = inputFF->read32f(LOG_Q3, nrows_start, nrows_end-1);
				//std::vector<float> q4 = inputFF->read32f(LOG_Q4, nrows_start, nrows_end-1);

				uint32_t saved = 0;
				//write data into file
				for(uint32_t i  = 0; i<nrows_end; i++) {
					
					//prefiltering IMPORTANTE
					//str << " && LIVETIME > 0 && LOG_STATUS == 0 && MODE == 2";
					bool save = false;
					if(livetime[i] > 0 && log_status[i] == 0 && mode[i] == 2 && (i+1) == (((i+1)/timeStep) * timeStep)) 
						save = true;
					else 
						continue;
						
					if( myisnan((double)livetime[i]) || myisnan((double)attitude_ra_y[i]) || myisnan((double)attitude_dec_y[i]) || myisnan((double)log_earth_ra[i]) || myisnan((double)log_earth_dec[i]) || myisnan((double)phase[i]) )   {//|| myisnan((double)phi[i]) 
                    	cout << i << " nan" << endl;
                        continue;
                    }
					
					//setting if good
					if(save) {
						log->setPhase(phase[i]);
						log->setTime(time[i]);
						log->setLivetime(livetime[i]);
						log->setAttitudeRaY(attitude_ra_y[i]);
						log->setAttitudeDecY(attitude_dec_y[i]);
						log->setEarthRa(log_earth_ra[i]);
						log->setEarthDec(log_earth_dec[i]);
					
						log->writePacket();
						saved++;
					
						cout << "------------------- " << i << " " << setprecision(15) << time[i] <<  endl;
						
						//log->printPacket_output();
					}
				}
				cout << "LOG saved " << saved << endl;

			}
			catch(PacketExceptionIO* e)
			{
				cout << e->geterror() << endl;;
			}
			catch(PacketException* e)
			{
				cout << e->geterror() << endl;
			}

			
			
		}
	} catch(IOException* e) {
		cout << e->getErrorCode() << ": " << e->what() << endl;
		return e->getErrorCode();
	}
}

int main(int argc, char** argv) {

	int op = atoi(argv[1]);

	if(op == 0) {
		string filename = argv[2];
		int nrows_end = -1;
		if(argc == 4) {
			nrows_end = atoi(argv[3]);
		}
		mainW(filename, nrows_end);
	}
	if(op == 1)
		mainR();
	if(op == 2)
		mainQ();
}
