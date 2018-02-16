/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <iostream>
#include <string>

#include <WPILib.h>
#include <Drive/DifferentialDrive.h>
#include <Joystick.h>
#include <SampleRobot.h>
#include <SmartDashboard/SendableChooser.h>
#include <SmartDashboard/SmartDashboard.h>
#include <Spark.h>
#include <Talon.h>
#include <Timer.h>
#include <ADXRS450_Gyro.h>
#include <SPI.h>
#include <DigitalInput.h>

using namespace std;
using namespace frc;

/**
 * This is a demo program showing the use of the DifferentialDrive class.
 * The SampleRobot class is the base of a robot application that will
 * automatically call your Autonomous and OperatorControl methods at the right
 * time as controlled by the switches on the driver station or the field
 * controls.
 *
 * WARNING: While it may look like a good choice to use for your code if you're
 * inexperienced, don't. Unless you know what you are doing, complex code will
 * be much more difficult under this system. Use IterativeRobot or Command-Based
 * instead if you're new.
 */
class Robot : public SampleRobot {
public:

	Robot() {
		// Note SmartDashboard is not initialized here, wait until
		// RobotInit to make SmartDashboard calls
		RobotDrive.SetExpiration(0.1);
	}

	void RobotInit() {

		c_Mode.AddDefault(autoOnLeft, autoOnLeft);
		c_Mode.AddObject(autoOnRight, autoOnRight);
		c_Mode.AddObject(autoOnCenter, autoOnCenter);
		c_Mode.AddObject(autoIdle, autoIdle);
		c_Prioritize.AddDefault(DoSwitch, DoSwitch);
		c_Prioritize.AddObject(DoScale, DoScale);
		c_Prioritize.AddObject(CrossLineOnly, CrossLineOnly);

		SmartDashboard::PutData("Auto Modes", &c_Mode);
		SmartDashboard::PutData("Priority", &c_Prioritize);

		gyro.Calibrate();
		d_setLeft.SetInverted(true);
		clawY.SetInverted(true);
		mClimb.SetInverted(true);

	}

	/*
	 * This autonomous (along with the chooser code above) shows how to
	 * select between different autonomous modes using the dashboard. The
	 * sendable chooser code works with the Java SmartDashboard. If you
	 * prefer the LabVIEW Dashboard, remove all of the chooser code and
	 * uncomment the GetString line to get the auto name from the text box
	 * below the Gyro.
	 *
	 * You can add additional auto modes by adding additional comparisons to
	 * the if-else structure below with additional strings. If using the
	 * SendableChooser make sure to add them to the chooser code above as
	 * well.
	 */

	void DropClaw() {
		RobotDrive.ArcadeDrive(1,0);
		RobotDrive.ArcadeDrive(0,0);

	}

	void Autonomous() {


		string autoSelected = c_Mode.GetSelected();
		// string autoSelected = SmartDashboard::GetString(
		// "Auto Selector", kAutoNameDefault);
		cout << "Auto selected: " << autoSelected << endl;

		string prioritizeSelected = c_Prioritize.GetSelected();
		cout << "Prioritize: " << prioritizeSelected << endl;

		// MotorSafety improves safety when motors are updated in loops
		// but is disabled here because motor updates are not looped in
		// this autonomous mode.
		RobotDrive.SetSafetyEnabled(false);

		string gameData = DriverStation::GetInstance().GetGameSpecificMessage();

//The block below is for assigning priorities to a numerical value...
		int priorityInt;

		if(prioritizeSelected == DoSwitch){
			priorityInt = 0;
		}
		else if(prioritizeSelected == DoScale){
			priorityInt = 1;
		}
		else if(prioritizeSelected == CrossLineOnly){
			priorityInt = 2;
		}
//END BLOCK

//The block below is for assigning game data entries to a numerical value...
		//Switch
		int switchSideInt;
		if(gameData[0] == 'L'){
			switchSideInt = 0;
		}
		else if(gameData[0] == 'R'){
			switchSideInt = 1;
		}

		//Scale
		int scaleSideInt;
		if(gameData[1] == 'L'){
			scaleSideInt = 0;
		}
		else if(gameData[1] == 'R'){
			scaleSideInt = 1;
		}
//END BLOCK

		if (autoSelected == autoOnCenter) {
			// Custom Auto goes here
			cout << "Running Center Auton..." << endl;

			switch (switchSideInt){
			case 0/*Left*/:
				//In front of the center station and going to left side of switch.
				break;
			case 1/*Right*/:
				//In front of the center station and going to right side of switch.
				break;
			default/*NO CODE ERROR*/:
				cout << "NO CODE GIVEN... DRIVING PAST AUTO LINE" << endl;
				while (gyro.GetAngle() != 45 || gyro.GetAngle() != -45) {
					RobotDrive.TankDrive(-0.5, 0.5);
				}
				break;
			}


		}
		else if (autoSelected == autoOnRight) {
			cout << "Running Right Auton..." << endl;

			gyro.Reset();

			switch(priorityInt) {

			case 1 :
				//Prioritize scale
				switch(scaleSideInt) {
				case 0 :
					//Scale is on left, checking switch...
					switch(switchSideInt) {
					case 1 :
						//Do switch
						DropClaw();
						RobotDrive.ArcadeDrive(-1, -gyro.GetAngle()*0.003);
						Wait(0.8);
						do {
						RobotDrive.ArcadeDrive(0, -0.33);
						}
						while (gyro.GetAngle() < 85);

						RobotDrive.ArcadeDrive(0.5, -gyro.GetAngle()*0.003);
						Wait(0.3);
						clawY.Set(0.75);
						break;
					default :
						//Cross line
						RobotDrive.ArcadeDrive(1, -gyro.GetAngle()*0.003);
						break;
					}
					break;
				case 1 :
					//Scale is on right, doing scale...



					break;
				}
				break;

			case 0 :
				//Prioritize switch
				switch(switchSideInt) {
				case 0 :
					//Switch is on left, checking scale...
					switch(scaleSideInt) {
					case 1 :
						//Scale is on right, doing scale
						break;
					default :
						//Cross line
						break;
					}
					break;

				case 1 :
					//Do switch
					break;
				}
				break;

			case 2 :
				//Crossing line.
				break;
			}

		}
		else if (autoSelected == autoOnLeft) {
			cout << "Running Left Auton..." << endl;

			gyro.Reset();

			switch(priorityInt){

			case 1 :
				switch(scaleSideInt){
				case 0 :
					//Do scale
					break;
				case 1 :
					switch(switchSideInt){
					case 0 :
						//Do switch
						break;
					default :
						//Cross line
						break;
					}
					break;
				}
				break;
			case 0 :
				switch(switchSideInt){
				case 0 :
						//Do switch
						break;
				case 1 :
					switch(scaleSideInt){
					case 0 :
						//Do scale
						break;
					default :
						//Cross line
						break;
						}
					break;
				}
				break;
			case 2 :
				//Cross line
				break;
			}
		}
		else if (autoSelected == autoIdle) {
			cout << "Doing Nothing in Auton..." << endl;
			gyro.Reset();

		}
	}

