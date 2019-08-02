#pragma once

#include "stdafx.h"

// Encapsulates a request as an object, thereby letting us parameterize other objects with different requests, queue or log requests, and support undoable operations.

class Command
{
    public:
    virtual void execute()=0;
};

class Light
{
    public:
        void on()
        {
            cout<<"Light On" << endl;
        }
        void off()
        {
            cout << "Light Off" << endl;
        }
};

class LightOnCommand : public Command
{
public:
    LightOnCommand(Light *light) : mLight(light) {}
    void execute() {
        mLight->on();
    }
private:
    Light *mLight;
};

class LightOffCommand : public Command
{
public:
    LightOffCommand(Light *light) : mLight(light) {}
    void execute() {
        mLight->off();
    }
private:
    Light *mLight;
};

class RemoteControl
{
    public:
        void setCommand(Command* cmd)
        {
            mCmd =cmd;
        }
        void buttonPressed()
        {
            mCmd->execute();
        }
private:
Command * mCmd;
};

void commandPattern_Example()
{
    // Receiver 
    Light *light = new Light;

    // concrete Command objects 
    LightOnCommand *lightOn = new LightOnCommand(light);
    LightOffCommand *lightOff = new LightOffCommand(light);

    // invoker objects
    RemoteControl *control = new RemoteControl;

    // execute
    control->setCommand(lightOn);
    control->buttonPressed();
    control->setCommand(lightOff);
    control->buttonPressed();

    // good practice to delete all ptr
    // better to create unique_ptr
    delete light, lightOn, lightOff, control;
}