class IRNode_Task : public LeanTask {
    private:
        int node_num;
        IRNode IR;
        
    public:
        IRNode_Task(num, IR_class) {
            node_num = num
            IR = IR_class
        }
        
    protected:
      void setup() {}
      
      void loop() {
        IR.get_is_active();
      }
};