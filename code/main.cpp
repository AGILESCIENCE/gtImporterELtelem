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
#include "qlbase/InputFileFITS.h"
#include "EVTPacket.h"
#include "LOGPacket.h"
#include "RATPacket.h"
#include "LOGFilter.h"
#include "EVTFilter.h"
#include <locale>
#include <sstream>
#include "qlbase/mac_clock_gettime.h"

//file types
#define EVT 1
#define LOG 2
#define RAT 3

//columns of EVT file
//0 for TIME, 18 for TIMEBAR
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
#define LOG_EARTH_THETA 26
#define LOG_EARTH_PHI 27

//column of RATEMETERS file
#define RAT_TIME 19
#define RAT_M6271_0_AC_TOP1_RATEM 289
#define RAT_M6272_0_AC_TOP2_RATEM 290
#define RAT_M6273_0_AC_TOP3_RATEM 291
#define RAT_M6274_0_AC_LAT1_1_RATEM 292
#define RAT_M6275_0_AC_LAT1_2_RATEM 293
#define RAT_M6276_0_AC_LAT1_3_RATEM 294
#define RAT_M6277_0_AC_LAT2_1_RATEM 295
#define RAT_M6278_0_AC_LAT2_2_RATEM 296
#define RAT_M6279_0_AC_LAT2_3_RATEM 297
#define RAT_M6280_0_AC_LAT3_1_RATEM 298
#define RAT_M6281_0_AC_LAT3_2_RATEM 299
#define RAT_M6282_0_AC_LAT3_3_RATEM 300
#define RAT_M6283_0_AC_LAT4_1_RATEM 301
#define RAT_M6284_0_AC_LAT4_2_RATEM 302
#define RAT_M6285_0_AC_LAT4_3_RATEM 303
#define RAT_M6305_0_PDHU_ratemeter_of_T1_YES_signal 341
#define RAT_M6320_0_PDHU_GRID_events_sent_to_ground 356
#define RAT_M6573_0_MCAL_Fixed_Rt_BoardX 728
#define RAT_M6575_0_MCAL_Fixed_Rt_BoardZ 730


using namespace std;
using namespace qlbase;

struct timespec start, stop;
struct timespec startg, stopg;

bool myisnan(double var) {
        double d = var;
        return d != d;
}



int mainRAT() {
	const char* home = getenv("AGILE");
    if (!home)
    {
    	std::cerr << "AGILE environment variable is not defined." << std::endl;
        exit(0);
    }
    string basedir = home;

	/// The Packet containing the FADC value of each triggered telescope
	AGILETelem::RATPacket* rat = new AGILETelem::RATPacket(basedir + "/share/agiletelem/agile.stream", "/Users/bulgarelli/devel.agile/data_agiletelem/agilerat.raw", "");
	///Read a telemetry packet from .raw file. Return 0 if end of file
	ByteStreamPtr bs = rat->readPacket();
	double lasttime = 0;
	uint32_t counter = 0;

	while(bs != 0) { //if not end of file
		counter++;


		double time = rat->getTime();
		if(lasttime < time)
			lasttime = time;
		else {
			//cout << "error in time " << setprecision(15) << lasttime << " " << time << endl;
			bs = rat->readPacket();
			continue;
		}
		//uint16_t val = rat->getM6283_0_AC_LAT4_1_RATEM();
		//uint16_t val = rat->getM6320_0_PDHU_GRID_events_sent_to_ground();
		//uint16_t val = rat->getM6271_0_AC_TOP2_RATEM();

		//cout << setprecision(15) << time << "\t" << val << endl;

		bs = rat->readPacket();


	}
	//cout << counter << endl;
}

