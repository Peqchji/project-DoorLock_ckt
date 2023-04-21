#include <Arduino.h>
#include <Scheduler.h>
#include <Task.h>
#include <LeanTask.h>
#include "../header.hpp"

class IRNode_Task : public LeanTask {
    private:
        int current_status;
        bool is_active;
        int node_num;
        int (*sel_func)(int);
        
    public:
        IRNode_Task() { }
        
        int init(int num, int (*sel_func_args)(int)) {
            this->node_num = num;
            this->is_active = false;
            this->current_status = LOW;
            this->sel_func = sel_func_args;
            return 0;
        }
        
        bool get_is_active() {
            return this->is_active;
        };
        
        int set_is_active(){
            if (this->current_status == HIGH) {
                this->is_active = true;
            }
            return 0;
        };
        
    protected:
        void setup() {}
        
        void loop() {
            
            sel_func(node_num);
            Serial.println(this->current_status);
            
            if(this->is_active){
                return;
            }
            this->current_status = digitalRead(D0);
        }
};