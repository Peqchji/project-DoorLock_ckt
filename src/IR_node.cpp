class IRNode {
    private:
        int current_status;
        bool is_active;
        short node_num;
        
    public:
        get_is_active();
        set_is_active();
}

IRNode::set_is_active() {
    if (this.current_status == HIGH) {
        this.is_active = true;
    }
}


IRNode::get_is_active() {
    return this.is_active;
}