int mainEVT() {
	/// start clock
    //clock_gettime( CLOCK_MONOTONIC, &startg);

	//EVTFilter f("agileevt.phearthL70.thetaG70.poin.raw");
	EVTFilter f("/Users/bulgarelli/devel.agile/data_agiletelem/agilelog.10.poin.raw");
	uint32_t index;
	bool ret;
	double t1, t2;
	t1 = 158868000;
	t2 = t1 + 86400*7;
	//t2 = 159472800;
	ret = f.query(t1, t2, 18, 5, 100, 200, 80, 0, 60);
	cout << "size " << f.time.size() << " capacity " << f.time.capacity() << endl;
	for(int i=0; i<f.time.size(); i++) {
		cout << setprecision(12) << f.time[i] << "\t" << setw(10) << setprecision(5) << f.ra[i] << "\t" << setw(10) << f.dec[i] << "\t" << f.energy[i] << "\t" << (int) f.evstatus[i] << "\t" << (int)f.ph_earth[i] << "\t" << (int) f.theta[i] << endl;
	}
	/*
	double t3 = t1;
	double t4 = t1;
	for(int i=0; i<100; i++) {
		t3 = t4;
		t4 = t3 + tstep;
		f.reset();
		bool ret = f.query(t3, t4, 18);
		cout << "size " << f.time.size() << " capacity " << f.time.capacity() << endl;
	}
	/*
	cout << "size " << f.time.size() << " capacity " << f.time.capacity() << endl;
	bool ret = f.query(t1, t2, 18);
	cout << ret << endl;
	//f.readTimeInterval(index, t1, t2);
	//cout << t1 << " " << t2 << endl;
	cout << "size " << f.time.size() << " capacity " << f.time.capacity() << endl;
	//for(int i=0 ; i< f.time.size(); i++) cout << f.time[i] << endl;
	*/
	/// stop the clock
    //clock_gettime( CLOCK_MONOTONIC, &stopg);
    //double time = difftime((time_t)startg.tv_sec, (time_t)stopg.tv_sec);
    //std::cout << "Total time: " << time << std::endl << std::endl;

	return 0;
}


int mainLOG(string filename) {
	cout << "mainLOG" << endl;
	const char* home = getenv("AGILE");
    if (!home)
    {
    	std::cerr << "AGILE environment variable is not defined." << std::endl;
        exit(0);
    }
    string basedir = home;

	/// start clock
    //clock_gettime( CLOCK_MONOTONIC, &startg);

	//LOGFilter f("/Users/bulgarelli/devel.agile/data_agiletelem/agilelog.10.spink.raw", 10);
	LOGFilter f(filename, 10);
	uint32_t index;
	//190599896.181275 190600896.181275
	/*
	bool ret = f.binary_search(257479896.181275, index, true);
	cout << "index " << index << endl;
	index = 74223319;
	double start, end;
	f.readTimeInterval(index, start, end);
	cout << setprecision(15) << start << " " << end << endl;
	uint32_t mid = f.midpoint(index, 74223334);
	cout << "mid " << mid << endl;
	return 0;
	*/
	//if(ret == false) {
	//	cout << "index not found" << endl;
	//	return -1;
	//}
	//cout << "index " << index << endl;
	/*
	double t1, t2;
	t1 = 158868000;
	t2 = 159472800;
	double tstep = 10;
	double t3 = t1;
	double t4 = t1;
	for(int i=0; i<200000; i++) {
		t3 = t4;
		t4 = t3 + tstep;
		f.reset();
		bool ret = f.query(t3, t4, 18);
		cout << "size " << f.time.size() << " capacity " << f.time.capacity() << endl;
	}

//	cout << "size " << f.time.size() << " capacity " << f.time.capacity() << endl;
//	bool ret = f.query(t1, t2, 18);
//	cout << ret << endl;
//	//f.readTimeInterval(index, t1, t2);
//	//cout << t1 << " " << t2 << endl;
//	cout << "size " << f.time.size() << " capacity " << f.time.capacity() << endl;
//	//for(int i=0 ; i< f.time.size(); i++) cout << f.time[i] << endl;

	/// stop the clock
    clock_gettime( CLOCK_MONOTONIC, &stopg);
    double time = timediff(startg, stopg);
    std::cout << "Total time: " << time << std::endl << std::endl;

	return 0;
	 */



	//AGILETelem::LOGPacket* rat = new AGILETelem::LOGPacket(basedir + "/share/agiletelem/agile.stream", "/Users/bulgarelli/devel.agile/data_agiletelem/agilelog.10.spink.raw", "");
	AGILETelem::LOGPacket* rat = new AGILETelem::LOGPacket(basedir + "/share/agiletelem/agile.stream", filename, "");
	///Read a telemetry packet from .raw file. Return 0 if end of file
	ByteStreamPtr bs = rat->readPacket();
	double lasttime = 0;
	uint32_t counter = 0;
	double lastval = 0;

	while(bs != 0) { //if not end of file
		counter++;


		double time = rat->getTime();
		/*if(lasttime < time)
			lasttime = time;
		else {
			//cout << "error in time " << setprecision(15) << lasttime << " " << time << endl;
			bs = rat->readPacket();
			continue;
		}*/
		//uint16_t val = rat->getM6283_0_AC_LAT4_1_RATEM();
		//uint16_t val = rat->getM6320_0_PDHU_GRID_events_sent_to_ground();
		//uint16_t val = rat->getM6271_0_AC_TOP2_RATEM();

		//double val = rat->getEarthTheta();


		cout << counter << " " << setprecision(15) << time << endl;

		//lastval = val;

		bs = rat->readPacket();


	}
}

