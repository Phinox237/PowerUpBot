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
		m_chooser.AddDefault(autoOnLeft, autoOnLeft);
		m_chooser.AddObject(autoOnRight, autoOnRight);
		m_chooser.AddObject(autoOnCenter, autoOnCenter);
		m_chooser.AddObject(autoIdle, autoIdle);
		SmartDashboard::PutData("Auto Modes", &m_chooser);

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
	void Autonomous() {

		string prioritizeSelected = Prioritize.GetSelected();
		cout << "Prioritize: " << prioritizeSelected << endl;

		string autoSelected = m_chooser.GetSelected();
		// string autoSelected = SmartDashboard::GetString(
		// "Auto Selector", kAutoNameDefault);
		cout << "Auto selected: " << autoSelected << endl;

		// MotorSafety improves safety when motors are updated in loops
		// but is disabled here because motor updates are not looped in
		// this autonomous mode.
		RobotDrive.SetSafetyEnabled(false);

		if (autoSelected == autoOnCenter) {
			// Custom Auto goes here
			cout << "Running Center Auton..." << endl;

			drive_setLeft.SetInverted(true);

			gameData = DriverStation::GetInstance().GetGameSpecificMessage();

			switch(gameData[0]) {

				case "R" :
					//In front of the center station and going to right side of switch.
				break;

				case "L" :
					//In front of the center station and going to left side of switch.
				break;

				default :
					cout << "NO CODE GIVEN... DRIVING PAST AUTO LINE" << endl;
					while(gyro.GetAngle() != 45 || gyro.GetAngle() != -45) {
						RobotDrive.TankDrive(-0.5,0.5);
				}
			}
		}
		else if (autoSelected == autoOnRight) {
			cout << "Running Right Auton..." << endl;

			gyro.Reset();
			gyro.Calibrate();

			gameData = DriverStation::GetInstance().GetGameSpecificMessage();

			switch(Prioritize) {

			case DoScale :
				switch(gameData[1]) {
				case "L" :
					switch(gameData[0]) {
					case "R" :
						//Do switch
						break;
					default :
						//Cross line
						break;
					}
					break;
				case "R" :
					//Do scale
					break;
				}
				break;

			case DoSwitch :
				switch(gameData[0]) {
				case "L" :
					switch(gameData[1]) {
					case "R" :
						//Do scale
						break;
					default :
						//Cross line
						break;
					}
					break;

				case "R" :
					//Do switch
					break;

				}
				break;

			case CrossLineOnly :
				//Crossing line.
				break;
			}

		} else if (autoSelected == autoOnLeft) {
			cout << "Running Left Auton..." << endl;

			gyro.Reset();
			gyro.Calibrate();

			gameData = DriverStation::GetInstance().GetGameSpecificMessage();

			switch(Prioritize){
			case DoScale :
				switch(gameData[1]){
				case "L" :
					//Do scale
					break;
				case "R" :
					switch(gameData[0]){
					case "L" :
						//Do switch
						break;
					case "R" :
						//Cross line
						break;
					}
					break;
				}
				break;
			case DoSwitch :
				switch(gameData[0]){
				case "L" :
					//Do switch
					break;
				case "R" :
				switch(gameData[1]){
				case "L" :
					//Do scale
					break;
				case "R" :
					//Cross line
					break;
				}
					break;
				default :
					//Cross line
					break;
				}
				break;
			case CrossLineOnly :
				//Cross line
				break;
			default :
				cout << "No code given, crossing line..." << endl;
				//Cross line
			}

			} else if (autoSelected == autoIdle) {
				cout << "Doing Nothing in Auton..." << endl;

				gyro.Reset();
				gyro.Calibrate();

		}
	}

	/*
	 * Runs the motors with arcade steering.
	 */
	void OperatorControl() override {
		RobotDrive.SetSafetyEnabled(true);
		while (IsOperatorControl() && IsEnabled()) {
			// Drive with arcade style (use right stick)
			drive_setLeft.SetInverted(true);

			RobotDrive.TankDrive(stickDriveL.GetY(), stickDriveR.GetY());

			clawY.Set(stickAux.GetY());

			if (stickAux.GetRawButton(1)) {
				drive_setLeft.Set(1);
				drive_setRight.Set(-1);
			} else {
				drive_setLeft.Set(0);
				drive_setRight.Set(0);
			}

			if (stickAux.GetRawButton(2)) {
				drive_setLeft.Set(-1);
				drive_setRight.Set(1);
			} else {
				drive_setLeft.Set(0);
				drive_setRight.Set(0);
			}

			if (stickAux.GetRawButton(12)) {
				mClimb.Set(1);
			} else {
				mClimb.Set(0);
			}

			if (stickAux.GetRawButton(11)) {
				mClimb.Set(-1);
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
	SpeedControllerGroup drive_setLeft{driveRL, driveFL};

	//Right Drive
	Spark driveFR{2};
	Spark driveRR{3};
	SpeedControllerGroup drive_setRight{driveRR, driveRL};

	DifferentialDrive RobotDrive{drive_setLeft, drive_setRight};

	//Claw motors
	Talon clawL{4};
	Talon clawR{5};
	Talon clawY{6};

	//Climing
	Talon mClimb{7};

	//Joysticks
	Joystick stickDriveL{0};
	Joystick stickDriveR{1};
	Joystick stickAux{2};

	ADXRS450_Gyro gyro{0};

	//Auton stuff
	SendableChooser<string> Prioritize;
	const string DoScale = "Scale";
	const string DoSwitch = "Switch";
	const string CrossLineOnly = "Cross Line Only";

	SendableChooser<string> Cube;
	const string GetCube = "Get a Power Cube";
	const string LeaveCube = "Do not get a Power Cube";

	SendableChooser<string> m_chooser;
	const string autoIdle = "DoNothing";
	const string autoOnLeft = "StartInLeft";
	const string autoOnRight = "StartInRight";
	const string autoOnCenter = "StartInCenter";


};

START_ROBOT_CLASS(Robot)
