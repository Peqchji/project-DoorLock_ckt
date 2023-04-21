#include <Arduino.h>
#include <Scheduler.h>
#include <Task.h>
#include <LeanTask.h>


class IRNode_Task : public LeanTask {
    private:
        int current_status;
        bool is_active;
        int node_num;
        int (*sel_func)(int);
        unsigned int task_timer = 0;
        
    public:
        IRNode_Task() { }
        
        int init(int num, int (*sel_func_args)(int)) {
            this->node_num = num;
            this->is_active = false;
            this->current_status = LOW;
            this->sel_func = sel_func_args;
            return 0;
        }
        
        void reset_props(){
            this->is_active = false;
            this->current_status = LOW;
        }
        
    protected:
        void setup() {}
        
        void loop() {
            sel_func(node_num);
            Serial.println(this->current_status);
            
            
            this->current_status = digitalRead(D0);
            if(this->current_status == HIGH && !this->is_active){
                this->is_active = true;
                
            }
            
            if(this->is_active){
                return;
            }
        }
};