///mainR
int mainR() {
	cout << "reader" << endl;
	try
	{

		const char* home = getenv("AGILE");
		if (!home)
		{
			std::cerr << "AGILE environment variable is not defined." << std::endl;
			exit(0);
		}
		string basedir = home;

		int type;
		type = EVT;

		if(type == EVT) {
			/// The Packet containing the FADC value of each triggered telescope
			AGILETelem::EVTPacket* evt = new AGILETelem::EVTPacket(basedir + "/share/agiletelem/agile.stream", "agileevt.baric.igrj17354-3255.poine.raw", "");
			///Read a telemetry packet from .raw file. Return 0 if end of file
			ByteStreamPtr bs = evt->readPacket();

			uint32_t counter = 0;
			while(bs != 0) { //if not end of file
				counter++;
				//print the overall content of the packet
				//evt->printPacket_input();



				//access the packet header information

				cout << "---------------------------------" << endl;
				cout << "---- " << counter <<endl;
				cout << "D: " << evt->getInputPacketDimension(bs) << endl;

				cout << "TIME: " << setprecision(20) << evt->getTime() << endl;
				/*
				cout << "(RA, DEC) = (" << setprecision(7) << evt->getRA() << ", " << evt->getDEC() << ") " << endl;
				cout << "EVSTATUS " <<  (int) evt->getEvstatus() << " PHASE " << (int) evt->getPhase() << endl;
				cout << "ENERGY " << evt->getEnergy() << " PH_EARTH " << (int)evt->getPh_Earth() << " THETA " << (int)evt->getTheta() << endl;
				cout << "---- " << counter <<endl;
				*/
				///Read a telemetry packet from .raw file
				bs = evt->readPacket();


			}
			cout << counter << endl;
		}
		if(type == LOG) {
			/// The Packet containing the FADC value of each triggered telescope
			AGILETelem::LOGPacket* log = new AGILETelem::LOGPacket(basedir + "/share/agiletelem/agile.stream", "agilelog.10.spink.raw", "");
			///Read a telemetry packet from .raw file. Return 0 if end of file
			ByteStreamPtr bs = log->readPacket();

			uint32_t counter = 0;
			double lasttime = 0;
			while(bs != 0) { //if not end of file
				counter++;
				//print the overall content of the packet
				//evt->printPacket_input();

				/*
				cout << "---------------------------------" << endl;
				cout << "---- " << counter <<endl;
				cout << "D: " << log->getInputPacketDimension(bs) << endl;

				cout << "TIME: " << setprecision(20) << log->getTime() << endl;
				cout << "PHASE: " <<  (int)log->getPhase() << endl;
				cout << "LIVETIME: " << setprecision(7) << log->getLivetime() << endl;
				cout << "ATTITUDE_RA_Y: " << log->getAttitudeRaY() << endl;
				cout << "ATTITUDE_DEC_Y: " << log->getAttitudeDecY() << endl;
				cout << "EARTH_RA: " << setprecision(7) << log->getEarthRa() << endl;
				cout << "EARTH_DEC: " << setprecision(7) << log->getEarthDec() << endl;
				cout << "EARTH_THETA: " << setprecision(7) << log->getEarthTheta() << endl;
				cout << "EARTH_PHI: " << setprecision(7) << log->getEarthPhi() << endl;
				cout << "---- " << counter <<endl;
				*/
				double time = log->getTime();

				if(time > 267489283) {
					cerr << "out of time " << setprecision(20)  << time << endl;
					continue;
				}
				if(lasttime>=time) {
					cerr << "time out of order" << setprecision(20)  << time << endl;
					continue;
				}
				lasttime = time;
				///Read a telemetry packet from .raw file
				bs = log->readPacket();


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

///Import AGILE RAT LOG and EVT files into Ice/Freeze/BDB
///mainW
int mainW(string filename, int nrows_end) {

	cout << "gtImporterELpacket" << endl;

	/*if(argc == 1) {
		cerr << "Please, provide (1) the fits file to import (2 optional) the last line to be read" << endl;
		return 0;
	}
	*/

	const char* home = getenv("AGILE");
  if (!home)
  {
    	std::cerr << "AGILE environment variable is not defined." << std::endl;
        exit(0);
  }
  string basedir = home;


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
		if(ncols == 897) type = RAT;


		if(type == EVT) {
			try
    		{
				/// The Packet containing the FADC value of each triggered telescope
        AGILETelem::EVTPacket* evt = new AGILETelem::EVTPacket(basedir + "/share/agiletelem/agile.stream", "", "agileevt.xxxx.raw");



				//read all columns
				cout << "Read EVT file " << endl;
				std::vector<double> time2 = inputFF->read64f(EVT_TIME, nrows_start, nrows_end-1);
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
          //cout << "I " << time2[i] << " " << theta[i] << " " << ph_earth[i] << " " << ra[i] << endl;
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
					if(time2[i] == 0) {
            cout << "time is 0" << endl;
						continue;
          }
          //cout << setprecision(15) << time2[i] << endl;
					evt->setTime(time2[i]);
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
				uint32_t timeStep = 10;

				ostringstream outfilename;
				outfilename << "agilelog." << timeStep << ".xxxx.raw";

        AGILETelem::LOGPacket* log = new AGILETelem::LOGPacket(basedir + "/share/agiletelem/agile.stream", "", outfilename.str());

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
				std::vector<double> log_earth_theta = inputFF->read64f(LOG_EARTH_THETA, nrows_start, nrows_end-1);
				std::vector<double> log_earth_phi = inputFF->read64f(LOG_EARTH_PHI, nrows_start, nrows_end-1);
				//std::vector<float> q1 = inputFF->read32f(LOG_Q1, nrows_start, nrows_end-1);
				//std::vector<float> q2 = inputFF->read32f(LOG_Q2, nrows_start, nrows_end-1);
				//std::vector<float> q3 = inputFF->read32f(LOG_Q3, nrows_start, nrows_end-1);
				//std::vector<float> q4 = inputFF->read32f(LOG_Q4, nrows_start, nrows_end-1);

				uint32_t saved = 0;
				//write data into file
				for(uint32_t i  = 0; i<nrows_end; i+=timeStep) {

					//prefiltering IMPORTANTE
					//str << " && LIVETIME > 0 && LOG_STATUS == 0 && MODE == 2";
          //TIME >= 191808000.000000 && TIME < 197856000.000000 &&
          //LIVETIME > 0 && LOG_STATUS == 0 && MODE == 2 && PHASE .NE. 1 && PHASE .NE. 2 && ((#ROW == 1) || (#ROW == (#ROW/10) *10))
					bool save = false;
					if(livetime[i] > 0 && log_status[i] == 0 && mode[i] == 2 && phase[i] != 1 && phase[i] != 2) // && (i+1) == (((i+1)/timeStep) * timeStep))
						save = true;
					else {
            cout << i << " " << livetime[i] << " " << log_status[i] << " " << mode[i] << " " << phase[i]  << endl;
						continue;
          }
					if( myisnan((double)livetime[i]) || myisnan((double)attitude_ra_y[i]) || myisnan((double)attitude_dec_y[i]) || myisnan((double)log_earth_ra[i]) || myisnan((double)log_earth_dec[i]) || myisnan((double)phase[i]) || myisnan((double) log_earth_theta[i]) || myisnan((double) log_earth_phi[i]) )   {//|| myisnan((double)phi[i])
            cout << i << " nan" << endl;
            continue;
          }
          /*
          if(time[i] > 267489283) {
						cerr << "out of time " << time[i] << endl;
						continue;
					}
          */
					if(time[i-1]>=time[i]) {
						cerr << "time out of order" << endl;
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
						log->setEarthTheta(log_earth_theta[i]);
						log->setEarthPhi(log_earth_phi[i]);
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
		if(type == RAT) {
			try {

				ostringstream outfilename;
				outfilename << "agilerat.raw";
        		AGILETelem::RATPacket* rat = new AGILETelem::RATPacket(basedir + "/share/agiletelem/agile.stream", "", outfilename.str());

				//read all columns
				cout << "Read RAT file " << nrows_end << endl;
				std::vector<double> time = inputFF->read64f(RAT_TIME, nrows_start, nrows_end-1);

				std::vector<uint16_t> M6271_0_AC_TOP1_RATEM = inputFF->read16u(RAT_M6271_0_AC_TOP1_RATEM, nrows_start, nrows_end-1);

				std::vector<uint16_t> M6272_0_AC_TOP2_RATEM= inputFF->read16u(RAT_M6272_0_AC_TOP2_RATEM, nrows_start, nrows_end-1);
				std::vector<uint16_t> M6273_0_AC_TOP3_RATEM= inputFF->read16u(RAT_M6273_0_AC_TOP3_RATEM, nrows_start, nrows_end-1);
				std::vector<uint16_t> M6274_0_AC_LAT1_1_RATEM= inputFF->read16u(RAT_M6274_0_AC_LAT1_1_RATEM, nrows_start, nrows_end-1);
				std::vector<uint16_t> M6275_0_AC_LAT1_2_RATEM= inputFF->read16u(RAT_M6275_0_AC_LAT1_2_RATEM, nrows_start, nrows_end-1);
				std::vector<uint16_t> M6276_0_AC_LAT1_3_RATEM= inputFF->read16u(RAT_M6276_0_AC_LAT1_3_RATEM, nrows_start, nrows_end-1);
				std::vector<uint16_t> M6277_0_AC_LAT2_1_RATEM= inputFF->read16u(RAT_M6277_0_AC_LAT2_1_RATEM, nrows_start, nrows_end-1);
				std::vector<uint16_t> M6278_0_AC_LAT2_2_RATEM= inputFF->read16u(RAT_M6278_0_AC_LAT2_2_RATEM, nrows_start, nrows_end-1);
				std::vector<uint16_t> M6279_0_AC_LAT2_3_RATEM= inputFF->read16u(RAT_M6279_0_AC_LAT2_3_RATEM, nrows_start, nrows_end-1);
				std::vector<uint16_t> M6280_0_AC_LAT3_1_RATEM= inputFF->read16u(RAT_M6280_0_AC_LAT3_1_RATEM, nrows_start, nrows_end-1);
				std::vector<uint16_t> M6281_0_AC_LAT3_2_RATEM= inputFF->read16u(RAT_M6281_0_AC_LAT3_2_RATEM, nrows_start, nrows_end-1);
				std::vector<uint16_t> M6282_0_AC_LAT3_3_RATEM= inputFF->read16u(RAT_M6282_0_AC_LAT3_3_RATEM, nrows_start, nrows_end-1);
				std::vector<uint16_t> M6283_0_AC_LAT4_1_RATEM= inputFF->read16u(RAT_M6283_0_AC_LAT4_1_RATEM, nrows_start, nrows_end-1);
				std::vector<uint16_t> M6284_0_AC_LAT4_2_RATEM= inputFF->read16u(RAT_M6284_0_AC_LAT4_2_RATEM, nrows_start, nrows_end-1);
				std::vector<uint16_t> M6285_0_AC_LAT4_3_RATEM= inputFF->read16u(RAT_M6285_0_AC_LAT4_3_RATEM, nrows_start, nrows_end-1);
				std::vector<uint16_t> M6305_0_PDHU_ratemeter_of_T1_YES_signal= inputFF->read16u(RAT_M6305_0_PDHU_ratemeter_of_T1_YES_signal, nrows_start, nrows_end-1);
				std::vector<uint16_t> M6320_0_PDHU_GRID_events_sent_to_ground= inputFF->read16u(RAT_M6320_0_PDHU_GRID_events_sent_to_ground, nrows_start, nrows_end-1);
				std::vector<uint16_t> M6573_0_MCAL_Fixed_Rt_BoardX= inputFF->read16u(RAT_M6573_0_MCAL_Fixed_Rt_BoardX, nrows_start, nrows_end-1);
				std::vector<uint16_t> M6575_0_MCAL_Fixed_Rt_BoardZ= inputFF->read16u(RAT_M6575_0_MCAL_Fixed_Rt_BoardZ, nrows_start, nrows_end-1);

				uint32_t saved = 0;
				//write data into file
				for(uint32_t i  = 0; i<nrows_end; i++) {

					bool save = true;

					//setting if good
					if(save) {
						rat->setTime(time[i]);
						rat->setM6271_0_AC_TOP1_RATEM((uint16_t) M6271_0_AC_TOP1_RATEM[i]);

						rat->setM6272_0_AC_TOP2_RATEM((uint16_t) M6272_0_AC_TOP2_RATEM[i]);
						rat->setM6273_0_AC_TOP3_RATEM((uint16_t) M6273_0_AC_TOP3_RATEM[i]);
						rat->setM6274_0_AC_LAT1_1_RATEM((uint16_t) M6274_0_AC_LAT1_1_RATEM[i]);
						rat->setM6275_0_AC_LAT1_2_RATEM((uint16_t) M6275_0_AC_LAT1_2_RATEM[i]);
						rat->setM6276_0_AC_LAT1_3_RATEM((uint16_t) M6276_0_AC_LAT1_3_RATEM[i]);
						rat->setM6277_0_AC_LAT2_1_RATEM((uint16_t) M6277_0_AC_LAT2_1_RATEM[i]);
						rat->setM6278_0_AC_LAT2_2_RATEM((uint16_t) M6278_0_AC_LAT2_2_RATEM[i]);
						rat->setM6279_0_AC_LAT2_3_RATEM((uint16_t) M6279_0_AC_LAT2_3_RATEM[i]);
						rat->setM6280_0_AC_LAT3_1_RATEM((uint16_t) M6280_0_AC_LAT3_1_RATEM[i]);
						rat->setM6281_0_AC_LAT3_2_RATEM((uint16_t) M6281_0_AC_LAT3_2_RATEM[i]);
						rat->setM6282_0_AC_LAT3_3_RATEM((uint16_t) M6282_0_AC_LAT3_3_RATEM[i]);
						rat->setM6283_0_AC_LAT4_1_RATEM((uint16_t) M6283_0_AC_LAT4_1_RATEM[i]);
						rat->setM6284_0_AC_LAT4_2_RATEM((uint16_t) M6284_0_AC_LAT4_2_RATEM[i]);
						rat->setM6285_0_AC_LAT4_3_RATEM((uint16_t) M6285_0_AC_LAT4_3_RATEM[i]);
						rat->setM6305_0_PDHU_ratemeter_of_T1_YES_signal((uint16_t) M6305_0_PDHU_ratemeter_of_T1_YES_signal[i]);
						rat->setM6320_0_PDHU_GRID_events_sent_to_ground((uint16_t) M6320_0_PDHU_GRID_events_sent_to_ground[i]);
						rat->setM6573_0_MCAL_Fixed_Rt_BoardX((uint16_t) M6573_0_MCAL_Fixed_Rt_BoardX[i]);
						rat->setM6575_0_MCAL_Fixed_Rt_BoardZ((uint16_t) M6575_0_MCAL_Fixed_Rt_BoardZ[i]);

						rat->writePacket();
						saved++;

						//cout << "------------------- " << i << " " << setprecision(15) << time[i] << " " << (uint16_t) M6271_0_AC_TOP1_RATEM[i]<< " " << M6320_0_PDHU_GRID_events_sent_to_ground[i] << " " << M6283_0_AC_LAT4_1_RATEM[i] << endl;

						//log->printPacket_output();
					}
				}
				cout << "RAT saved " << saved << endl;

			} catch(PacketExceptionIO* e) {
				cout << e->geterror() << endl;;
			}
			catch(PacketException* e)
			{
				cout << e->geterror() << endl;
			}
		}
    return 0;

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
	if(op == 2) {
		string filename = argv[2];
		mainLOG(filename);
	}
	if(op == 3)
		mainEVT();
	///read ratemeters and generate a text file (stdout) with time and a column with the ratemeter
	if(op == 4)
		mainRAT();
}
