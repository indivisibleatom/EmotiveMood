#include <iostream>
#include <fstream>
#include <conio.h>
#include <sstream>
#include <windows.h>
#include <map>

#include "EmoStateDLL.h"
#include "edk.h"
#include "edkErrorCode.h"
#include "serial.h"
#include <iostream>

#pragma comment(lib, "../lib/edk.lib")

void logEmoState(std::ostream& os, unsigned int userID, EmoStateHandle eState, bool withHeader = false);
Serial* SP = new Serial("COM4");    // adjust as needed

void sendSerialData(const char* data)
{
	bool writeResult = 0;

	if (!SP->WriteData(data,strlen(data)))
	{
		std::cout<<"Error in writing data";
	}
}

int main(int argc, char** argv) {

	EmoEngineEventHandle eEvent			= EE_EmoEngineEventCreate();
	EmoStateHandle eState				= EE_EmoStateCreate();
	unsigned int userID					= 0;
	const unsigned short composerPort	= 1726;
	int option = 0;
	int state  = 0;
	std::string input;

	
	if (SP->IsConnected())
		std::cout<<"We're connected";

	try {

		if (argc != 2) {
			throw std::exception("Please supply the log file name.\nUsage: EmoStateLogger [log_file_name].");
		}

		std::cout << "===================================================================" << std::endl;
		std::cout << "Example to show how to log the EmoState from EmoEngine/EmoComposer." << std::endl;
		std::cout << "===================================================================" << std::endl;
		std::cout << "Press '1' to start and connect to the EmoEngine                    " << std::endl;
		std::cout << "Press '2' to connect to the EmoComposer                            " << std::endl;
		std::cout << ">> ";

		std::getline(std::cin, input, '\n');
		option = atoi(input.c_str());

		switch (option) {
			case 1:
			{
				if (EE_EngineConnect() != EDK_OK) {
					throw std::exception("Emotiv Engine start up failed.");
				}
				break;
			}
			case 2:
			{
				std::cout << "Target IP of EmoComposer? [127.0.0.1] ";
				std::getline(std::cin, input, '\n');

				if (input.empty()) {
					input = std::string("127.0.0.1");
				}

				if (EE_EngineRemoteConnect(input.c_str(), composerPort) != EDK_OK) {
					std::string errMsg = "Cannot connect to EmoComposer on [" + input + "]";
					throw std::exception(errMsg.c_str());
				}
				break;
			}
			default:
				throw std::exception("Invalid option...");
				break;
		}
		
		
		std::cout << "Start receiving EmoState! Press any key to stop logging...\n" << std::endl;

		std::ofstream ofs(argv[1]);
		bool writeHeader = true;
		
		while (!_kbhit()) {

			state = EE_EngineGetNextEvent(eEvent);

			// New event needs to be handled
			if (state == EDK_OK) {

				EE_Event_t eventType = EE_EmoEngineEventGetType(eEvent);
				EE_EmoEngineEventGetUserId(eEvent, &userID);

				// Log the EmoState if it has been updated
				if (eventType == EE_EmoStateUpdated) {

					EE_EmoEngineEventGetEmoState(eEvent, eState);
					const float timestamp = ES_GetTimeFromStart(eState);

					printf("%10.3fs : New EmoState from user %d ...\r", timestamp, userID);
					
					logEmoState(ofs, userID, eState, writeHeader);
					writeHeader = false;
				}
			}
			else if (state != EDK_NO_EVENT) {
				std::cout << "Internal error in Emotiv Engine!" << std::endl;
				break;
			}

			//Sleep(1000);
		}

		ofs.close();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		std::cout << "Press any key to exit..." << std::endl;
		getchar();
	}

	EE_EngineDisconnect();
	EE_EmoStateFree(eState);
	EE_EmoEngineEventFree(eEvent);

	return 0;
}


void logEmoState(std::ostream& os, unsigned int userID, EmoStateHandle eState, bool withHeader) {
	/*// Create the top header
	if (withHeader) {
		os << "Time,";
		os << "UserID,";
		os << "Wireless Signal Status,";
		os << "Blink,";
		os << "Wink Left,";
		os << "Wink Right,";
		os << "Look Left,";
		os << "Look Right,";
		os << "Eyebrow,";
		os << "Furrow,";
		os << "Smile,";
		os << "Clench,";
		os << "Smirk Left,";
		os << "Smirk Right,";
		os << "Laugh,";
		os << "Short Term Excitement,";
		os << "Long Term Excitement,";
		os << "Engagement/Boredom,";
		os << "Cognitiv Action,";
		os << "Cognitiv Power,";
		os << std::endl;
	}

	// Log the time stamp and user ID
	os << ES_GetTimeFromStart(eState) << ",";
	os << userID << ",";
	os << static_cast<int>(ES_GetWirelessSignalStatus(eState)) << ",";

	// Expressiv Suite results
	os << ES_ExpressivIsBlink(eState) << ",";
	os << ES_ExpressivIsLeftWink(eState) << ",";
	os << ES_ExpressivIsRightWink(eState) << ",";

	os << ES_ExpressivIsLookingLeft(eState) << ",";
	os << ES_ExpressivIsLookingRight(eState) << ",";

	std::map<EE_ExpressivAlgo_t, float> expressivStates;

	EE_ExpressivAlgo_t upperFaceAction = ES_ExpressivGetUpperFaceAction(eState);
	float			   upperFacePower  = ES_ExpressivGetUpperFaceActionPower(eState);

	EE_ExpressivAlgo_t lowerFaceAction = ES_ExpressivGetLowerFaceAction(eState);
	float			   lowerFacePower  = ES_ExpressivGetLowerFaceActionPower(eState);

	expressivStates[ upperFaceAction ] = upperFacePower;
	expressivStates[ lowerFaceAction ] = lowerFacePower;
	
	os << expressivStates[ EXP_EYEBROW     ] << ","; // eyebrow
	os << expressivStates[ EXP_FURROW      ] << ","; // furrow
	os << expressivStates[ EXP_SMILE       ] << ","; // smile
	os << expressivStates[ EXP_CLENCH      ] << ","; // clench
	os << expressivStates[ EXP_SMIRK_LEFT  ] << ","; // smirk left
	os << expressivStates[ EXP_SMIRK_RIGHT ] << ","; // smirk right
	os << expressivStates[ EXP_LAUGH       ] << ","; // laugh*/

	// Affectiv Suite results
	//os << ES_AffectivGetExcitementShortTermScore(eState) << ",";
	float scoreY = ES_AffectivGetExcitementShortTermScore(eState);
	float scoreX = ES_AffectivGetEngagementBoredomScore(eState);

	std::cout<<"Sending data "<<scoreY<<" "<<scoreX;

	if (scoreY < 0.5 && scoreX < 0.5)
	{
		sendSerialData("2");
	}
	else if (scoreY < 0.5 && scoreX >= 0.5)
	{
		sendSerialData("3");
	}
	else if (scoreY >= 0.5 && scoreX < 0.5 )
	{
		sendSerialData("1");
	}
	else
	{
		sendSerialData("0");
	}

	// Cognitiv Suite results
	//os << static_cast<int>(ES_CognitivGetCurrentAction(eState)) << ",";
	//os << ES_CognitivGetCurrentActionPower(eState);

	//os << std::endl;
}
