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

			LeftMotorSet.SetInverted(true);

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

		} else if (autoSelected == autoOnRight) {
			cout << "Running Right Auton..." << endl;

			gyro.Reset();
			gyro.Calibrate();

			gameData = DriverStation::GetInstance().GetGameSpecificMessage();

			switch(gameData[0]) {

			case "R" :
				//In front of right station and going to right side of switch.

				break;

			case "L" :
				//In front of right station and...
				switch(Prioritize) {
				case DoScale :
					if(gameData[1] == "R") {
						//...going to right side of scale.
					}
					else {
						//...crossing line only.
					}
					break;
				case DoSwitch :
					//...going to left side of switch.
					break;
				case CrossLineOnly :
					//...crossing line only.
					break;
				default :
					//...crossing line only.
					break;
				}
				break;

			default :
				cout << "NO CODE GIVEN... DRIVING PAST AUTO LINE" << endl;
				RobotDrive.TankDrive(0.5,0.5);
			}
		} else if (autoSelected == autoOnLeft) {
			cout << "Running Left Auton..." << endl;

			gyro.Reset();
			gyro.Calibrate();

			gameData = DriverStation::GetInstance().GetGameSpecificMessage();

			switch(gameData[0]) {

			case "R" :
				//In front of left station and...
				switch(Prioritize) {
				case DoScale :
					if(gameData[1] == "L") {
						//...going to left side of scale.
					}
					else {
						//...crossing line only.
					}
					break;
				case DoSwitch :
					//...going to right side of switch.
					break;
				case CrossLineOnly :
					//...crossing line only.
					break;
				default :
					//...crossing line only.
					break;
				}
				//going to right side of switch.
				break;


			case "L" :
				//In front of right station and going to left side of switch.
				break;

			default :
				cout << "NO CODE GIVEN... DRIVING PAST AUTO LINE" << endl;
				RobotDrive.TankDrive(0.5,0.5);
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
			LeftMotorSet.SetInverted(true);

			RobotDrive.ArcadeDrive(
					stickDrive.GetY(), stickDrive.GetX());

			// The motors will be updated every 5ms
			Wait(0.005);
		}
	}

	/*
	 * Runs during test mode
	 */
	void Test() override {}

private:
	// Robot drive system
	string gameData;

	Talon clawRL{0};
	Talon clawML{1};
	Talon clawRF{2};
	Talon clawRR{3};
	Talon clawRM{4};
	Talon clawRF{5};



	Talon driveRL{6};
	Talon driveFL{7};
	SpeedControllerGroup LeftMotorSet{driveRL, driveFL};


	Talon driveRR{8};
	Talon driveFR{9};
	SpeedControllerGroup RightMotorSet{driveRR, driveRL};

	DifferentialDrive RobotDrive{LeftMotorSet, RightMotorSet};



	Joystick stickDrive{0};
	Joystick stickAux{1};

	ADXRS450_Gyro gyro{0};

	SendableChooser<string> Prioritize;
	const string DoScale = "Scale";
	const string DoSwitch = "Switch";
	const string CrossLineOnly = "Cross Line Only";

	SendableChooser<string> m_chooser;
	const string autoIdle = "DoNothing";
	const string autoOnLeft = "StartInLeft";
	const string autoOnRight = "StartInRight";
	const string autoOnCenter = "StartInCenter";


};

START_ROBOT_CLASS(Robot)