	/*
	 * Runs the motors with tank steering.
	 */
	void OperatorControl() override {
		RobotDrive.SetSafetyEnabled(true);
		while (IsOperatorControl() && IsEnabled()) {

			cout << stickAux.GetTop() << endl;

			// Drive
			RobotDrive.TankDrive(-stickDriveL.GetY()*0.75, -stickDriveR.GetY()*0.73);

			// Claw up/down
			if (stickAux.GetRawButton(3)) {
				clawY.Set(0.7);
			} else if (stickAux.GetRawButton(4)) {
				clawY.Set(-0.7);
			} else {
				clawY.Set(0);
			}

			//Cube-related code
			if (stickAux.GetRawButton(2)) {
				clawL.Set(1);
				clawR.Set(-1);

			} else if (stickAux.GetRawButton(1)) {
				clawL.Set(-0.8);
				clawR.Set(0.8);

			} else if (stickAux.GetY() > 0.2 || stickAux.GetY() < -0.2) {
				clawL.Set(stickAux.GetY()*0.25);
				clawR.Set(-stickAux.GetY()*0.25);

			} else if (stickAux.GetZ() > 0.2 || stickAux.GetZ() < -0.2){
				clawL.Set(stickAux.GetZ()*0.25);
				clawR.Set(stickAux.GetZ()*0.25);

			} else {
				clawL.Set(0);
				clawR.Set(0);
			}

			//Climber
			if (stickAux.GetRawButton(11)) {
				mClimb.Set(0.8);
			} else if (stickAux.GetRawButton(12)) {
				mClimb.Set(-0.8);
			} else {
				mClimb.Set(0);
			}

			// The motors will be updated every 5ms
			Wait(0.005);
		}
	}

	/*
	 * Runs during test mode
	 */
	void Test() override {}

private:
	//For switch/scale state
	string gameData;

	//Left Drive
	Spark driveFL{0};
	Spark driveRL{1};
	SpeedControllerGroup d_setLeft{driveRL, driveFL};

	//Right Drive
	Spark driveFR{2};
	Spark driveRR{3};
	SpeedControllerGroup d_setRight{driveRR, driveFR};

	DifferentialDrive RobotDrive{d_setLeft, d_setRight};

	//Claw motors
	Victor clawL{4};
	Victor clawR{5};
	Spark clawY{6};

	//Climing
	Spark mClimb{7};

	//Joysticks
	Joystick stickDriveL{0};
	Joystick stickDriveR{1};
	Joystick stickAux{2};

	//Gyro
	ADXRS450_Gyro gyro{SPI::Port::kOnboardCS0};

	//Limit Switches
	DigitalInput limit0{0};
	DigitalInput limit1{1};
	DigitalInput limit2{2};
	DigitalInput limit3{3};

	//Auton stuff
	SendableChooser<string> c_Prioritize;
	const string DoScale = "Scale";
	const string DoSwitch = "Switch";
	const string CrossLineOnly = "Cross Line Only";

	SendableChooser<string> c_Mode;
	const string autoIdle = "DoNothing";
	const string autoOnLeft = "StartInLeft";
	const string autoOnRight = "StartInRight";
	const string autoOnCenter = "StartInCenter";


};

START_ROBOT_CLASS(Robot)
