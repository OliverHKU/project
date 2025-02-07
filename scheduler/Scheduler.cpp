// ============================================================================
// Algorithm Description:
// Implementation of the scheduling algorithm
//
// Update:
// Nov 29th 2011,
// Feb 24th 2012, Change the Scheduling algorithm due to New PE structure
// Mar 27th 2012, Pipeline the ALU operation
// May 29th 2012, Support 400MHz-16bit PE based CGRA
// July 17th 2012, Add IO model for accurate scheduling
// July 18th 2012, Add operation transmission history and reuse 
// data stored in intermediate PE
// Sep 9th 2014, Update coding style and remove useless functionality. 
//
// Author:
// Cheng Liu
// st.liucheng@gmail.com, liucheng@eee.hku.hk
// E.E.E department, The University of Hong Kong
//
// ============================================================================

#include "Scheduler.h"

Scheduler::Scheduler(Data_Flow_Graph * _DFG, Coarse_Grain_Recon_Arch* _CGRA){

    Load_Parameters();
    Init(_DFG, _CGRA);

}

// Estimate time that consumed by loading/storing data from/to 
// single port IO buffer to multiple IO FIFO.
// Input/output load/store time of a DFG might be different, 
// so we must keep a record of all of them to evaluate the 
// load-computting-store pipeling. 
void Scheduler::IO_LD_ST(){

    std::vector<std::vector<int> > Input_Layout;
    std::vector<std::vector<int> > Output_Layout;
    std::vector<std::vector<int> > OP_And_LD_Time;
    std::vector<std::vector<int> > OP_And_ST_Time;
    Get_Group_Info(Input_Layout, Output_Layout);
    Get_OP_LD_ST_Time(OP_And_LD_Time, OP_And_ST_Time);
    for(int DFG_ID=0; DFG_ID<DFG_Num; DFG_ID++){
       IO_Scheduling(DFG_ID, Input_Layout, Output_Layout, \
               OP_And_LD_Time, OP_And_ST_Time); 
    }
}

int Scheduler::Get_IO_Physical_Addr(const int &OP_ID, \
        const int &DFG_ID, \
        const std::vector<std::vector<int> > &Input_Layout, \
        const std::vector<std::vector<int> > &Output_Layout){
    if(OP_ID == 0){
        return -1; // OP 0 is not in IO Buffer.
    }
    else if(DFG->OP_Array[OP_ID]->Is_Input()){
        int Logic_Addr = DFG->Get_IO_Logic_Addr(OP_ID);
        return Input_Layout[Logic_Addr][DFG_ID];
    }
    else if(DFG->OP_Array[OP_ID]->Is_Output()){
        int Logic_Addr = DFG->Get_IO_Logic_Addr(OP_ID);
        return Output_Layout[Logic_Addr][DFG_ID];
    }
    else{
        ERROR("Unknown OP_ID in IO Buffer!");
    }
}

void Scheduler::IO_Scheduling(const int &DFG_ID, \
        const std::vector<std::vector<int> > &Input_Layout, \
        const std::vector<std::vector<int> > &Output_Layout, \
        const std::vector<std::vector<int> > &OP_And_LD_Time, \
        const std::vector<std::vector<int> > &OP_And_ST_Time){
    std::vector<std::vector<int> > Addr_And_LD_Time;
    std::vector<std::vector<int> > Addr_And_ST_Time;
    std::vector<std::vector<int> >::const_iterator cit;
    for(cit=OP_And_LD_Time.begin(); cit!=OP_And_LD_Time.end(); cit++){
        std::vector<int> tmp = *cit;
        int OP_Tmp = tmp[0];
        tmp[1] = Get_IO_Physical_Addr(OP_Tmp, DFG_ID, Input_Layout, Output_Layout);
        Addr_And_LD_Time.push_back(tmp);
    }

    for(cit=OP_And_ST_Time.begin(); cit!=OP_And_ST_Time.end(); cit++){
        std::vector<int> tmp = *cit;
        int OP_Tmp = tmp[0];
        tmp[1] = Get_IO_Physical_Addr(OP_Tmp, DFG_ID, Input_Layout, Output_Layout);
        Addr_And_ST_Time.push_back(tmp);
    }
    
    //FIFO_Req_Construction(LD_FIFO, Addr_And_LD_Time, FIFO_Num);
    //FIFO_Req_Construction(ST_FIFO, Addr_And_ST_Time, FIFO_Num);
    //Group_LD_Time[DFG_ID] = SPM_FIFO_Scheduling(LD_FIFO, SPM_Width);
    //Group_ST_Time[DFG_ID] = SPM_FIFO_Scheduling(ST_FIFO, SPM_Width);

}

void Scheduler::Get_OP_LD_ST_Time(std::vector<std::vector<int> > &OP_And_LD_Time, \
        std::vector<std::vector<int> > &OP_And_ST_Time){
    for(int i=1; i<DFG->OP_Num; i++){
        std::vector<int> tmp;
        if(DFG->OP_Array[i]->Is_Input()){
            tmp.push_back(i);
            tmp.push_back(DFG->OP_Array[i]->OP_Attribute.OP_LD_Time);
            OP_And_LD_Time.push_back(tmp);
        }
        else if(DFG->OP_Array[i]->Is_Output()){
            tmp.push_back(i-DFG->Input_OP_Num);
            tmp.push_back(DFG->OP_Array[i]->OP_Attribute.OP_ST_Time);
            OP_And_ST_Time.push_back(tmp);
        }
    }

    //VecComp MyComp(1);
    //std::stable_sort(OP_And_LD_Time.begin(), OP_And_LD_Time.end(), MyComp);
    //std::stable_sort(OP_And_ST_Time.begin(), OP_And_ST_Time.end(), MyComp);
}

void Scheduler::LD_ST_Schedule(const std::vector<std::vector<int> > &Input_Layout, \
        const std::vector<std::vector<int> > &Output_Layout, \
        const std::vector<int> &LD_Req_Seq, \
        const std::vector<int> &ST_Req_Seq){
    int In_SPM_Width = CGRA->In_SPM_Width;
    int Out_SPM_Width = CGRA->Out_SPM_Width;
    int In_FIFO_Num = CGRA->In_Buffer_Num;
    int Out_FIFO_Num = CGRA->Out_Buffer_Num;

    int Row = Input_Layout.size();
    int Col = Input_Layout[0].size();
    std::vector<std::vector<int> > LD_Addr_Req;
    LD_Addr_Req.resize(Row);
    for(int i=0; i<Row; i++){
        LD_Addr_Req[i].resize(Col);
    }

    for(int i=0; i<Row; i++){
        int OP_ID = LD_Req_Seq[i];
        for(int j=0; j<Col; j++){
           LD_Addr_Req[i][j] = Input_Layout[OP_ID][j];
        }
    }

    Get_Scheduling_Result(LD_Addr_Req, In_SPM_Width, In_FIFO_Num);
    Row = Output_Layout.size();
    Col = Output_Layout[0].size();
    std::vector<std::vector<int> > ST_Addr_Req;
    ST_Addr_Req.resize(Row);
    for(int i=0; i<Row; i++){
        ST_Addr_Req[i].resize(Col);
    }

    for(int i=0; i<Row; i++){
        int OP_ID = ST_Req_Seq[i];
        for(int j=0; j<Col; j++){
            ST_Addr_Req[i][j] = Output_Layout[OP_ID-1-Input_Layout.size()][j];
        }
    }

    Get_Scheduling_Result(ST_Addr_Req, Out_SPM_Width, Out_FIFO_Num);

}

void Scheduler::Get_Scheduling_Result(const std::vector<std::vector<int> > &Addr_Req, 
        const int &SPM_Width, const int &FIFO_Num){
    for(int i=0; i<DFG_Num; i++){
        //distribute each DFG input addr requests into input FIFOs.
        std::vector<std::vector<int> > FIFO_Req;
        FIFO_Req.resize(FIFO_Num);
        for(unsigned row=0; row<Addr_Req.size(); row++){
            unsigned FIFO_ID = row%FIFO_Num;
            FIFO_Req[FIFO_ID].push_back(Addr_Req[row][i]);
        }

        //Set the numbe of data that lies in the same line as priority.
        std::vector<int> SPM_Addr;
        Group_LD_Time[i] = 0;
        while(!Is_FIFO_Empty(FIFO_Req)){
            Group_LD_Time[i]++;
            int Tmp_Addr = Get_SPM_Addr(FIFO_Req, SPM_Width);
            SPM_Addr.push_back(Tmp_Addr);
            Update_FIFO_Content(FIFO_Req, Tmp_Addr, SPM_Width);
        }
    }

}

bool Scheduler::Is_FIFO_Empty(const std::vector<std::vector<int> > &FIFO_Req){
    std::vector<std::vector<int> >::const_iterator cit;
    bool All_Empty = true;
    for(cit=FIFO_Req.begin(); cit!=FIFO_Req.end(); cit++){
        All_Empty = All_Empty && cit->empty();
    }
    return All_Empty;
}

void Scheduler::Update_FIFO_Content(std::vector<std::vector<int> > &FIFO_Req, \
        const int &SPM_Addr, const int &SPM_Width){
    std::vector<std::vector<int> >::iterator it;
    int Cnt = 0;
    for(it=FIFO_Req.begin(); it!=FIFO_Req.end(); it++){
        if(!it->empty() && Cnt<SPM_Width){
            int Addr = (int)(it->front()/SPM_Width);
            if(Addr == SPM_Addr){
                it->erase(it->begin());
                Cnt++;
            }
        }
    }
}

int Scheduler::Get_SPM_Addr(const std::vector<std::vector<int> > &FIFO_Req, \
        const int &SPM_Width){
    int SPM_Addr;
    std::vector<std::vector<int> >::const_iterator cit1, cit2;
    std::vector<int> FIFO_Pri; 
    int Max_Pri = 0;
    for(cit1=FIFO_Req.begin(); cit1!=FIFO_Req.end(); cit1++){
        int Pri = 0;
        int m1 = 0;
        int m2 = 0;
        for(cit2=FIFO_Req.begin(); cit2!=FIFO_Req.end(); cit2++){
            if(cit1!=cit2){
                if(!cit1->empty()){
                    m1 = (int)(cit1->front()/SPM_Width);
                } 
                if(cit2->empty()){
                    m2 = (int)(cit2->front()/SPM_Width);
                }

                if(m1==m2){
                    Pri++;
                }
            }
        }
        if(Pri > Max_Pri){
            Max_Pri = Pri;
            SPM_Addr = m1;
        }
    }
    return SPM_Addr;
}


// 0 will not be stored in IO buffer, but it is an OP of DFG.
void Scheduler::Get_Group_Info(std::vector<std::vector<int> > &Input_Layout, \
        std::vector<std::vector<int> > &Output_Layout){
    std::string Group_Info_fName = "./config/block-io.txt";
    std::vector<std::vector<int> > Data_Array;
    Read_2D_int_File(Group_Info_fName, Data_Array);

    int Row = Data_Array.size();
    int Col = Data_Array[0].size();
    DFG_Num = Col - 1;
    for(int i=0; i<Row; i++){
        std::vector<int> Tmp;
        if(DFG->OP_Array[i]->Is_Input()){
            for(int j=1; j<Col; j++){
                Tmp.push_back(Data_Array[i][j]-1);
            }
            Input_Layout.push_back(Tmp);
        }
        else if(DFG->OP_Array[i]->Is_Output()){
            for(int j=1; j<Col; j++){
                Tmp.push_back(Data_Array[i][j]);
            }
            Output_Layout.push_back(Tmp);
        }
        else{
            ERROR("Unexpected IO OP!");
        }
    }

}

void Scheduler::Read_2D_int_File(const std::string &fName, \
        std::vector<std::vector<int> > &Data){
    std::ifstream fHandle(fName.c_str());
    if(!fHandle.is_open()){
        ERROR("Failed to open %s!", fName.c_str());
    }
    std::string Line;
    while(std::getline(fHandle, Line)){
        std::istringstream iss(Line);
        Data.push_back(
                std::vector<int>(std::istream_iterator<int>(iss),
                    std::istream_iterator<int>()));
    }
    fHandle.close();
}

void Scheduler::Load_Parameters(){

    std::string Config_fName = "./config/configure.txt";
    std::ifstream Config_fHandle(Config_fName.c_str());
    if(!Config_fHandle.is_open()){
        ERROR("Failed to open configure.txt!");
    }

    while(!Config_fHandle.eof()){
        std::string Config_Item_Key;
        Config_fHandle >> Config_Item_Key;
        if(Config_Item_Key == "IO_Placement_Scheme"){
            std::string Config_Item_Val;
            Config_fHandle >> Config_Item_Val;
            if(Config_Item_Val == "Sequential_Placement"){
                IO_Placement_Scheme = Sequential_Placement;
            }
            else if(Config_Item_Val == "Interleaving_Placement"){
                IO_Placement_Scheme = Interleaving_Placement;
            }
            else if(Config_Item_Val == "Pre_Placement"){
                IO_Placement_Scheme = Pre_Placement;
            }
            else if(Config_Item_Val == "On_Demand_Placement"){
                IO_Placement_Scheme = On_Demand_Placement;
            }
            else{
                ERROR("Unknown IO Placement Scheme!\n");
            }
        }
        else if(Config_Item_Key == "List_Scheduling_Strategy"){
            std::string Config_Item_Val;
            Config_fHandle >> Config_Item_Val;
            if(Config_Item_Val == "PE_Pref"){
                List_Scheduling_Strategy = PE_Pref;
            }
            else if(Config_Item_Val == "OP_Pref"){
                List_Scheduling_Strategy = OP_Pref;
            }
            else if(Config_Item_Val == "PE_OP_Together"){
                List_Scheduling_Strategy = PE_OP_Together;
            }
            else{
                ERROR("Unknown scheduling strategy!\n");
            }
        }
        else if(Config_Item_Key == "Load_Balance_Factor"){
            Config_fHandle >> Load_Balance_Factor;
        }
        else if(Config_Item_Key == "PE_Sel_Strategy"){
            std::string Config_Item_Val;
            Config_fHandle >> Config_Item_Val;
            if(Config_Item_Val == "Least_Recent_Used"){
                PE_Sel_Strategy = Least_Recent_Used;
            }
            else if(Config_Item_Val == "Least_Ready_OP_Attached"){
                PE_Sel_Strategy = Least_Ready_OP_Attached;
            }
        }
    }

}

void Scheduler::Init(Data_Flow_Graph* _DFG, Coarse_Grain_Recon_Arch* _CGRA){
    
    DFG = _DFG;
    CGRA = _CGRA;
    Scheduling_Complete_Time = 0;

    // Resize the PE Component Trace
    float Assumed_OPC_Per_PE = 0.2;
    int Max_Sim_Cnt = (int) (DFG->OP_Num/(CGRA->CGRA_Scale*Assumed_OPC_Per_PE));
    int Resize_Len = CGRA->PE_Array[0]->Resize_Len;
    if(Max_Sim_Cnt < Resize_Len){
        Max_Sim_Cnt = Resize_Len;
    }

    for(int i=0; i<CGRA->CGRA_Scale; i++){
        CGRA->PE_Array[i]->Component_Trace.resize(Max_Sim_Cnt);
        for(int j=0; j<Max_Sim_Cnt; j++){
            CGRA->PE_Array[i]->Component_Trace[j] = new Component_State();
        }
    }

}


int Scheduler::Get_IO_Attached_PE_ID(const int &OP_ID){
    
    int Buffer_Slice_ID;
    int Buffer_Num;
    int Buffer_Used_Depth;
    if(DFG->OP_Array[OP_ID]->Is_Input()){
        Buffer_Num = CGRA->In_Buffer_Num;
        Buffer_Used_Depth = (DFG->Input_OP_Num-1)/Buffer_Num; 
        if((DFG->Input_OP_Num-1)%Buffer_Num != 0){
            Buffer_Used_Depth++;
        }
    }
    else if(DFG->OP_Array[OP_ID]->Is_Output()){
        Buffer_Num = CGRA->Out_Buffer_Num;
        Buffer_Used_Depth = DFG->Output_OP_Num/Buffer_Num;
        if(DFG->Output_OP_Num%Buffer_Num != 0){
            Buffer_Used_Depth++;
        }
    }
    else{
        ERROR("It not an IO operand!");
    }

    if(IO_Placement_Scheme == Sequential_Placement){
        Buffer_Slice_ID = (int) (DFG->Get_IO_Logic_Addr(OP_ID)/Buffer_Used_Depth);
        if(DFG->OP_Array[OP_ID]->Is_Input()){
            return CGRA->Load_PE_ID[Buffer_Slice_ID];
        }
        else{
            return CGRA->Store_PE_ID[Buffer_Slice_ID];
        }
    }
    else if(IO_Placement_Scheme == Interleaving_Placement){
        Buffer_Slice_ID = (DFG->Get_IO_Logic_Addr(OP_ID))%Buffer_Num;
        if(DFG->OP_Array[OP_ID]->Is_Input()){
            return CGRA->Load_PE_ID[Buffer_Slice_ID];
        }
        else{
            return CGRA->Store_PE_ID[Buffer_Slice_ID];
        }
    }
    else if(IO_Placement_Scheme == On_Demand_Placement){
        return DFG->OP_Array[OP_ID]->Initially_Attached_PE_ID;
    }
    else if(IO_Placement_Scheme == Pre_Placement){
        Buffer_Slice_ID = (int) (DFG->Get_IO_Logic_Addr(OP_ID)/Buffer_Used_Depth);
        if(DFG->OP_Array[OP_ID]->Is_Input()){
            return CGRA->Load_PE_ID[Buffer_Slice_ID];
        }
        else{
            return CGRA->Store_PE_ID[Buffer_Slice_ID];
        }
    }
    else{
        ERROR("Undefined IO buffer partition\n");
    }

}

/*****************************************************
 * This function decides the initial placement of DFG I/O
 * There are three placement strategies are provided.
 * 1) Place the DFG input/output sequentially
 * 2) Place the DFG input/output interleavingly 
 * 3) Assume all the DFG input/output are available, 
 * however, they can only be loaded once. Then let the 
 * operation scheduling decide which I/O should be loaded or 
 * stored. Finally, the scheduling result is used as an initial 
 * placement. 
 *****************************************************/

void Scheduler::IO_Placing(){
    
    /*-----------------------------------------------------
     * It adopts a block partition scheme on IO buffer or
     * a cyclic partition on IO buffer
     *-----------------------------------------------------*/
    if(IO_Placement_Scheme == Sequential_Placement || \
            IO_Placement_Scheme == Interleaving_Placement){
        std::vector<Operand*>::iterator Vit;
        for(Vit = DFG->OP_Array.begin(); Vit != DFG->OP_Array.end(); Vit++){
            if(((*Vit)->Is_Input()) && ((*Vit)->OP_ID !=0)){
                (*Vit)->OP_Attribute.OP_Cost = 0;
                (*Vit)->OP_Attribute.Exe_PE_ID = Get_IO_Attached_PE_ID((*Vit)->OP_ID);
                (*Vit)->OP_Attribute.OP_Avail_Time = INT_MAX;
                (*Vit)->OP_Attribute.OP_State = In_IO_Buffer;
            }
        }
    }
    else if(IO_Placement_Scheme == On_Demand_Placement){
        std::vector<Operand*>::iterator Vit;
        for(Vit = DFG->OP_Array.begin(); Vit != DFG->OP_Array.end(); Vit++){
            if(((*Vit)->Is_Input()) && ((*Vit)->OP_ID !=0)){
                (*Vit)->OP_Attribute.OP_Cost = 0;
                (*Vit)->OP_Attribute.Exe_PE_ID = NaN;
                (*Vit)->OP_Attribute.OP_Avail_Time = INT_MAX;
                (*Vit)->OP_Attribute.OP_State = In_IO_Buffer;
            }
        }
    }
    else if(IO_Placement_Scheme == Pre_Placement){
        /*Fir test only*/
        int tap = 32;
        int in_len = 256;
        int start_id = in_len+tap;
        int end_id = in_len+tap+tap;
        std::vector<Operand*>::iterator Vit;
        for(Vit = DFG->OP_Array.begin(); Vit != DFG->OP_Array.end(); Vit++){
            //if(((*Vit)->Is_Input()) && ((*Vit)->OP_ID !=0)){
            if(((*Vit)->Is_Input()) && ((*Vit)->OP_ID >=start_id && (*Vit)->OP_ID <=end_id)){
                (*Vit)->OP_Attribute.OP_Cost = 0;
                (*Vit)->OP_Attribute.Exe_PE_ID = 0;
                (*Vit)->OP_Attribute.OP_Avail_Time = 0;
                (*Vit)->OP_Attribute.OP_State = Avail;
                Attach_History Attach_Point;
                Attach_Point.Attached_PE_ID = ((*Vit)->OP_ID)%CGRA->CGRA_Scale;
                Attach_Point.Attached_Time = 0;
                (*Vit)->OP_Attach_History.push_back(Attach_Point);
            }
        }
    }
    else {
        ERROR("Unknown IO placement!\n");
    }

}

void Scheduler::Scheduling(){

    time_t Start_Time, End_Time;

    if(GL_Var::Impl_Or_Sim == 1){
        Start_Time = clock();
    }
    std::cout << "Operation scheduling starts!" << std::endl;

    IO_Placing();
    if(List_Scheduling_Strategy == PE_Pref){
        List_Scheduling_PE_Pref();
    }
    else if(List_Scheduling_Strategy == OP_Pref){
        List_Scheduling_OP_Pref();
    }
    else if(List_Scheduling_Strategy == PE_OP_Together){
        List_Scheduling_PE_OP_Together();
    }
    else{
        ERROR("Unknown scheduling strategy!\n");
    }


    std::cout << "Operation scheduling is completed!" << std::endl;
    std::cout << "Kernel execution time " << Scheduling_Complete_Time \
        << " cycles" << std::endl;
    GL_Var::fTrace << "Sim_Perf " << Scheduling_Complete_Time << std::endl;

    //IO_LD_ST();
    if(GL_Var::Impl_Or_Sim == 1){
        std::cout << "Start to dump the scheduling result for hardware implementation!" \
            << std::endl;
        Scheduling_Stat();
        Computation_Result_Dump();
        IO_Buffer_Dump_Coe();
        Data_Mem_Analysis();
        Inst_Mem_Dump_Coe();
        Inst_Mem_Dump_Mem();
        Addr_Buffer_Dump_Coe();
        Addr_Buffer_Dump_Mem();
        std::cout << std::endl << "Scheduling result is dumpped! " << std::endl;
        End_Time = clock();
        std::cout << "Total compilation time: " \
            << (double)(1.0*(End_Time-Start_Time)/CLOCKS_PER_SEC) \
            << " seconds." << std::endl;
    }

}

void Scheduler::List_Scheduling_PE_OP_Together(){}

void Scheduler::Load_Balance_Filter(std::list<int> &Candidates){

    int Min_Exe_OP_Num = DFG->OP_Num;
    int Max_Exe_OP_Num = 0;
    std::list<int>::iterator Lit;
    for(Lit = Candidates.begin(); Lit != Candidates.end(); Lit++){
        if(Min_Exe_OP_Num > CGRA->PE_Array[*Lit]->Exe_OP_Num){
            Min_Exe_OP_Num = CGRA->PE_Array[*Lit]->Exe_OP_Num;
        }
        if(Max_Exe_OP_Num < CGRA->PE_Array[*Lit]->Exe_OP_Num){
            Max_Exe_OP_Num = CGRA->PE_Array[*Lit]->Exe_OP_Num;
        }
    }

    int Threshold_Num = Min_Exe_OP_Num + \
                        (Max_Exe_OP_Num - Min_Exe_OP_Num) * \
                        Load_Balance_Factor;

    for(Lit = Candidates.begin(); Lit != Candidates.end(); ){
        if(CGRA->PE_Array[*Lit]->Exe_OP_Num > 1.3*Min_Exe_OP_Num && \
                CGRA->PE_Array[*Lit]->Exe_OP_Num > Threshold_Num){
            Lit = Candidates.erase(Lit);
        }
        else{
            Lit++;
        }
    }

}

/* Try to remove the PEs that is recetly used from candidate list, because
 * they are more likely to be busy. */
void Scheduler::Recent_Busy_Filter(std::list<int> &Candidates){

    int Min_Active_Time = INT_MAX;
    int Max_Active_Time = 0;

    std::list<int>::iterator Lit;
    for(Lit = Candidates.begin(); Lit != Candidates.end(); Lit++){
        int Current_Active_Time = CGRA->PE_Array[*Lit]->Max_Active_Time;
        if(Current_Active_Time < Min_Active_Time){
            Min_Active_Time = Current_Active_Time;
        }
        if(Current_Active_Time > Max_Active_Time){
            Max_Active_Time = Current_Active_Time;
        }
    }

    int Threshold_Time = Min_Active_Time + Load_Balance_Factor * (Max_Active_Time - Min_Active_Time);
    for(Lit = Candidates.begin(); Lit != Candidates.end();){
        int Current_Active_Time = CGRA->PE_Array[*Lit]->Max_Active_Time;
        if(Current_Active_Time <= Threshold_Time){
            Lit++;
        }
        else{
            Lit = Candidates.erase(Lit);
        }
    }

}

int Scheduler::Least_Ready_OP_Attached_Sel(const std::list<int> &Candidates, const std::list<int> &OP_Ready_Set){

    int Sel_PE_ID;
    std::vector<int> Attached_Ready_OP_Num;
    Attached_Ready_OP_Num.resize(CGRA->CGRA_Scale);
    for(int i=0; i<CGRA->CGRA_Scale; i++){
        Attached_Ready_OP_Num[i] = 0;
    }

    std::list<int>::const_iterator Lcit;
    std::vector<Operand*>::iterator Vit;
    std::list<Attach_History>::iterator Lit;
    for(Lcit = OP_Ready_Set.begin(); Lcit != OP_Ready_Set.end(); Lcit++){
        for(Vit = DFG->OP_Array[*Lcit]->OP_Parents.begin(); Vit != DFG->OP_Array[*Lcit]->OP_Parents.end(); Vit++){
            for(Lit = DFG->OP_Array[(*Vit)->OP_ID]->OP_Attach_History.begin(); Lit != DFG->OP_Array[(*Vit)->OP_ID]->OP_Attach_History.end(); Lit++){
                Attached_Ready_OP_Num[Lit->Attached_PE_ID]++;
            }
        }
    }

    int Min_Attached_OP_Num = INT_MAX;
    for(Lcit = Candidates.begin(); Lcit != Candidates.end(); Lcit++){
        if(Attached_Ready_OP_Num[*Lcit] < Min_Attached_OP_Num){
            Sel_PE_ID = *Lcit;
            Min_Attached_OP_Num = Attached_Ready_OP_Num[*Lcit];
        }
    }

    return Sel_PE_ID;

}

void Scheduler::List_Scheduling_PE_Pref(){

    bool Scheduling_Completed = false;
    std::list<int> OP_Ready_Set;
    OP_Ready_Set_Init(OP_Ready_Set);
    GL_Var::fTrace << "OP_Ready_List ";

    while(!Scheduling_Completed){
        //Dump ready list info.
        GL_Var::fTrace << OP_Ready_Set.size() << " ";

        int Sel_PE_ID;
        int Sel_OP_ID;

        // Select a PE for the next operation
        std::list<int> Candidates;
        for(int i=0; i<CGRA->CGRA_Scale; i++){
            Candidates.push_back(i);
        }
        Load_Balance_Filter(Candidates);
        Recent_Busy_Filter(Candidates);
        if(PE_Sel_Strategy == Least_Recent_Used){
            Sel_PE_ID = Least_Recent_Used_Sel(Candidates);
        }
        else if(PE_Sel_Strategy == Least_Ready_OP_Attached){
            Sel_PE_ID = Least_Ready_OP_Attached_Sel(Candidates, OP_Ready_Set);
        }
        else{
            ERROR("Unknown PE selection strategy!\n");
        }
        CGRA->PE_Array[Sel_PE_ID]->Exe_OP_Num++;

        //Select an operation that can be executed on the pre-selected PE.
        Sel_OP_ID = Least_Cost_OP_Sel(Sel_PE_ID, OP_Ready_Set);

        //Operation Execution
        std::vector<Operand*>::iterator Vit;
        std::vector<int> Src_OP_IDs;
        std::vector<int> Src_OP_Arrival_Time;
        Src_OP_IDs.resize(3);
        Src_OP_Arrival_Time.resize(3);
        Operand* OP_Ptr = DFG->OP_Array[Sel_OP_ID];

        int i=0;
        for(Vit = OP_Ptr->OP_Parents.begin(); Vit != OP_Ptr->OP_Parents.end(); Vit++){
            Src_OP_IDs[i] = (*Vit)->OP_ID;
            Src_OP_Arrival_Time[i] = Fetch_OP(Src_OP_IDs[i], Sel_PE_ID, Impl);
            i++;
        }
        OP_Exe(Sel_OP_ID, Src_OP_IDs, Sel_PE_ID, Src_OP_Arrival_Time, Impl);

        //output operation that has been executed should be moved to output PE
        if(DFG->OP_Array[Sel_OP_ID]->OP_Type == OUTVAR || \
                DFG->OP_Array[Sel_OP_ID]->OP_Type == IMOUT){
            Store_In_IO_Buffer(Sel_OP_ID);
        }

        //Update the operation that is ready for execution
        OP_Ready_Set_Update(OP_Ready_Set, Sel_OP_ID);

        Scheduling_Completed = Is_Scheduling_Completed();
        Resize_Trace_Vector();

    }
    GL_Var::fTrace << std::endl;

    //Add an idle cycle in the end
    Scheduling_Complete_Time++ ;

}

void Scheduler::Resize_Trace_Vector(){
    int Len = CGRA->PE_Array[0]->Resize_Len;
    for(int i=0; i<CGRA->CGRA_Scale; i++){
        int Active_Time = CGRA->PE_Array[i]->Max_Active_Time;
        CGRA->PE_Array[i]->Trace_Size_Test(Active_Time+Len);
    }
}

void Scheduler::OP_Ready_Set_Update(std::list<int> &OP_Ready_Set, const int &Sel_OP_ID){

    std::list<int>::iterator lit;
    for(lit=OP_Ready_Set.begin(); lit!=OP_Ready_Set.end(); lit++){
        if((*lit) == Sel_OP_ID){
            OP_Ready_Set.erase(lit);
            break;
        }
    }

    std::vector<Operand*>::iterator Vit;
    for(Vit=DFG->OP_Array[Sel_OP_ID]->OP_Children.begin(); \
            Vit!=DFG->OP_Array[Sel_OP_ID]->OP_Children.end(); \
            Vit++){
        bool All_Src_Ready = true;
        bool Is_In_Ready_List = false;
        std::vector<Operand*>::iterator it;
        for(it=(*Vit)->OP_Parents.begin(); it!=(*Vit)->OP_Parents.end(); it++){
            std::list<int>::iterator OP_It;
            for(OP_It=OP_Ready_Set.begin(); OP_It!=OP_Ready_Set.end(); OP_It++){
                if(*OP_It==(*Vit)->OP_ID){
                    Is_In_Ready_List = true;
                    break;
                }
            }
            if((*it)->OP_Attribute.OP_State == Unavail){
                All_Src_Ready = false;
                break;
            }
        }

        bool Not_Executed = DFG->OP_Array[(*Vit)->OP_ID]->OP_Attribute.OP_State==Unavail;
        if(All_Src_Ready && Not_Executed && !Is_In_Ready_List){
            OP_Ready_Set.push_back((*Vit)->OP_ID);
        }
    }

}

void Scheduler::PE_OP_Sel(int &Sel_PE_ID, int &Sel_OP_ID, const std::list<int> &Candidates, const std::list<int> &OP_Ready_Set){}

/*
void Scheduler::PEOPPairSelection(int &selected_PE_id, int &selected_op_id, const list<int> &Candidates, const list<int> &OP_Ready_Set){

    int min_execution_cost=GL_Var::maximum_simulation_time;
    list<int>::const_iterator PE_it;
    for(PE_it=Candidates.begin(); PE_it!=Candidates.end(); PE_it++){
        list<int>::const_iterator op_it;
        int PE_free_time=CGRA->PE_Array[*PE_it]->maximum_active_time;
        for(op_it=OP_Ready_Set.begin(); op_it!=OP_Ready_Set.end(); op_it++){
            float op_cost=0;
            int dst_op=*op_it;
            vector<Vertex*>::iterator vit;
            for(vit=DFG->OP_Array[dst_op]->parents.begin(); vit!=DFG->OP_Array[dst_op]->parents.end(); vit++){
                int src_op=(*vit)->vertex_id;
                //int time_cost=FetchOP(src_op,selected_PE_id,Simulation);
                int attached_PE_id;
                int ready_time;
                if(DFG->OP_Array[src_op]->vertex_attribute.vertex_state==DataAvail){
                    attached_PE_id=NearestAttachedPE(src_op, selected_PE_id, ready_time);
                }
                else{
                    attached_PE_id=GL_Var::load_PE_id;
                    ready_time=CGRA->PE_Array[attached_PE_id]->maximum_active_time;
                }
                int dist_cost=CGRA->PE_pair_distance[attached_PE_id][*PE_it];

                //Expand the distance
                if(dist_cost>1){
                    dist_cost=dist_cost*2;
                }

                if(PE_free_time>ready_time+100){
                    op_cost+=dist_cost;
                }
                else if(PE_free_time>ready_time+50){
                    op_cost+=dist_cost*2;
                }
                else{
                    op_cost+=dist_cost*4;
                }
            }
            //cout<<"PE="<<*PE_it<<" OP="<<dst_op<<" cost="<<op_cost<<endl;
            if(op_cost<min_execution_cost){
                min_execution_cost=op_cost;
                selected_op_id=dst_op;
                selected_PE_id=*PE_it;
            }
        }
    }

}
*/


int Scheduler::Least_Cost_OP_Sel(const int &Sel_PE_ID, \
        const std::list<int> &OP_Ready_Set){
    int Sel_OP_ID = NaN;
    std::list<int>::const_iterator Lcit;
    int Min_Exe_Cost = INT_MAX; 
    int PE_Active_Time = CGRA->PE_Array[Sel_PE_ID]->Max_Active_Time;

    for(Lcit = OP_Ready_Set.begin(); Lcit != OP_Ready_Set.end(); Lcit++){
        float OP_Exe_Cost = 0;
        std::vector<Operand*>::iterator Vit;
        for(Vit = DFG->OP_Array[*Lcit]->OP_Parents.begin(); \
                Vit != DFG->OP_Array[*Lcit]->OP_Parents.end(); \
                Vit++){
            int Src_OP_ID = (*Vit)->OP_ID;
            int Attached_PE_ID;
            int Src_Ready_Time;
            if(DFG->OP_Array[Src_OP_ID]->OP_Attribute.OP_State == Avail){
                Attached_PE_ID = Nearest_Attached_PE(Src_OP_ID, Sel_PE_ID, Src_Ready_Time);
            }
            else{
                Attached_PE_ID = Get_IO_Attached_PE_ID(Src_OP_ID);
                if(Attached_PE_ID == NaN){
                    Attached_PE_ID = CGRA->Get_Min_Load_PE(Sel_PE_ID);
                }
                Src_Ready_Time = CGRA->PE_Array[Attached_PE_ID]->Max_Active_Time;
            }

            int Physical_Dist = CGRA->Get_Dist(Attached_PE_ID, Sel_PE_ID);

            OP_Exe_Cost += DFG->OP_Array[Src_OP_ID]->OP_Attribute.OP_Cost;
            if(Physical_Dist > 1){
                OP_Exe_Cost += Physical_Dist * 2;
            }

            if(PE_Active_Time > (Src_Ready_Time + 100)){
                OP_Exe_Cost += Physical_Dist;
            }
            else if(PE_Active_Time > (Src_Ready_Time + 50)){
                OP_Exe_Cost += Physical_Dist * 1.5;
            }
            else{
                OP_Exe_Cost += Physical_Dist * 2;
            }

        }

        if(OP_Exe_Cost < Min_Exe_Cost){
            Min_Exe_Cost = OP_Exe_Cost;
            Sel_OP_ID = *Lcit;
        }
    }

    if(Sel_OP_ID == NaN){
        ERROR("Unexpected operation selection!");
    }

    return Sel_OP_ID;

}

void Scheduler::OP_Ready_Set_Init(std::list<int> &OP_Ready_Set){

    for(int i=0; i<DFG->OP_Num; i++){
        Operand* OP_Ptr = DFG->OP_Array[i];
        if(!OP_Ptr->Is_Input()){
            std::vector<Operand*>::iterator OP_It;
            bool Is_Src_OP_Ready = true;
            for(OP_It = OP_Ptr->OP_Parents.begin(); \
                    OP_It != OP_Ptr->OP_Parents.end(); \
                    OP_It++){
                if((*OP_It)->OP_Attribute.OP_State == Unavail){
                    Is_Src_OP_Ready = false;
                    break;
                }
            }

            if(Is_Src_OP_Ready){
                OP_Ready_Set.push_back(i);
            }
        }
    }

}

int Scheduler::Fetch_OP(const int &Src_OP_ID, const int &Target_PE_ID, \
        const Exe_Mode &Mode){
    int Expected_Complete_Time = 0;
    int Src_Avail_Time;
    int Src_Attached_PE_ID;

    // Const 0 is always available on each PE
    if(Src_OP_ID == 0){
        DFG->OP_Array[0]->OP_Attribute.OP_State = Avail;
        DFG->OP_Array[0]->OP_Attribute.OP_LD_Time = 0;
        Src_Attached_PE_ID = Target_PE_ID;
        Src_Avail_Time = 0;
    }
    else if(Src_OP_ID!=0 && DFG->OP_Array[Src_OP_ID]->OP_Attribute.OP_State == Avail){
        Src_Attached_PE_ID = Nearest_Attached_PE(Src_OP_ID, Target_PE_ID, Src_Avail_Time);
    }
    else if(DFG->OP_Array[Src_OP_ID]->OP_Attribute.OP_State == In_IO_Buffer){
        Src_Attached_PE_ID = Get_IO_Attached_PE_ID(Src_OP_ID);
        if(Src_Attached_PE_ID == NaN){
            Src_Attached_PE_ID = CGRA->Get_Min_Load_PE(Target_PE_ID);
            DFG->OP_Array[Src_OP_ID]->Reset_Init_PE_ID(Src_Attached_PE_ID);
        }
        Src_Avail_Time = Load_From_IO_Buffer(Src_OP_ID, Mode);
    }
    else{
        ERROR("Unexpected operation fectching!");
    }

    // Move source operand to target PE.
    if(Src_Attached_PE_ID != Target_PE_ID){
        std::list<int> Routing_Path;
        CGRA->Dynamic_Routing(CGRA->Dynamic_Routing_Alg, \
                Src_Avail_Time, Src_Attached_PE_ID, \
                Target_PE_ID, Routing_Path);

        Expected_Complete_Time = OP_Migration(Src_Avail_Time, \
                Src_OP_ID, Routing_Path, Mode);
    }
    else{
        Expected_Complete_Time = Src_Avail_Time;
    }

    return Expected_Complete_Time;

}

void Scheduler::List_Scheduling_OP_Pref(){}
/*
void Scheduler::List_Scheduling_OP_Pref(){

    bool scheduling_completed=false;
    InputOperationScheduling();

    while(!scheduling_completed){
        int selected_OP_ID;
        //selected_OP_ID=StaticOperationSelection();
        selected_OP_ID=DynamicOperationSelection();

        int selected_PE_id;
        vector<int> Src_OP_IDs;
        vector<int> source_ready_time; //The time that source operands arrive at target PE data memory
        vector<int> source_Start_Time; //The time that source operands start to transmit
        vector<list<int> > source_routing_paths; //Rotuing paths for moving source operands to target PE
        Src_OP_IDs.resize(INST_OP_NUM-1);
        source_ready_time.resize(INST_OP_NUM-1);
        source_Start_Time.resize(INST_OP_NUM-1);
        source_routing_paths.resize(INST_OP_NUM-1);

        bool input_operation_flag=DFG->OP_Array[selected_OP_ID]->vertex_type==InputData;
        bool output_operation_flag=DFG->OP_Array[selected_OP_ID]->vertex_type==OutputData;
        bool in_out_mem_flag=DFG->OP_Array[selected_OP_ID]->vertex_attribute.vertex_state==DataInOutMem;

        //Input operation that is still in out memory will be loaded first
        if(input_operation_flag && in_out_mem_flag){
            LoadDataFromOutMem(selected_OP_ID, Implementation);
        }
        //output operation and intermediate operation will be executed
        else if(!input_operation_flag){
            vector<Vertex*>::iterator it;
            Vertex* selected_vertex=DFG->OP_Array[selected_OP_ID];
            for(it=selected_vertex->parents.begin(); it!=selected_vertex->parents.end(); it++){
                Src_OP_IDs.push_back((*it)->vertex_id);
            }

            selected_PE_id=PESelection(selected_OP_ID, Src_OP_IDs, source_routing_paths, source_Start_Time);

            //tmp_op=CGRA->PE_Array[1]->Component_Trace[42]->Component_Activity->Data_Mem_Port_OP[0];
            //if(tmp_op==154){
            //cout<<"checkpoint 1: right!"<<endl;
            //}
            //else{
            //cout<<"checkpoint 1: wrong! tmp_op="<<tmp_op<<endl;
            //}

            FetchSourceOperation(selected_PE_id, Src_OP_IDs, source_routing_paths, source_Start_Time, source_ready_time);

            //tmp_op=CGRA->PE_Array[1]->Component_Trace[42]->Component_Activity->Data_Mem_Port_OP[0];
            //if(tmp_op==154){
            //cout<<"checkpoint 2: right!"<<endl;
            //}
            //else{
            //cout<<"checkpoint 2: wrong! tmp_op="<<tmp_op<<endl;
            //}

            OperationExecution(selected_OP_ID, Src_OP_IDs, selected_PE_id, source_ready_time, Implementation);

            //output operation that has not been executed should be moved to output PE
            if(output_operation_flag){
                StoreDataInOutMem(selected_OP_ID);
            }

        }
        scheduling_completed=Is_Scheduling_Completed();
    }

}
*/

int Scheduler::Least_Recent_Used_Sel(const std::list<int> &Candidates){

    int Min_Active_Time = INT_MAX;
    int Sel_PE_ID = 0;
    std::list<int>::const_iterator Lcit;

    for(Lcit = Candidates.begin(); Lcit!=Candidates.end(); Lcit++){
        int Current_PE_Active_Time = CGRA->PE_Array[*Lcit]->Max_Active_Time;
        if(Current_PE_Active_Time < Min_Active_Time){
            Sel_PE_ID = *Lcit;
            Min_Active_Time = Current_PE_Active_Time;
        }
    }

    return Sel_PE_ID;

}

/*
int Scheduler::PESelection(const int &Target_OP_ID, const vector<int> &Src_OP_IDs, vector<list<int> > &source_routing_paths, vector<int> &source_operation_ready_time){

    //Initial PE Candidates
    vector<int> candidate_PE_id;

    //Filter the input PE 
    for(int i=0; i<CGRA->CGRA_Scale; i++){
        //if(i!=GL_Var::Store_PE_ID[0]){
        candidate_PE_id.push_back(i);
        //}
    }

    //Reduce iteration times by filtering out some candidate PEs that fails to satisfy certain metric
    //PESelectionFilter(candidate_PE_id, Target_OP_ID, Src_OP_IDs, PhysicalDistanceFiltering);
    //if(candidate_PE_id.size()==0){
    //candidate_PE_id.push_back(GL_Var::Store_PE_ID[0]);
    //}
    if(candidate_PE_id.size()==0){
        ERROR("All the candidate PEs are kicked off by physical distance filter!\n");
    }
    //PESelectionFilter(candidate_PE_id, Target_OP_ID, Src_OP_IDs, MemoryUtilizationFiltering);
    PESelectionFilter(candidate_PE_id, Target_OP_ID, Src_OP_IDs, DSPutilizationFiltering);
    //PESelectionFilter(candidate_PE_id, Target_OP_ID, Src_OP_IDs, WriteMemoryUtilizationFiltering);
    //PESelectionFilter(candidate_PE_id, Target_OP_ID, Src_OP_IDs, OutputPortUtilizationFiltering);
    if(candidate_PE_id.size()==0){
        ERROR("All the candidate PEs are kicked off by DSP utilization filter!\n");
    }

    vector<int> execution_time_on_each_PE;
    execution_time_on_each_PE.resize(CGRA->CGRA_Scale);
    int earliest_execution_time=INT_MAX;
    int selected_PE_id=NaN;
    vector<list<int> > source_routing_paths_tmp;
    source_routing_paths_tmp.resize(INST_OP_NUM-1);
    vector<int> source_operation_ready_time_tmp;
    source_operation_ready_time_tmp.resize(INST_OP_NUM-1);
    list<int> shortest_path;
    for(int i=0; i<CGRA->CGRA_Scale; i++){
        vector<int>::iterator iter_tmp;
        bool is_candidate=false;
        for(iter_tmp=candidate_PE_id.begin(); iter_tmp!=candidate_PE_id.end(); iter_tmp++){
            if(i==*iter_tmp){
                is_candidate=true;
                break;
            }
        }
        if(is_candidate){
            vector<int> arrival_destination_time;
            arrival_destination_time.resize(INST_OP_NUM-1);
            for(int j=0; j<INST_OP_NUM-1; j++){
                int attached_PE_id;
                source_operation_ready_time_tmp[j]=DFG->OP_Array[Src_OP_IDs[j]]->vertex_attribute.operation_avail_time;
                attached_PE_id=NearestAttachedPE(Src_OP_IDs[j], i, source_operation_ready_time_tmp[j]);

                shortest_path.clear();
                CGRA->PossiblePath(source_operation_ready_time_tmp[j],attached_PE_id,i,shortest_path);
                source_routing_paths_tmp[j]=shortest_path;

                arrival_destination_time[j]=OperationTransmission(source_operation_ready_time_tmp[j], Src_OP_IDs[j], shortest_path, Simulation);
            }
            execution_time_on_each_PE[i]=OperationExecution(Target_OP_ID, Src_OP_IDs, i, arrival_destination_time, Simulation);

            //Get the execution time and refresh the selected routing path information that will return
            if(earliest_execution_time>execution_time_on_each_PE[i]){
                earliest_execution_time=execution_time_on_each_PE[i];
                selected_PE_id=i;

                //cout<<"="<<source_operation_ready_time_tmp[0]<<" "<<source_operation_ready_time_tmp[1]<<" "<<source_operation_ready_time[2]<<" ";
                for(int j=0; j<INST_OP_NUM-1; j++){
                    source_routing_paths[j]=source_routing_paths_tmp[j];
                    source_operation_ready_time[j]=source_operation_ready_time_tmp[j];
                }
            }

        }

    }

    return selected_PE_id;

}
*/

int Scheduler::Load_From_IO_Buffer(const int &OP_ID, const Exe_Mode &Mode){

    //Load the operation from IO Buffer
    int Expected_Complete_Time = NaN;
    int Load_PE_ID = Get_IO_Attached_PE_ID(OP_ID);
    for(int i=0; i<INT_MAX; i++){
        bool Load_Path_Avail;
        bool WR_Port1_Avail;
        bool RD_Port3_Avail;
        bool RD_Port4_Avail;
        bool RD_Port5_Avail;

        if(CGRA->Pipeline == OLD){
            Load_Path_Avail = CGRA->PE_Array[Load_PE_ID]->Is_Load_Path_Avail(i+1); 
            WR_Port1_Avail = CGRA->PE_Array[Load_PE_ID]->Is_WR_Port_Avail(i+2, 1);
            RD_Port3_Avail = CGRA->PE_Array[Load_PE_ID]->Is_RD_Port_Avail(i+2, 3);
            RD_Port4_Avail = CGRA->PE_Array[Load_PE_ID]->Is_RD_Port_Avail(i+2, 4);
            RD_Port5_Avail = CGRA->PE_Array[Load_PE_ID]->Is_RD_Port_Avail(i+2, 5);
        }
        else if(CGRA->Pipeline == LF){
            Load_Path_Avail = CGRA->PE_Array[Load_PE_ID]->Is_Load_Path_Avail(i+1);
            WR_Port1_Avail = CGRA->PE_Array[Load_PE_ID]->Is_WR_Port_Avail(i+1, 1);
            RD_Port3_Avail = CGRA->PE_Array[Load_PE_ID]->Is_RD_Port_Avail(i+1, 3);
            RD_Port4_Avail = CGRA->PE_Array[Load_PE_ID]->Is_RD_Port_Avail(i+1, 4);
            RD_Port5_Avail = CGRA->PE_Array[Load_PE_ID]->Is_RD_Port_Avail(i+1, 5);
        }
        else if(CGRA->Pipeline == MF){
            Load_Path_Avail = CGRA->PE_Array[Load_PE_ID]->Is_Load_Path_Avail(i+1);
            WR_Port1_Avail = CGRA->PE_Array[Load_PE_ID]->Is_WR_Port_Avail(i+1, 1);
            RD_Port3_Avail = CGRA->PE_Array[Load_PE_ID]->Is_RD_Port_Avail(i+1, 3);
            RD_Port4_Avail = CGRA->PE_Array[Load_PE_ID]->Is_RD_Port_Avail(i+1, 4);
            RD_Port5_Avail = CGRA->PE_Array[Load_PE_ID]->Is_RD_Port_Avail(i+1, 5);
        }
        else if(CGRA->Pipeline == LHF){
            Load_Path_Avail = CGRA->PE_Array[Load_PE_ID]->Is_Load_Path_Avail(i+1);
            WR_Port1_Avail = CGRA->PE_Array[Load_PE_ID]->Is_WR_Port_Avail(i+2, 1);
            RD_Port3_Avail = CGRA->PE_Array[Load_PE_ID]->Is_RD_Port_Avail(i+2, 3);
            RD_Port4_Avail = CGRA->PE_Array[Load_PE_ID]->Is_RD_Port_Avail(i+2, 4);
            RD_Port5_Avail = CGRA->PE_Array[Load_PE_ID]->Is_RD_Port_Avail(i+2, 5);
        }
        else if(CGRA->Pipeline == HF){
            Load_Path_Avail = CGRA->PE_Array[Load_PE_ID]->Is_Load_Path_Avail(i+1);
            WR_Port1_Avail = CGRA->PE_Array[Load_PE_ID]->Is_WR_Port_Avail(i+2, 1);
            RD_Port3_Avail = CGRA->PE_Array[Load_PE_ID]->Is_RD_Port_Avail(i+2, 3);
            RD_Port4_Avail = CGRA->PE_Array[Load_PE_ID]->Is_RD_Port_Avail(i+2, 4);
            RD_Port5_Avail = CGRA->PE_Array[Load_PE_ID]->Is_RD_Port_Avail(i+2, 5);
        }
        else{
            ERROR("Unknown pipeline intensity setup!\n");
        }

        if(Load_Path_Avail && WR_Port1_Avail && RD_Port3_Avail && \
                RD_Port4_Avail && RD_Port5_Avail){
            if(Mode == Impl){
                if(CGRA->Pipeline == OLD){
                    //update corresponding PE component state
                    CGRA->PE_Array[Load_PE_ID]->Reserve_Load_Path(i+1);
                    CGRA->PE_Array[Load_PE_ID]->Reserve_WR_Port(i+2, 1);

                    //update corresponding PE component activity
                    CGRA->PE_Array[Load_PE_ID]->Set_Load_OP(i, OP_ID);
                    CGRA->PE_Array[Load_PE_ID]->Set_Load_Mux(i+1, 0);
                    CGRA->PE_Array[Load_PE_ID]->Set_WR_Ena(i+2, 1, 1);
                    CGRA->PE_Array[Load_PE_ID]->Set_Mem_Port(i+2, 3, OP_ID);
                    CGRA->PE_Array[Load_PE_ID]->Set_Mem_Port(i+2, 4, OP_ID);
                    CGRA->PE_Array[Load_PE_ID]->Set_Mem_Port(i+2, 5, OP_ID);

                    //Update operation state
                    DFG->OP_Array[OP_ID]->OP_Attribute.OP_State = Avail;
                    DFG->OP_Array[OP_ID]->OP_Attribute.Exe_PE_ID = Load_PE_ID;
                    DFG->OP_Array[OP_ID]->OP_Attribute.OP_Avail_Time = i+2;

                    Attach_History Attach_Point;
                    Attach_Point.Attached_Time = i+2;
                    Attach_Point.Attached_PE_ID = Load_PE_ID;
                    DFG->OP_Array[OP_ID]->OP_Attach_History.push_back(Attach_Point);
                }
                else if(CGRA->Pipeline == LF){
                    //update corresponding PE component state
                    CGRA->PE_Array[Load_PE_ID]->Reserve_Load_Path(i+1);
                    CGRA->PE_Array[Load_PE_ID]->Reserve_WR_Port(i+1, 1);

                    //update corresponding PE component activity
                    CGRA->PE_Array[Load_PE_ID]->Set_Load_OP(i, OP_ID);
                    CGRA->PE_Array[Load_PE_ID]->Set_Load_Mux(i+1, 0);
                    CGRA->PE_Array[Load_PE_ID]->Set_WR_Ena(i+1, 1, 1);
                    CGRA->PE_Array[Load_PE_ID]->Set_Mem_Port(i+1, 3, OP_ID);
                    CGRA->PE_Array[Load_PE_ID]->Set_Mem_Port(i+1, 4, OP_ID);
                    CGRA->PE_Array[Load_PE_ID]->Set_Mem_Port(i+1, 5, OP_ID);

                    //Update operation state
                    DFG->OP_Array[OP_ID]->OP_Attribute.OP_State = Avail;
                    DFG->OP_Array[OP_ID]->OP_Attribute.Exe_PE_ID = Load_PE_ID;
                    DFG->OP_Array[OP_ID]->OP_Attribute.OP_Avail_Time = i+1;

                    Attach_History Attach_Point;
                    Attach_Point.Attached_Time = i+1;
                    Attach_Point.Attached_PE_ID = Load_PE_ID;
                    DFG->OP_Array[OP_ID]->OP_Attach_History.push_back(Attach_Point);
                }
                else if(CGRA->Pipeline == MF){
                    //update corresponding PE component state
                    CGRA->PE_Array[Load_PE_ID]->Reserve_Load_Path(i+1);
                    CGRA->PE_Array[Load_PE_ID]->Reserve_WR_Port(i+1, 1);

                    //update corresponding PE component activity
                    CGRA->PE_Array[Load_PE_ID]->Set_Load_OP(i, OP_ID);
                    CGRA->PE_Array[Load_PE_ID]->Set_Load_Mux(i+1, 0);
                    CGRA->PE_Array[Load_PE_ID]->Set_WR_Ena(i+1, 1, 1);
                    CGRA->PE_Array[Load_PE_ID]->Set_Mem_Port(i+1, 3, OP_ID);
                    CGRA->PE_Array[Load_PE_ID]->Set_Mem_Port(i+1, 4, OP_ID);
                    CGRA->PE_Array[Load_PE_ID]->Set_Mem_Port(i+1, 5, OP_ID);

                    //Update operation state
                    DFG->OP_Array[OP_ID]->OP_Attribute.OP_State = Avail;
                    DFG->OP_Array[OP_ID]->OP_Attribute.Exe_PE_ID = Load_PE_ID;
                    DFG->OP_Array[OP_ID]->OP_Attribute.OP_Avail_Time = i+1;

                    Attach_History Attach_Point;
                    Attach_Point.Attached_Time = i+1;
                    Attach_Point.Attached_PE_ID = Load_PE_ID;
                    DFG->OP_Array[OP_ID]->OP_Attach_History.push_back(Attach_Point);
                }
                else if(CGRA->Pipeline == LHF){
                    //update corresponding PE component state
                    CGRA->PE_Array[Load_PE_ID]->Reserve_Load_Path(i+1);
                    CGRA->PE_Array[Load_PE_ID]->Reserve_WR_Port(i+2, 1);

                    //update corresponding PE component activity
                    CGRA->PE_Array[Load_PE_ID]->Set_Load_OP(i, OP_ID);
                    CGRA->PE_Array[Load_PE_ID]->Set_Load_Mux(i+1, 0);
                    CGRA->PE_Array[Load_PE_ID]->Set_WR_Ena(i+2, 1, 1);
                    CGRA->PE_Array[Load_PE_ID]->Set_Mem_Port(i+2, 3, OP_ID);
                    CGRA->PE_Array[Load_PE_ID]->Set_Mem_Port(i+2, 4, OP_ID);
                    CGRA->PE_Array[Load_PE_ID]->Set_Mem_Port(i+2, 5, OP_ID);

                    //Update operation state
                    DFG->OP_Array[OP_ID]->OP_Attribute.OP_State = Avail;
                    DFG->OP_Array[OP_ID]->OP_Attribute.Exe_PE_ID = Load_PE_ID;
                    DFG->OP_Array[OP_ID]->OP_Attribute.OP_Avail_Time = i+2;

                    Attach_History Attach_Point;
                    Attach_Point.Attached_Time = i+2;
                    Attach_Point.Attached_PE_ID = Load_PE_ID;
                    DFG->OP_Array[OP_ID]->OP_Attach_History.push_back(Attach_Point);
                }
                else if(CGRA->Pipeline == HF){
                    //update corresponding PE component state
                    CGRA->PE_Array[Load_PE_ID]->Reserve_Load_Path(i+1);
                    CGRA->PE_Array[Load_PE_ID]->Reserve_WR_Port(i+2, 1);

                    //update corresponding PE component activity
                    CGRA->PE_Array[Load_PE_ID]->Set_Load_OP(i, OP_ID);
                    CGRA->PE_Array[Load_PE_ID]->Set_Load_Mux(i+1, 0);
                    CGRA->PE_Array[Load_PE_ID]->Set_WR_Ena(i+2, 1, 1);
                    CGRA->PE_Array[Load_PE_ID]->Set_Mem_Port(i+2, 3, OP_ID);
                    CGRA->PE_Array[Load_PE_ID]->Set_Mem_Port(i+2, 4, OP_ID);
                    CGRA->PE_Array[Load_PE_ID]->Set_Mem_Port(i+2, 5, OP_ID);

                    //Update operation state
                    DFG->OP_Array[OP_ID]->OP_Attribute.OP_State = Avail;
                    DFG->OP_Array[OP_ID]->OP_Attribute.Exe_PE_ID = Load_PE_ID;
                    DFG->OP_Array[OP_ID]->OP_Attribute.OP_Avail_Time = i+2;

                    Attach_History Attach_Point;
                    Attach_Point.Attached_Time = i+2;
                    Attach_Point.Attached_PE_ID = Load_PE_ID;
                    DFG->OP_Array[OP_ID]->OP_Attach_History.push_back(Attach_Point);
                }
                else{
                    ERROR("Undefined pipeline intensity setup!\n");
                }

                DFG->OP_Array[OP_ID]->OP_Attribute.OP_LD_Time = i;

                //Dump the trace
                if(GL_Var::Print_Level>10){
                    GL_Var::fTrace << "Load " << OP_ID << " through " \
                        << Load_PE_ID << " at time " << i << std::endl;
                }
            }

            if(CGRA->Pipeline == OLD){
                Expected_Complete_Time = i+2;
            }
            else if(CGRA->Pipeline == LF){
                Expected_Complete_Time = i+1;
            }
            else if(CGRA->Pipeline == MF){
                Expected_Complete_Time = i+1;
            }
            else if(CGRA->Pipeline == LHF){
                Expected_Complete_Time = i+2;
            }
            else if(CGRA->Pipeline == HF){
                Expected_Complete_Time = i+2;
            }
            else{
                ERROR("Unknown pipeline intensity setup!\n");
            }
            break;
        }
    }

    if(Expected_Complete_Time == NaN && DFG->OP_Array[OP_ID]->OP_Type != UNUSED){
        ERROR("OP_ID=%d, Unexpected load occassion!", OP_ID);
    }

    return Expected_Complete_Time;

}

void Scheduler::Store_In_IO_Buffer(const int &OP_ID){

    // Check whether the operation is executed in store_PE
    // If not, pull it out of the data memory first and then send it to store PE.
    int OP_Avail_Time = DFG->OP_Array[OP_ID]->OP_Attribute.OP_Avail_Time + 1;
    int Src_PE_ID = DFG->OP_Array[OP_ID]->OP_Attribute.Exe_PE_ID;
    //int Dst_PE_ID = CGRA->Store_PE_ID[0];
    int Dst_PE_ID = Get_IO_Attached_PE_ID(OP_ID);
    if(Dst_PE_ID == NaN){
        Dst_PE_ID = CGRA->Get_Min_Store_PE(Src_PE_ID);
        DFG->OP_Array[OP_ID]->Reset_Init_PE_ID(Dst_PE_ID);
    }
    
    if(Src_PE_ID == Dst_PE_ID){
        WR_To_IO_Buffer(OP_ID, OP_Avail_Time);
    }
    else{
        //Find a routing path from src to dst
        std::list<int> Routing_Path;
        int OP_Ready_Time = DFG->OP_Array[OP_ID]->OP_Attribute.OP_Avail_Time;

        CGRA->Dynamic_Routing(CGRA->Dynamic_Routing_Alg, \
                OP_Ready_Time, Src_PE_ID, \
                Dst_PE_ID, Routing_Path);

        //Transfer data from src to dst
        int OP_Arrival_Time = OP_Migration(OP_Ready_Time, OP_ID, Routing_Path, Impl) + 1;

        //Move data from dst to IO Buffer
        WR_To_IO_Buffer(OP_ID, OP_Arrival_Time);
    }

}

void Scheduler::WR_To_IO_Buffer(const int &OP_ID, const int &Start_Time){

    int Store_Ready_Time = Start_Time;
    int Store_PE_ID = Get_IO_Attached_PE_ID(OP_ID);

    while(true){
        bool Data_Mem_WR_Avail0 = CGRA->PE_Array[Store_PE_ID]->Is_WR_Port_Avail(Store_Ready_Time, 0);
        bool Data_Mem_RD_Avail0 = CGRA->PE_Array[Store_PE_ID]->Is_RD_Port_Avail(Store_Ready_Time, 0);
        bool Data_Mem_RD_Avail1 = CGRA->PE_Array[Store_PE_ID]->Is_RD_Port_Avail(Store_Ready_Time, 1);
        bool Data_Mem_RD_Avail2 = CGRA->PE_Array[Store_PE_ID]->Is_RD_Port_Avail(Store_Ready_Time, 2);
        bool Store_Path_Avail = CGRA->PE_Array[Store_PE_ID]->Is_Store_Path_Avail(Store_Ready_Time+2);

        if(Data_Mem_WR_Avail0 && Data_Mem_RD_Avail0 && Store_Path_Avail){
            CGRA->PE_Array[Store_PE_ID]->Reserve_RD_Port(Store_Ready_Time,0);
            CGRA->PE_Array[Store_PE_ID]->Reserve_Store_Path(Store_Ready_Time+2);
            CGRA->PE_Array[Store_PE_ID]->Set_WR_Ena(Store_Ready_Time, 0, 0);
            CGRA->PE_Array[Store_PE_ID]->Set_Mem_Port(Store_Ready_Time, 0, OP_ID);
            CGRA->PE_Array[Store_PE_ID]->Set_Store_OP(Store_Ready_Time+2, OP_ID);
            CGRA->PE_Array[Store_PE_ID]->Set_Store_Mux(Store_Ready_Time+2, 0);

            //Dump the trace
            if(GL_Var::Print_Level>10){
                GL_Var::fTrace << "Store " << OP_ID << " in IO Buffer " \
                    << " at time " << Store_Ready_Time+3 << std::endl;
            }

            break;

        }
        else if(Data_Mem_WR_Avail0 && Data_Mem_RD_Avail1 && Store_Path_Avail){
            CGRA->PE_Array[Store_PE_ID]->Reserve_RD_Port(Store_Ready_Time, 1);
            CGRA->PE_Array[Store_PE_ID]->Reserve_Store_Path(Store_Ready_Time+2);
            CGRA->PE_Array[Store_PE_ID]->Set_WR_Ena(Store_Ready_Time, 0, 0);
            CGRA->PE_Array[Store_PE_ID]->Set_Mem_Port(Store_Ready_Time, 1, OP_ID);
            CGRA->PE_Array[Store_PE_ID]->Set_Store_OP(Store_Ready_Time+2, OP_ID);
            CGRA->PE_Array[Store_PE_ID]->Set_Store_Mux(Store_Ready_Time+2, 1);

            //Dump the trace
            if(GL_Var::Print_Level>10){
                GL_Var::fTrace << "Store " << OP_ID << " in IO Buffer " \
                    << " at time " << Store_Ready_Time+3 << std::endl;
            }

            break;
        }
        else if(Data_Mem_WR_Avail0 && Data_Mem_RD_Avail2 && Store_Path_Avail){
            CGRA->PE_Array[Store_PE_ID]->Reserve_RD_Port(Store_Ready_Time, 2);
            CGRA->PE_Array[Store_PE_ID]->Reserve_Store_Path(Store_Ready_Time+2);
            CGRA->PE_Array[Store_PE_ID]->Set_WR_Ena(Store_Ready_Time, 0, 0);
            CGRA->PE_Array[Store_PE_ID]->Set_Mem_Port(Store_Ready_Time, 2, OP_ID);
            CGRA->PE_Array[Store_PE_ID]->Set_Store_OP(Store_Ready_Time+2, OP_ID);
            CGRA->PE_Array[Store_PE_ID]->Set_Store_Mux(Store_Ready_Time+2, 2);

            //Dump the trace
            if(GL_Var::Print_Level>10){
                GL_Var::fTrace << "Store " << OP_ID << " in IO Buffer " \
                    << " at time " << Store_Ready_Time+3 << std::endl;
            }

            break;
        }
        else{
            Store_Ready_Time++;
        }
    }
    
    DFG->OP_Array[OP_ID]->OP_Attribute.OP_ST_Time = Store_Ready_Time + 3;
    if(Scheduling_Complete_Time < (Store_Ready_Time+3)){
        Scheduling_Complete_Time = Store_Ready_Time+3;
    }

}

/*
void Scheduler::PESelectionFilter(vector<int> &candidate_PE_id, const int &Target_OP_ID, const vector<int> &Src_OP_IDs, const PESelectionFilteringType &filtering_type){

    int begin_time;
    int end_time;
    begin_time=INT_MAX;
    float physical_distance_acceptable_percentile=0.25;
    float utilization_acceptable_percentile=0.9;

    for(int i=0; i<INST_OP_NUM-1; i++){
        int src_ready_time=DFG->OP_Array[Src_OP_IDs[i]]->vertex_attribute.operation_avail_time;
        if(begin_time>src_ready_time){
            begin_time=src_ready_time;
        }
    }
    end_time=GL_Var::maximum_simulation_time-1;
    if(filtering_type==PhysicalDistanceFiltering){
        int max_distance=0;
        int min_distance=INT_MAX;
        for(int i=0; i<CGRA->CGRA_Scale; i++){
            for(int j=0; j<CGRA->CGRA_Scale; j++){
                if(CGRA->PE_pair_distance[i][j]>max_distance && CGRA->PE_pair_distance[i][j]>0){
                    max_distance=CGRA->PE_pair_distance[i][j];
                }
                if(CGRA->PE_pair_distance[i][j]<min_distance && CGRA->PE_pair_distance[i][j]>0){
                    min_distance=CGRA->PE_pair_distance[i][j];
                }
            }
        }
        int maximum_acceptable_physical_distance;
        maximum_acceptable_physical_distance=(int)((min_distance+(max_distance-min_distance)*physical_distance_acceptable_percentile)*3);
        vector<int>::iterator iter_tmp;
        for(iter_tmp=candidate_PE_id.begin(); iter_tmp!=candidate_PE_id.end(); ){
            int total_distance=0;
            for(int i=0; i<INST_OP_NUM-1; i++){
                int time_tmp=0;
                int attached_PE_id=NearestAttachedPE(Src_OP_IDs[i], *iter_tmp, time_tmp);
                total_distance=total_distance+CGRA->PE_pair_distance[attached_PE_id][*iter_tmp];
            }
            if(total_distance>maximum_acceptable_physical_distance){
                iter_tmp=candidate_PE_id.erase(iter_tmp);
            }
            else{
                iter_tmp++;
            }
        }
    }
    else if(filtering_type==MemoryUtilizationFiltering){
        vector<float> utilization_per_PE;
        utilization_per_PE.resize(CGRA->CGRA_Scale);
        for(int i=0; i<CGRA->CGRA_Scale; i++){
            utilization_per_PE[i]=1;
        }
        vector<int>::iterator iter_tmp;
        for(iter_tmp=candidate_PE_id.begin(); iter_tmp!=candidate_PE_id.end(); iter_tmp++){
            float utilization_tmp=CGRA->PE_Array[*iter_tmp]->MemoryUtilization(begin_time, end_time);
            utilization_per_PE[*iter_tmp]=utilization_tmp;
        }
        UtilizationFilter(candidate_PE_id, utilization_per_PE, utilization_acceptable_percentile);
    }
    else if(filtering_type==DSPutilizationFiltering){
        vector<float> utilization_per_PE;
        utilization_per_PE.resize(CGRA->CGRA_Scale);
        for(int i=0; i<CGRA->CGRA_Scale; i++){
            utilization_per_PE[i]=1;
        }
        vector<int>::iterator iter_tmp;
        for(iter_tmp=candidate_PE_id.begin(); iter_tmp!=candidate_PE_id.end(); iter_tmp++){
            float utilization_tmp=CGRA->PE_Array[*iter_tmp]->DSPutilization(begin_time, end_time);
            utilization_per_PE[*iter_tmp]=utilization_tmp;
        }
        UtilizationFilter(candidate_PE_id, utilization_per_PE, utilization_acceptable_percentile);
    }
    else if(filtering_type==WriteMemoryUtilizationFiltering){
        vector<float> utilization_per_PE;
        utilization_per_PE.resize(CGRA->CGRA_Scale);
        for(int i=0; i<CGRA->CGRA_Scale; i++){
            utilization_per_PE[i]=1;
        }
        vector<int>::iterator iter_tmp;
        for(iter_tmp=candidate_PE_id.begin(); iter_tmp!=candidate_PE_id.end(); iter_tmp++){
            float utilization_tmp=CGRA->PE_Array[*iter_tmp]->WriteMemoryUtilization(begin_time, end_time);
            utilization_per_PE[*iter_tmp]=utilization_tmp;
        }
        UtilizationFilter(candidate_PE_id, utilization_per_PE, utilization_acceptable_percentile);
    }
    else if(filtering_type==OutputPortUtilizationFiltering){
        vector<float> utilization_per_PE;
        utilization_per_PE.resize(CGRA->CGRA_Scale);
        for(int i=0; i<CGRA->CGRA_Scale; i++){
            utilization_per_PE[i]=1;
        }
        vector<int>::iterator iter_tmp;
        for(iter_tmp=candidate_PE_id.begin(); iter_tmp!=candidate_PE_id.end(); iter_tmp++){
            float utilization_tmp=CGRA->PE_Array[*iter_tmp]->OutputPortUtilization(begin_time, end_time);
            utilization_per_PE[*iter_tmp]=utilization_tmp;
        }
        UtilizationFilter(candidate_PE_id, utilization_per_PE, utilization_acceptable_percentile);
    }
    else{
        ERROR("Unrecognized PE selection filtering type!\n");
    }

}
*/

/*
void Scheduler::UtilizationFilter(vector<int> &candidate_PE_id, const vector<float> &utilization_per_PE, const float &acceptable_percentile){

    float maximum_utilization=0;
    float minimum_utilization=1;
    vector<int>::iterator id_iter;
    for(id_iter=candidate_PE_id.begin(); id_iter!=candidate_PE_id.end(); id_iter++){
        float current_utilization=utilization_per_PE[*id_iter];
        if(current_utilization>maximum_utilization){
            maximum_utilization=current_utilization;
        }
        if(current_utilization<minimum_utilization){
            minimum_utilization=current_utilization;
        }
    }
    if(maximum_utilization>minimum_utilization){
        float metric_utilization=minimum_utilization+acceptable_percentile*(maximum_utilization-minimum_utilization);
        for(id_iter=candidate_PE_id.begin(); id_iter!=candidate_PE_id.end(); ){
            float current_utilization=utilization_per_PE[*id_iter];
            if(current_utilization>metric_utilization){
                id_iter=candidate_PE_id.erase(id_iter);
            }
            else{
                id_iter++;
            }
        }
    }

}

*/


int Scheduler::OP_Migration(const int &Start_Time, const int &Src_OP_ID, \
        const std::list<int> &Routing_Path, const Exe_Mode &Mode){

    int Complete_Time = Start_Time;
    int Migration_Time = Start_Time;

    int PE_Num_On_Path = Routing_Path.size();
    if(PE_Num_On_Path==0){
        ERROR("Empty routing path!\n");
    }
    std::vector<int> Routing_Path_Copy(PE_Num_On_Path);
    std::copy(Routing_Path.begin(), Routing_Path.end(), Routing_Path_Copy.begin());

    //IO load brings additional mux and pipeline.
    int Current_PE_Additional_Pipeline = 0;
    int Next_PE_Additional_Pipeline = 0;
    int Child_Index = NaN;
    int Parent_Index = NaN;
    int Last_Parent_Index = NaN;
    int Last_PE_ID = NaN;

    for(int i=0; i<PE_Num_On_Path;){

        //Which result in additional pipeline.
        int Current_PE_ID = Routing_Path_Copy[i];
        if(CGRA->Is_Load_PE(Current_PE_ID) || CGRA->Is_Store_PE(Current_PE_ID)){
            if(CGRA->Pipeline == OLD){
                Current_PE_Additional_Pipeline = 1;
            }
            else if(CGRA->Pipeline == LF){
                Current_PE_Additional_Pipeline = 0;
            }
            else if(CGRA->Pipeline == MF){
                Current_PE_Additional_Pipeline = 0;
            }
            else if(CGRA->Pipeline == LHF){
                Current_PE_Additional_Pipeline = 0;
            }
            else if(CGRA->Pipeline == HF){
                Current_PE_Additional_Pipeline = 1;
            }
            else{
                ERROR("Unknown pipeline intensity setup!\n");
            }
        }
        else{
            Current_PE_Additional_Pipeline = 0;
        }

        int Next_PE_ID = NaN;
        if(i<PE_Num_On_Path){
            if(i==PE_Num_On_Path-1){
                Next_PE_ID = NaN;
                Child_Index = NaN;
                Last_Parent_Index = Parent_Index;
                Parent_Index = NaN;
            }
            else{
                Next_PE_ID = Routing_Path_Copy[i+1];
                Child_Index = CGRA->Get_Downstream_Index(Current_PE_ID, Next_PE_ID);
                Last_Parent_Index = Parent_Index;
                Parent_Index = CGRA->Get_Upstream_Index(Current_PE_ID, Next_PE_ID);
            }

            if(CGRA->Is_Load_PE(Next_PE_ID) || CGRA->Is_Store_PE(Next_PE_ID)){
                if(CGRA->Pipeline == OLD){
                    Next_PE_Additional_Pipeline = 1;
                }
                else if(CGRA->Pipeline == LF){
                    Next_PE_Additional_Pipeline = 0;
                }
                else if(CGRA->Pipeline == MF){
                    Next_PE_Additional_Pipeline = 0;
                }
                else if(CGRA->Pipeline == LHF){
                    Next_PE_Additional_Pipeline = 0;
                }
                else if(CGRA->Pipeline == HF){
                    Next_PE_Additional_Pipeline = 1;
                }
                else{
                    ERROR("Unknown pipeline intensity setup!\n");
                }
            }
            else{
                Next_PE_Additional_Pipeline = 0;
            }
        }

        //Destination PE is exactly the same with source PE. And, therefore there is no need for data transmission.
        if(PE_Num_On_Path==1){
            i++;
        }
        else{
            //First transmission on the path
            if(i==0){
                bool Current_PE_Data_Mem_RD_Avail0; 
                bool Current_PE_Data_Mem_RD_Avail1;
                bool Current_PE_Data_Mem_RD_Avail2;
                bool Current_PE_Data_Mem_WR_Avail0;
                bool Current_PE_Output_Avail;
                bool Next_PE_Input_Avail;

                if(CGRA->Pipeline == OLD){
                    Current_PE_Data_Mem_RD_Avail0 = CGRA->PE_Array[Current_PE_ID]->Is_RD_Port_Avail(Migration_Time+1, 0);
                    Current_PE_Data_Mem_RD_Avail1 = CGRA->PE_Array[Current_PE_ID]->Is_RD_Port_Avail(Migration_Time+1, 1);
                    Current_PE_Data_Mem_RD_Avail2 = CGRA->PE_Array[Current_PE_ID]->Is_RD_Port_Avail(Migration_Time+1, 2);
                    Current_PE_Data_Mem_WR_Avail0 = CGRA->PE_Array[Current_PE_ID]->Is_WR_Port_Avail(Migration_Time+1, 0);
                    Current_PE_Output_Avail = CGRA->PE_Array[Current_PE_ID]->Is_Output_Port_Avail(Migration_Time+3, Child_Index);
                    Next_PE_Input_Avail = CGRA->PE_Array[Next_PE_ID]->Is_Input_Port_Avail(Migration_Time+5);
                }
                else if(CGRA->Pipeline == LF){
                    Current_PE_Data_Mem_RD_Avail0 = CGRA->PE_Array[Current_PE_ID]->Is_RD_Port_Avail(Migration_Time+1, 0);
                    Current_PE_Data_Mem_RD_Avail1 = CGRA->PE_Array[Current_PE_ID]->Is_RD_Port_Avail(Migration_Time+1, 1);
                    Current_PE_Data_Mem_RD_Avail2 = CGRA->PE_Array[Current_PE_ID]->Is_RD_Port_Avail(Migration_Time+1, 2);
                    Current_PE_Data_Mem_WR_Avail0 = CGRA->PE_Array[Current_PE_ID]->Is_WR_Port_Avail(Migration_Time+1, 0);
                    Current_PE_Output_Avail = CGRA->PE_Array[Current_PE_ID]->Is_Output_Port_Avail(Migration_Time+2, Child_Index);
                    Next_PE_Input_Avail = CGRA->PE_Array[Next_PE_ID]->Is_Input_Port_Avail(Migration_Time+3);
                }
                else if(CGRA->Pipeline == MF){
                    Current_PE_Data_Mem_RD_Avail0 = CGRA->PE_Array[Current_PE_ID]->Is_RD_Port_Avail(Migration_Time+1, 0);
                    Current_PE_Data_Mem_RD_Avail1 = CGRA->PE_Array[Current_PE_ID]->Is_RD_Port_Avail(Migration_Time+1, 1);
                    Current_PE_Data_Mem_RD_Avail2 = CGRA->PE_Array[Current_PE_ID]->Is_RD_Port_Avail(Migration_Time+1, 2);
                    Current_PE_Data_Mem_WR_Avail0 = CGRA->PE_Array[Current_PE_ID]->Is_WR_Port_Avail(Migration_Time+1, 0);
                    Current_PE_Output_Avail = CGRA->PE_Array[Current_PE_ID]->Is_Output_Port_Avail(Migration_Time+2, Child_Index);
                    Next_PE_Input_Avail = CGRA->PE_Array[Next_PE_ID]->Is_Input_Port_Avail(Migration_Time+3);
                }
                else if(CGRA->Pipeline == LHF){
                    Current_PE_Data_Mem_RD_Avail0 = CGRA->PE_Array[Current_PE_ID]->Is_RD_Port_Avail(Migration_Time+1, 0);
                    Current_PE_Data_Mem_RD_Avail1 = CGRA->PE_Array[Current_PE_ID]->Is_RD_Port_Avail(Migration_Time+1, 1);
                    Current_PE_Data_Mem_RD_Avail2 = CGRA->PE_Array[Current_PE_ID]->Is_RD_Port_Avail(Migration_Time+1, 2);
                    Current_PE_Data_Mem_WR_Avail0 = CGRA->PE_Array[Current_PE_ID]->Is_WR_Port_Avail(Migration_Time+1, 0);
                    Current_PE_Output_Avail = CGRA->PE_Array[Current_PE_ID]->Is_Output_Port_Avail(Migration_Time+2, Child_Index);
                    Next_PE_Input_Avail = CGRA->PE_Array[Next_PE_ID]->Is_Input_Port_Avail(Migration_Time+4);
                }
                else if(CGRA->Pipeline == HF){
                    Current_PE_Data_Mem_RD_Avail0 = CGRA->PE_Array[Current_PE_ID]->Is_RD_Port_Avail(Migration_Time+1, 0);
                    Current_PE_Data_Mem_RD_Avail1 = CGRA->PE_Array[Current_PE_ID]->Is_RD_Port_Avail(Migration_Time+1, 1);
                    Current_PE_Data_Mem_RD_Avail2 = CGRA->PE_Array[Current_PE_ID]->Is_RD_Port_Avail(Migration_Time+1, 2);
                    Current_PE_Data_Mem_WR_Avail0 = CGRA->PE_Array[Current_PE_ID]->Is_WR_Port_Avail(Migration_Time+1, 0);
                    Current_PE_Output_Avail = CGRA->PE_Array[Current_PE_ID]->Is_Output_Port_Avail(Migration_Time+5, Child_Index);
                    Next_PE_Input_Avail = CGRA->PE_Array[Next_PE_ID]->Is_Input_Port_Avail(Migration_Time+7);
                }
                else{
                    ERROR("Unknown pipeline intensity setup!\n");
                }

                bool Next_Load_Path_Avail = true;
                if(CGRA->Is_Load_PE(Next_PE_ID) || CGRA->Is_Store_PE(Next_PE_ID)){
                    if(CGRA->Pipeline == OLD){
                        Next_Load_Path_Avail = CGRA->PE_Array[Next_PE_ID]->Is_Load_Path_Avail(Migration_Time+6);
                    }
                    else if(CGRA->Pipeline == LF){
                        Next_Load_Path_Avail = CGRA->PE_Array[Next_PE_ID]->Is_Load_Path_Avail(Migration_Time+3);
                    }
                    else if(CGRA->Pipeline == MF){
                        Next_Load_Path_Avail = CGRA->PE_Array[Next_PE_ID]->Is_Load_Path_Avail(Migration_Time+3);
                    }
                    else if(CGRA->Pipeline == LHF){
                        Next_Load_Path_Avail = CGRA->PE_Array[Next_PE_ID]->Is_Load_Path_Avail(Migration_Time+4);
                    }
                    else if(CGRA->Pipeline == HF){
                        Next_Load_Path_Avail = CGRA->PE_Array[Next_PE_ID]->Is_Load_Path_Avail(Migration_Time+8);
                    }
                    else{
                        ERROR("Undefined pipeline intensity setup!\n");
                    }
                }

                bool Next_PE_Data_Mem_WR_Avail1;
                bool Next_PE_Data_Mem_RD_Avail3;
                bool Next_PE_Data_Mem_RD_Avail4;
                bool Next_PE_Data_Mem_RD_Avail5;
                if(CGRA->Pipeline == OLD){
                    Next_PE_Data_Mem_WR_Avail1 = CGRA->PE_Array[Next_PE_ID]->Is_WR_Port_Avail(Migration_Time+6+Next_PE_Additional_Pipeline, 1);
                    Next_PE_Data_Mem_RD_Avail3 = CGRA->PE_Array[Next_PE_ID]->Is_RD_Port_Avail(Migration_Time+6+Next_PE_Additional_Pipeline, 3);
                    Next_PE_Data_Mem_RD_Avail4 = CGRA->PE_Array[Next_PE_ID]->Is_RD_Port_Avail(Migration_Time+6+Next_PE_Additional_Pipeline, 4);
                    Next_PE_Data_Mem_RD_Avail5 = CGRA->PE_Array[Next_PE_ID]->Is_RD_Port_Avail(Migration_Time+6+Next_PE_Additional_Pipeline, 5);
                }
                else if(CGRA->Pipeline == LF){
                    Next_PE_Data_Mem_WR_Avail1 = CGRA->PE_Array[Next_PE_ID]->Is_WR_Port_Avail(Migration_Time+3+Next_PE_Additional_Pipeline, 1);
                    Next_PE_Data_Mem_RD_Avail3 = CGRA->PE_Array[Next_PE_ID]->Is_RD_Port_Avail(Migration_Time+3+Next_PE_Additional_Pipeline, 3);
                    Next_PE_Data_Mem_RD_Avail4 = CGRA->PE_Array[Next_PE_ID]->Is_RD_Port_Avail(Migration_Time+3+Next_PE_Additional_Pipeline, 4);
                    Next_PE_Data_Mem_RD_Avail5 = CGRA->PE_Array[Next_PE_ID]->Is_RD_Port_Avail(Migration_Time+3+Next_PE_Additional_Pipeline, 5);
                }
                else if(CGRA->Pipeline == MF){
                    Next_PE_Data_Mem_WR_Avail1 = CGRA->PE_Array[Next_PE_ID]->Is_WR_Port_Avail(Migration_Time+3+Next_PE_Additional_Pipeline, 1);
                    Next_PE_Data_Mem_RD_Avail3 = CGRA->PE_Array[Next_PE_ID]->Is_RD_Port_Avail(Migration_Time+3+Next_PE_Additional_Pipeline, 3);
                    Next_PE_Data_Mem_RD_Avail4 = CGRA->PE_Array[Next_PE_ID]->Is_RD_Port_Avail(Migration_Time+3+Next_PE_Additional_Pipeline, 4);
                    Next_PE_Data_Mem_RD_Avail5 = CGRA->PE_Array[Next_PE_ID]->Is_RD_Port_Avail(Migration_Time+3+Next_PE_Additional_Pipeline, 5);
                }
                else if(CGRA->Pipeline == LHF){
                    Next_PE_Data_Mem_WR_Avail1 = CGRA->PE_Array[Next_PE_ID]->Is_WR_Port_Avail(Migration_Time+5+Next_PE_Additional_Pipeline, 1);
                    Next_PE_Data_Mem_RD_Avail3 = CGRA->PE_Array[Next_PE_ID]->Is_RD_Port_Avail(Migration_Time+5+Next_PE_Additional_Pipeline, 3);
                    Next_PE_Data_Mem_RD_Avail4 = CGRA->PE_Array[Next_PE_ID]->Is_RD_Port_Avail(Migration_Time+5+Next_PE_Additional_Pipeline, 4);
                    Next_PE_Data_Mem_RD_Avail5 = CGRA->PE_Array[Next_PE_ID]->Is_RD_Port_Avail(Migration_Time+5+Next_PE_Additional_Pipeline, 5);
                }
                else if(CGRA->Pipeline == HF){
                    Next_PE_Data_Mem_WR_Avail1 = CGRA->PE_Array[Next_PE_ID]->Is_WR_Port_Avail(Migration_Time+8+Next_PE_Additional_Pipeline, 1);
                    Next_PE_Data_Mem_RD_Avail3 = CGRA->PE_Array[Next_PE_ID]->Is_RD_Port_Avail(Migration_Time+8+Next_PE_Additional_Pipeline, 3);
                    Next_PE_Data_Mem_RD_Avail4 = CGRA->PE_Array[Next_PE_ID]->Is_RD_Port_Avail(Migration_Time+8+Next_PE_Additional_Pipeline, 4);
                    Next_PE_Data_Mem_RD_Avail5 = CGRA->PE_Array[Next_PE_ID]->Is_RD_Port_Avail(Migration_Time+8+Next_PE_Additional_Pipeline, 5);
                }
                else{
                    ERROR("Undefined pipeline intensity setup!\n");
                }

                bool Current_PE_Data_Mem_RD_Avail = (Current_PE_Data_Mem_RD_Avail0 || Current_PE_Data_Mem_RD_Avail1 || Current_PE_Data_Mem_RD_Avail2) && Current_PE_Data_Mem_WR_Avail0;
                bool Next_PE_Data_Mem_WR_Avail = Next_PE_Data_Mem_WR_Avail1 && Next_PE_Data_Mem_RD_Avail3 && Next_PE_Data_Mem_RD_Avail4 && Next_PE_Data_Mem_RD_Avail5;
                if(Current_PE_Data_Mem_RD_Avail && Current_PE_Output_Avail && Next_PE_Input_Avail && Next_PE_Data_Mem_WR_Avail && Next_Load_Path_Avail){
                    if(Mode==Impl){
                        if(Current_PE_Data_Mem_RD_Avail0){
                            if(CGRA->Pipeline == OLD){
                                CGRA->PE_Array[Current_PE_ID]->Reserve_RD_Port(Migration_Time+1, 0);
                                CGRA->PE_Array[Current_PE_ID]->Set_Mem_Port(Migration_Time+1, 0, Src_OP_ID);
                                CGRA->PE_Array[Current_PE_ID]->Set_Output_Mux(Migration_Time+3, Child_Index, 0);
                            }
                            else if(CGRA->Pipeline == LF){
                                CGRA->PE_Array[Current_PE_ID]->Reserve_RD_Port(Migration_Time+1, 0);
                                CGRA->PE_Array[Current_PE_ID]->Set_Mem_Port(Migration_Time+1, 0, Src_OP_ID);
                                CGRA->PE_Array[Current_PE_ID]->Set_Output_Mux(Migration_Time+2, Child_Index, 0);
                            }
                            else if(CGRA->Pipeline == MF){
                                CGRA->PE_Array[Current_PE_ID]->Reserve_RD_Port(Migration_Time+1, 0);
                                CGRA->PE_Array[Current_PE_ID]->Set_Mem_Port(Migration_Time+1, 0, Src_OP_ID);
                                CGRA->PE_Array[Current_PE_ID]->Set_Output_Mux(Migration_Time+2, Child_Index, 0);
                            }
                            else if(CGRA->Pipeline == LHF){
                                CGRA->PE_Array[Current_PE_ID]->Reserve_RD_Port(Migration_Time+1, 0);
                                CGRA->PE_Array[Current_PE_ID]->Set_Mem_Port(Migration_Time+1, 0, Src_OP_ID);
                                CGRA->PE_Array[Current_PE_ID]->Set_Output_Mux(Migration_Time+2, Child_Index, 0);
                            }
                            else if(CGRA->Pipeline == HF){
                                CGRA->PE_Array[Current_PE_ID]->Reserve_RD_Port(Migration_Time+1, 0);
                                CGRA->PE_Array[Current_PE_ID]->Set_Mem_Port(Migration_Time+1, 0, Src_OP_ID);
                                CGRA->PE_Array[Current_PE_ID]->Set_Output_Mux(Migration_Time+5, Child_Index, 0);
                            }
                            else{
                                ERROR("Undefined pipeline intensity setup!\n");
                            }
                        }
                        else if(Current_PE_Data_Mem_RD_Avail1){
                            if(CGRA->Pipeline == OLD){
                                CGRA->PE_Array[Current_PE_ID]->Reserve_RD_Port(Migration_Time+1, 1);
                                CGRA->PE_Array[Current_PE_ID]->Set_Mem_Port(Migration_Time+1, 1, Src_OP_ID);
                                CGRA->PE_Array[Current_PE_ID]->Set_Output_Mux(Migration_Time+3, Child_Index, 1);
                            }
                            else if(CGRA->Pipeline == LF){
                                CGRA->PE_Array[Current_PE_ID]->Reserve_RD_Port(Migration_Time+1, 1);
                                CGRA->PE_Array[Current_PE_ID]->Set_Mem_Port(Migration_Time+1, 1, Src_OP_ID);
                                CGRA->PE_Array[Current_PE_ID]->Set_Output_Mux(Migration_Time+2, Child_Index, 1);
                            }
                            else if(CGRA->Pipeline == MF){
                                CGRA->PE_Array[Current_PE_ID]->Reserve_RD_Port(Migration_Time+1, 1);
                                CGRA->PE_Array[Current_PE_ID]->Set_Mem_Port(Migration_Time+1, 1, Src_OP_ID);
                                CGRA->PE_Array[Current_PE_ID]->Set_Output_Mux(Migration_Time+2, Child_Index, 1);
                            }
                            else if(CGRA->Pipeline == LHF){
                                CGRA->PE_Array[Current_PE_ID]->Reserve_RD_Port(Migration_Time+1, 1);
                                CGRA->PE_Array[Current_PE_ID]->Set_Mem_Port(Migration_Time+1, 1, Src_OP_ID);
                                CGRA->PE_Array[Current_PE_ID]->Set_Output_Mux(Migration_Time+2, Child_Index, 1);
                            }
                            else if(CGRA->Pipeline == HF){
                                CGRA->PE_Array[Current_PE_ID]->Reserve_RD_Port(Migration_Time+1, 1);
                                CGRA->PE_Array[Current_PE_ID]->Set_Mem_Port(Migration_Time+1, 1, Src_OP_ID);
                                CGRA->PE_Array[Current_PE_ID]->Set_Output_Mux(Migration_Time+5, Child_Index, 1);
                            }
                            else{
                                ERROR("Unknown pipeline intensity setup!\n");
                            }
                        }
                        else{
                            if(CGRA->Pipeline == OLD){
                                CGRA->PE_Array[Current_PE_ID]->Reserve_RD_Port(Migration_Time+1, 2);
                                CGRA->PE_Array[Current_PE_ID]->Set_Mem_Port(Migration_Time+1, 2, Src_OP_ID);
                                CGRA->PE_Array[Current_PE_ID]->Set_Output_Mux(Migration_Time+3, Child_Index, 2);
                            }
                            else if(CGRA->Pipeline == LF){
                                CGRA->PE_Array[Current_PE_ID]->Reserve_RD_Port(Migration_Time+1, 2);
                                CGRA->PE_Array[Current_PE_ID]->Set_Mem_Port(Migration_Time+1, 2, Src_OP_ID);
                                CGRA->PE_Array[Current_PE_ID]->Set_Output_Mux(Migration_Time+2, Child_Index, 2);
                            }
                            else if(CGRA->Pipeline == MF){
                                CGRA->PE_Array[Current_PE_ID]->Reserve_RD_Port(Migration_Time+1, 2);
                                CGRA->PE_Array[Current_PE_ID]->Set_Mem_Port(Migration_Time+1, 2, Src_OP_ID);
                                CGRA->PE_Array[Current_PE_ID]->Set_Output_Mux(Migration_Time+2, Child_Index, 2);
                            }
                            else if(CGRA->Pipeline == LHF){
                                CGRA->PE_Array[Current_PE_ID]->Reserve_RD_Port(Migration_Time+1, 2);
                                CGRA->PE_Array[Current_PE_ID]->Set_Mem_Port(Migration_Time+1, 2, Src_OP_ID);
                                CGRA->PE_Array[Current_PE_ID]->Set_Output_Mux(Migration_Time+2, Child_Index, 2);
                            }
                            else if(CGRA->Pipeline == HF){
                                CGRA->PE_Array[Current_PE_ID]->Reserve_RD_Port(Migration_Time+1, 2);
                                CGRA->PE_Array[Current_PE_ID]->Set_Mem_Port(Migration_Time+1, 2, Src_OP_ID);
                                CGRA->PE_Array[Current_PE_ID]->Set_Output_Mux(Migration_Time+5, Child_Index, 2);
                            }
                            else{
                                ERROR("Unkown pipeline intensity setup!\n");
                            }
                        }

                        if(CGRA->Pipeline == OLD){
                            CGRA->PE_Array[Current_PE_ID]->Reserve_Output_Port(Migration_Time+3, Child_Index);
                            CGRA->PE_Array[Current_PE_ID]->Set_WR_Ena(Migration_Time+1, 0, 0);
                        }
                        else if(CGRA->Pipeline == LF){
                            CGRA->PE_Array[Current_PE_ID]->Reserve_Output_Port(Migration_Time+2, Child_Index);
                            CGRA->PE_Array[Current_PE_ID]->Set_WR_Ena(Migration_Time+1, 0, 0);
                        }
                        else if(CGRA->Pipeline == MF){
                            CGRA->PE_Array[Current_PE_ID]->Reserve_Output_Port(Migration_Time+2, Child_Index);
                            CGRA->PE_Array[Current_PE_ID]->Set_WR_Ena(Migration_Time+1, 0, 0);
                        }
                        else if(CGRA->Pipeline == LHF){
                            CGRA->PE_Array[Current_PE_ID]->Reserve_Output_Port(Migration_Time+2, Child_Index);
                            CGRA->PE_Array[Current_PE_ID]->Set_WR_Ena(Migration_Time+1, 0, 0);
                        }
                        else if(CGRA->Pipeline == HF){
                            CGRA->PE_Array[Current_PE_ID]->Reserve_Output_Port(Migration_Time+5, Child_Index);
                            CGRA->PE_Array[Current_PE_ID]->Set_WR_Ena(Migration_Time+1, 0, 0);
                        }
                        else{
                            ERROR("Undefined pipeline intensity level!\n");
                        }                       

                        if(GL_Var::Print_Level>10){
                            GL_Var::fTrace << "Move operation " << Src_OP_ID << " First: from " << " PE " << Current_PE_ID << " to "<< " PE " << Next_PE_ID << " at time " << Migration_Time+1 << std::endl;
                        }

                    }
                    if(CGRA->Pipeline == OLD){
                        Migration_Time += 4;
                    }
                    else if(CGRA->Pipeline == LF){
                        Migration_Time += 2;
                    }
                    else if(CGRA->Pipeline == MF){
                        Migration_Time += 2;
                    }
                    else if(CGRA->Pipeline == LHF){
                        Migration_Time += 3;
                    }
                    else if(CGRA->Pipeline == HF){
                        Migration_Time += 6;
                    }
                    else{
                        ERROR("Unknown pipeline intensity!\n");
                    }
                    i++;
                }
                else{
                    Migration_Time++;
                }
            }

            // Transmission in the routing path 
            else if(i>0 && i<PE_Num_On_Path-1){
                bool Current_PE_Bypass_Avail;
                bool Current_PE_Output_Avail;
                bool Next_PE_Input_Avail;
                if(CGRA->Pipeline == OLD){
                    Current_PE_Bypass_Avail = CGRA->PE_Array[Current_PE_ID]->Is_Bypass_Avail(Migration_Time+1);
                    Current_PE_Output_Avail = CGRA->PE_Array[Current_PE_ID]->Is_Output_Port_Avail(Migration_Time+2, Child_Index);
                    Next_PE_Input_Avail = CGRA->PE_Array[Next_PE_ID]->Is_Input_Port_Avail(Migration_Time+4);
                }
                else if(CGRA->Pipeline == LF){
                    Current_PE_Bypass_Avail = CGRA->PE_Array[Current_PE_ID]->Is_Bypass_Avail(Migration_Time+1);
                    Current_PE_Output_Avail = CGRA->PE_Array[Current_PE_ID]->Is_Output_Port_Avail(Migration_Time+1, Child_Index);
                    Next_PE_Input_Avail = CGRA->PE_Array[Next_PE_ID]->Is_Input_Port_Avail(Migration_Time+2);
                }
                else if(CGRA->Pipeline == MF){
                    Current_PE_Bypass_Avail = CGRA->PE_Array[Current_PE_ID]->Is_Bypass_Avail(Migration_Time+1);
                    Current_PE_Output_Avail = CGRA->PE_Array[Current_PE_ID]->Is_Output_Port_Avail(Migration_Time+1, Child_Index);
                    Next_PE_Input_Avail = CGRA->PE_Array[Next_PE_ID]->Is_Input_Port_Avail(Migration_Time+2);
                }
                else if(CGRA->Pipeline == LHF){
                    Current_PE_Bypass_Avail = CGRA->PE_Array[Current_PE_ID]->Is_Bypass_Avail(Migration_Time+1);
                    Current_PE_Output_Avail = CGRA->PE_Array[Current_PE_ID]->Is_Output_Port_Avail(Migration_Time+1, Child_Index);
                    Next_PE_Input_Avail = CGRA->PE_Array[Next_PE_ID]->Is_Input_Port_Avail(Migration_Time+3);
                }
                else if(CGRA->Pipeline == HF){
                    Current_PE_Bypass_Avail = CGRA->PE_Array[Current_PE_ID]->Is_Bypass_Avail(Migration_Time+1);
                    Current_PE_Output_Avail = CGRA->PE_Array[Current_PE_ID]->Is_Output_Port_Avail(Migration_Time+1, Child_Index);
                    Next_PE_Input_Avail = CGRA->PE_Array[Next_PE_ID]->Is_Input_Port_Avail(Migration_Time+4);
                }
                else{
                    ERROR("Unknown pipeline intensity setup!\n");
                }
                
                bool Next_Load_Path_Avail = true;
                if(CGRA->Is_Load_PE(Next_PE_ID) || CGRA->Is_Store_PE(Next_PE_ID)){
                    if(CGRA->Pipeline == OLD){
                        Next_Load_Path_Avail = CGRA->PE_Array[Next_PE_ID]->Is_Load_Path_Avail(Migration_Time+5);
                    }
                    else if(CGRA->Pipeline == LF){
                        Next_Load_Path_Avail = CGRA->PE_Array[Next_PE_ID]->Is_Load_Path_Avail(Migration_Time+2);
                    }
                    else if(CGRA->Pipeline == MF){
                        Next_Load_Path_Avail = CGRA->PE_Array[Next_PE_ID]->Is_Load_Path_Avail(Migration_Time+2);
                    }
                    else if(CGRA->Pipeline == LHF){
                        Next_Load_Path_Avail = CGRA->PE_Array[Next_PE_ID]->Is_Load_Path_Avail(Migration_Time+3);
                    }
                    else if(CGRA->Pipeline == HF){
                        Next_Load_Path_Avail = CGRA->PE_Array[Next_PE_ID]->Is_Load_Path_Avail(Migration_Time+5);
                    }
                    else{
                        ERROR("Unknown pipeline intensity setup! \n");
                    }
                }


                bool Next_PE_Data_Mem_WR_Avail1;
                bool Next_PE_Data_Mem_RD_Avail3;
                bool Next_PE_Data_Mem_RD_Avail4;
                bool Next_PE_Data_Mem_RD_Avail5;
                if(CGRA->Pipeline == OLD){
                    Next_PE_Data_Mem_WR_Avail1 = CGRA->PE_Array[Next_PE_ID]->Is_WR_Port_Avail(Migration_Time+5+Next_PE_Additional_Pipeline, 1);
                    Next_PE_Data_Mem_RD_Avail3 = CGRA->PE_Array[Next_PE_ID]->Is_RD_Port_Avail(Migration_Time+5+Next_PE_Additional_Pipeline, 3);
                    Next_PE_Data_Mem_RD_Avail4 = CGRA->PE_Array[Next_PE_ID]->Is_RD_Port_Avail(Migration_Time+5+Next_PE_Additional_Pipeline, 4);
                    Next_PE_Data_Mem_RD_Avail5 = CGRA->PE_Array[Next_PE_ID]->Is_RD_Port_Avail(Migration_Time+5+Next_PE_Additional_Pipeline, 5);
                }
                else if(CGRA->Pipeline == LF){
                    Next_PE_Data_Mem_WR_Avail1 = CGRA->PE_Array[Next_PE_ID]->Is_WR_Port_Avail(Migration_Time+2+Next_PE_Additional_Pipeline, 1);
                    Next_PE_Data_Mem_RD_Avail3 = CGRA->PE_Array[Next_PE_ID]->Is_RD_Port_Avail(Migration_Time+2+Next_PE_Additional_Pipeline, 3);
                    Next_PE_Data_Mem_RD_Avail4 = CGRA->PE_Array[Next_PE_ID]->Is_RD_Port_Avail(Migration_Time+2+Next_PE_Additional_Pipeline, 4);
                    Next_PE_Data_Mem_RD_Avail5 = CGRA->PE_Array[Next_PE_ID]->Is_RD_Port_Avail(Migration_Time+2+Next_PE_Additional_Pipeline, 5);
                }
                else if(CGRA->Pipeline == MF){
                    Next_PE_Data_Mem_WR_Avail1 = CGRA->PE_Array[Next_PE_ID]->Is_WR_Port_Avail(Migration_Time+2+Next_PE_Additional_Pipeline, 1);
                    Next_PE_Data_Mem_RD_Avail3 = CGRA->PE_Array[Next_PE_ID]->Is_RD_Port_Avail(Migration_Time+2+Next_PE_Additional_Pipeline, 3);
                    Next_PE_Data_Mem_RD_Avail4 = CGRA->PE_Array[Next_PE_ID]->Is_RD_Port_Avail(Migration_Time+2+Next_PE_Additional_Pipeline, 4);
                    Next_PE_Data_Mem_RD_Avail5 = CGRA->PE_Array[Next_PE_ID]->Is_RD_Port_Avail(Migration_Time+2+Next_PE_Additional_Pipeline, 5);
                }
                else if(CGRA->Pipeline == LHF){
                    Next_PE_Data_Mem_WR_Avail1 = CGRA->PE_Array[Next_PE_ID]->Is_WR_Port_Avail(Migration_Time+4+Next_PE_Additional_Pipeline, 1);
                    Next_PE_Data_Mem_RD_Avail3 = CGRA->PE_Array[Next_PE_ID]->Is_RD_Port_Avail(Migration_Time+4+Next_PE_Additional_Pipeline, 3);
                    Next_PE_Data_Mem_RD_Avail4 = CGRA->PE_Array[Next_PE_ID]->Is_RD_Port_Avail(Migration_Time+4+Next_PE_Additional_Pipeline, 4);
                    Next_PE_Data_Mem_RD_Avail5 = CGRA->PE_Array[Next_PE_ID]->Is_RD_Port_Avail(Migration_Time+4+Next_PE_Additional_Pipeline, 5);
                }
                else if(CGRA->Pipeline == HF){
                    Next_PE_Data_Mem_WR_Avail1 = CGRA->PE_Array[Next_PE_ID]->Is_WR_Port_Avail(Migration_Time+5+Next_PE_Additional_Pipeline, 1);
                    Next_PE_Data_Mem_RD_Avail3 = CGRA->PE_Array[Next_PE_ID]->Is_RD_Port_Avail(Migration_Time+5+Next_PE_Additional_Pipeline, 3);
                    Next_PE_Data_Mem_RD_Avail4 = CGRA->PE_Array[Next_PE_ID]->Is_RD_Port_Avail(Migration_Time+5+Next_PE_Additional_Pipeline, 4);
                    Next_PE_Data_Mem_RD_Avail5 = CGRA->PE_Array[Next_PE_ID]->Is_RD_Port_Avail(Migration_Time+5+Next_PE_Additional_Pipeline, 5);
                }
                else{
                    ERROR("Unknown pipeline intensity setup!\n");
                }
                
                bool Next_PE_Data_Mem_WR_Avail = Next_PE_Data_Mem_WR_Avail1 && Next_PE_Data_Mem_RD_Avail3 && Next_PE_Data_Mem_RD_Avail4 && Next_PE_Data_Mem_RD_Avail5;

                //Bypass occasion
                if(Current_PE_Bypass_Avail && Current_PE_Output_Avail && Next_PE_Input_Avail && Next_PE_Data_Mem_WR_Avail && Next_Load_Path_Avail){
                    if(Mode == Impl){
                        if(CGRA->Pipeline == OLD){
                            CGRA->PE_Array[Current_PE_ID]->Reserve_Bypass(Migration_Time+1);
                            CGRA->PE_Array[Current_PE_ID]->Reserve_Output_Port(Migration_Time+2, Child_Index);
                            CGRA->PE_Array[Current_PE_ID]->Set_Bypass_Mux(Migration_Time+1, Last_Parent_Index);
                            CGRA->PE_Array[Current_PE_ID]->Set_Output_Mux(Migration_Time+2, Child_Index, 3);
                        }
                        else if(CGRA->Pipeline == LF){
                            CGRA->PE_Array[Current_PE_ID]->Reserve_Bypass(Migration_Time+1);
                            CGRA->PE_Array[Current_PE_ID]->Reserve_Output_Port(Migration_Time+1, Child_Index);
                            CGRA->PE_Array[Current_PE_ID]->Set_Bypass_Mux(Migration_Time+1, Last_Parent_Index);
                            CGRA->PE_Array[Current_PE_ID]->Set_Output_Mux(Migration_Time+1, Child_Index, 3);
                        }
                        else if(CGRA->Pipeline == MF){
                            CGRA->PE_Array[Current_PE_ID]->Reserve_Bypass(Migration_Time+1);
                            CGRA->PE_Array[Current_PE_ID]->Reserve_Output_Port(Migration_Time+1, Child_Index);
                            CGRA->PE_Array[Current_PE_ID]->Set_Bypass_Mux(Migration_Time+1, Last_Parent_Index);
                            CGRA->PE_Array[Current_PE_ID]->Set_Output_Mux(Migration_Time+1, Child_Index, 3);
                        }
                        else if(CGRA->Pipeline == LHF){
                            CGRA->PE_Array[Current_PE_ID]->Reserve_Bypass(Migration_Time+1);
                            CGRA->PE_Array[Current_PE_ID]->Reserve_Output_Port(Migration_Time+1, Child_Index);
                            CGRA->PE_Array[Current_PE_ID]->Set_Bypass_Mux(Migration_Time+1, Last_Parent_Index);
                            CGRA->PE_Array[Current_PE_ID]->Set_Output_Mux(Migration_Time+1, Child_Index, 3);
                        }
                        else if(CGRA->Pipeline == HF){
                            CGRA->PE_Array[Current_PE_ID]->Reserve_Bypass(Migration_Time+1);
                            CGRA->PE_Array[Current_PE_ID]->Reserve_Output_Port(Migration_Time+2, Child_Index);
                            CGRA->PE_Array[Current_PE_ID]->Set_Bypass_Mux(Migration_Time+1, Last_Parent_Index);
                            CGRA->PE_Array[Current_PE_ID]->Set_Output_Mux(Migration_Time+2, Child_Index, 3);
                        }
                        else{
                            ERROR("Unknown pipeline intensity setup!\n");
                        }

                        if(GL_Var::Print_Level>10){
                            GL_Var::fTrace << "Move operation " << Src_OP_ID<< " bypass: from" << " PE " << Current_PE_ID << " to " << " PE " << Next_PE_ID << " at time " << Migration_Time << std::endl;
                        }

                    }
                    if(CGRA->Pipeline == OLD){
                        Migration_Time += 3;
                    }
                    else if(CGRA->Pipeline == LF){
                        Migration_Time += 1;
                    }
                    else if(CGRA->Pipeline == MF){
                        Migration_Time += 1;
                    }
                    else if(CGRA->Pipeline == LHF){
                        Migration_Time += 2;
                    }
                    else if(CGRA->Pipeline == HF){
                        Migration_Time += 3;
                    }
                    else{
                        ERROR("Unknown pipeline intensity setup!\n");
                    }
                    i++;
                }

                //Store and forward occasion
                else{
                    //If the data needs to be stored, there must be no resource confliction and we simply reserve the corresponding resources.
                    if(Mode == Impl){
                        if(CGRA->Pipeline == OLD){
                            CGRA->PE_Array[Current_PE_ID]->Reserve_Input_Port(Migration_Time+1);
                            CGRA->PE_Array[Current_PE_ID]->Set_Input_Mux(Migration_Time+1, Last_Parent_Index);
                            if(CGRA->Is_Load_PE(Current_PE_ID) || CGRA->Is_Store_PE(Current_PE_ID)){
                                CGRA->PE_Array[Current_PE_ID]->Reserve_Load_Path(Migration_Time+2);
                                CGRA->PE_Array[Current_PE_ID]->Set_Load_Mux(Migration_Time+2, 1);
                            }
                            CGRA->PE_Array[Current_PE_ID]->Reserve_WR_Port(Migration_Time+2+Current_PE_Additional_Pipeline, 1);
                            CGRA->PE_Array[Current_PE_ID]->Set_WR_Ena(Migration_Time+2+Current_PE_Additional_Pipeline, 1, 1);
                            CGRA->PE_Array[Current_PE_ID]->Set_Mem_Port(Migration_Time+2+Current_PE_Additional_Pipeline, 3, Src_OP_ID);
                            CGRA->PE_Array[Current_PE_ID]->Set_Mem_Port(Migration_Time+2+Current_PE_Additional_Pipeline, 4, Src_OP_ID);
                            CGRA->PE_Array[Current_PE_ID]->Set_Mem_Port(Migration_Time+2+Current_PE_Additional_Pipeline, 5, Src_OP_ID);

                            //Keep the attach point which can be reused later
                            Attach_History Attach_Point;
                            Attach_Point.Attached_Time = Migration_Time+2+Current_PE_Additional_Pipeline;
                            Attach_Point.Attached_PE_ID = Current_PE_ID;
                            DFG->OP_Array[Src_OP_ID]->OP_Attach_History.push_back(Attach_Point);
                        }
                        else if(CGRA->Pipeline == LF){
                            CGRA->PE_Array[Current_PE_ID]->Reserve_Input_Port(Migration_Time+1);
                            CGRA->PE_Array[Current_PE_ID]->Set_Input_Mux(Migration_Time+1, Last_Parent_Index);
                            if(CGRA->Is_Load_PE(Current_PE_ID) || CGRA->Is_Store_PE(Current_PE_ID)){
                                CGRA->PE_Array[Current_PE_ID]->Reserve_Load_Path(Migration_Time+1);
                                CGRA->PE_Array[Current_PE_ID]->Set_Load_Mux(Migration_Time+1, 1);
                            }
                            CGRA->PE_Array[Current_PE_ID]->Reserve_WR_Port(Migration_Time+1+Current_PE_Additional_Pipeline, 1);
                            CGRA->PE_Array[Current_PE_ID]->Set_WR_Ena(Migration_Time+1+Current_PE_Additional_Pipeline, 1, 1);
                            CGRA->PE_Array[Current_PE_ID]->Set_Mem_Port(Migration_Time+1+Current_PE_Additional_Pipeline, 3, Src_OP_ID);
                            CGRA->PE_Array[Current_PE_ID]->Set_Mem_Port(Migration_Time+1+Current_PE_Additional_Pipeline, 4, Src_OP_ID);
                            CGRA->PE_Array[Current_PE_ID]->Set_Mem_Port(Migration_Time+1+Current_PE_Additional_Pipeline, 5, Src_OP_ID);

                            //Keep the attach point which can be reused later
                            Attach_History Attach_Point;
                            Attach_Point.Attached_Time = Migration_Time+1+Current_PE_Additional_Pipeline;
                            Attach_Point.Attached_PE_ID = Current_PE_ID;
                            DFG->OP_Array[Src_OP_ID]->OP_Attach_History.push_back(Attach_Point);
                        }
                        else if(CGRA->Pipeline == MF){
                            CGRA->PE_Array[Current_PE_ID]->Reserve_Input_Port(Migration_Time+1);
                            CGRA->PE_Array[Current_PE_ID]->Set_Input_Mux(Migration_Time+1, Last_Parent_Index);
                            if(CGRA->Is_Load_PE(Current_PE_ID) || CGRA->Is_Store_PE(Current_PE_ID)){
                                CGRA->PE_Array[Current_PE_ID]->Reserve_Load_Path(Migration_Time+1);
                                CGRA->PE_Array[Current_PE_ID]->Set_Load_Mux(Migration_Time+1, 1);
                            }
                            CGRA->PE_Array[Current_PE_ID]->Reserve_WR_Port(Migration_Time+1+Current_PE_Additional_Pipeline, 1);
                            CGRA->PE_Array[Current_PE_ID]->Set_WR_Ena(Migration_Time+1+Current_PE_Additional_Pipeline, 1, 1);
                            CGRA->PE_Array[Current_PE_ID]->Set_Mem_Port(Migration_Time+1+Current_PE_Additional_Pipeline, 3, Src_OP_ID);
                            CGRA->PE_Array[Current_PE_ID]->Set_Mem_Port(Migration_Time+1+Current_PE_Additional_Pipeline, 4, Src_OP_ID);
                            CGRA->PE_Array[Current_PE_ID]->Set_Mem_Port(Migration_Time+1+Current_PE_Additional_Pipeline, 5, Src_OP_ID);

                            //Keep the attach point which can be reused later
                            Attach_History Attach_Point;
                            Attach_Point.Attached_Time = Migration_Time+1+Current_PE_Additional_Pipeline;
                            Attach_Point.Attached_PE_ID = Current_PE_ID;
                            DFG->OP_Array[Src_OP_ID]->OP_Attach_History.push_back(Attach_Point);
                        }
                        else if(CGRA->Pipeline == LHF){
                            CGRA->PE_Array[Current_PE_ID]->Reserve_Input_Port(Migration_Time+1);
                            CGRA->PE_Array[Current_PE_ID]->Set_Input_Mux(Migration_Time+1, Last_Parent_Index);
                            if(CGRA->Is_Load_PE(Current_PE_ID) || CGRA->Is_Store_PE(Current_PE_ID)){
                                CGRA->PE_Array[Current_PE_ID]->Reserve_Load_Path(Migration_Time+1);
                                CGRA->PE_Array[Current_PE_ID]->Set_Load_Mux(Migration_Time+1, 1);
                            }
                            CGRA->PE_Array[Current_PE_ID]->Reserve_WR_Port(Migration_Time+2+Current_PE_Additional_Pipeline, 1);
                            CGRA->PE_Array[Current_PE_ID]->Set_WR_Ena(Migration_Time+2+Current_PE_Additional_Pipeline, 1, 1);
                            CGRA->PE_Array[Current_PE_ID]->Set_Mem_Port(Migration_Time+2+Current_PE_Additional_Pipeline, 3, Src_OP_ID);
                            CGRA->PE_Array[Current_PE_ID]->Set_Mem_Port(Migration_Time+2+Current_PE_Additional_Pipeline, 4, Src_OP_ID);
                            CGRA->PE_Array[Current_PE_ID]->Set_Mem_Port(Migration_Time+2+Current_PE_Additional_Pipeline, 5, Src_OP_ID);

                            //Keep the attach point which can be reused later
                            Attach_History Attach_Point;
                            Attach_Point.Attached_Time = Migration_Time+2+Current_PE_Additional_Pipeline;
                            Attach_Point.Attached_PE_ID = Current_PE_ID;
                            DFG->OP_Array[Src_OP_ID]->OP_Attach_History.push_back(Attach_Point);
                        }
                        else if(CGRA->Pipeline == HF){
                            CGRA->PE_Array[Current_PE_ID]->Reserve_Input_Port(Migration_Time+1);
                            CGRA->PE_Array[Current_PE_ID]->Set_Input_Mux(Migration_Time+1, Last_Parent_Index);
                            if(CGRA->Is_Load_PE(Current_PE_ID) || CGRA->Is_Store_PE(Current_PE_ID)){
                                CGRA->PE_Array[Current_PE_ID]->Reserve_Load_Path(Migration_Time+2);
                                CGRA->PE_Array[Current_PE_ID]->Set_Load_Mux(Migration_Time+2, 1);
                            }
                            CGRA->PE_Array[Current_PE_ID]->Reserve_WR_Port(Migration_Time+2+Current_PE_Additional_Pipeline, 1);
                            CGRA->PE_Array[Current_PE_ID]->Set_WR_Ena(Migration_Time+2+Current_PE_Additional_Pipeline, 1, 1);
                            CGRA->PE_Array[Current_PE_ID]->Set_Mem_Port(Migration_Time+2+Current_PE_Additional_Pipeline, 3, Src_OP_ID);
                            CGRA->PE_Array[Current_PE_ID]->Set_Mem_Port(Migration_Time+2+Current_PE_Additional_Pipeline, 4, Src_OP_ID);
                            CGRA->PE_Array[Current_PE_ID]->Set_Mem_Port(Migration_Time+2+Current_PE_Additional_Pipeline, 5, Src_OP_ID);

                            //Keep the attach point which can be reused later
                            Attach_History Attach_Point;
                            Attach_Point.Attached_Time = Migration_Time+2+Current_PE_Additional_Pipeline;
                            Attach_Point.Attached_PE_ID = Current_PE_ID;
                            DFG->OP_Array[Src_OP_ID]->OP_Attach_History.push_back(Attach_Point);
                        }
                        else{
                            ERROR("Unknown pipeline intensity setup!\n");
                        }


                        if(GL_Var::Print_Level>10){
                            GL_Var::fTrace << "Store operation " << Src_OP_ID << " from PE " << Last_Parent_Index << " in " << " PE " << Current_PE_ID << " at time " << Migration_Time << std::endl;
                        }
                    }

                    //Move the data from data memory to next PE
                    while(true){
                        bool Current_PE_Data_Mem_RD_Avail0;
                        bool Current_PE_Data_Mem_RD_Avail1;
                        bool Current_PE_Data_Mem_RD_Avail2;
                        bool Current_PE_Data_Mem_WR_Avail0;
                        bool Current_PE_Data_Mem_RD_Avail;
                        bool Current_PE_Output_Avail;
                        bool Next_PE_Input_Avail;
                        if(CGRA->Pipeline == OLD){
                            Current_PE_Data_Mem_RD_Avail0 = CGRA->PE_Array[Current_PE_ID]->Is_RD_Port_Avail(Migration_Time+3+Current_PE_Additional_Pipeline, 0);
                            Current_PE_Data_Mem_RD_Avail1 = CGRA->PE_Array[Current_PE_ID]->Is_RD_Port_Avail(Migration_Time+3+Current_PE_Additional_Pipeline, 1);
                            Current_PE_Data_Mem_RD_Avail2 = CGRA->PE_Array[Current_PE_ID]->Is_RD_Port_Avail(Migration_Time+3+Current_PE_Additional_Pipeline, 2);
                            Current_PE_Data_Mem_WR_Avail0 = CGRA->PE_Array[Current_PE_ID]->Is_WR_Port_Avail(Migration_Time+3+Current_PE_Additional_Pipeline, 0);
                            Current_PE_Data_Mem_RD_Avail = (Current_PE_Data_Mem_RD_Avail0 || Current_PE_Data_Mem_RD_Avail1 || Current_PE_Data_Mem_RD_Avail2) && Current_PE_Data_Mem_WR_Avail0;
                            Current_PE_Output_Avail = CGRA->PE_Array[Current_PE_ID]->Is_Output_Port_Avail(Migration_Time+5+Current_PE_Additional_Pipeline, Child_Index);
                            Next_PE_Input_Avail = CGRA->PE_Array[Next_PE_ID]->Is_Input_Port_Avail(Migration_Time+7+Current_PE_Additional_Pipeline);
                        }
                        else if(CGRA->Pipeline == LF){
                            Current_PE_Data_Mem_RD_Avail0 = CGRA->PE_Array[Current_PE_ID]->Is_RD_Port_Avail(Migration_Time+2+Current_PE_Additional_Pipeline, 0);
                            Current_PE_Data_Mem_RD_Avail1 = CGRA->PE_Array[Current_PE_ID]->Is_RD_Port_Avail(Migration_Time+2+Current_PE_Additional_Pipeline, 1);
                            Current_PE_Data_Mem_RD_Avail2 = CGRA->PE_Array[Current_PE_ID]->Is_RD_Port_Avail(Migration_Time+2+Current_PE_Additional_Pipeline, 2);
                            Current_PE_Data_Mem_WR_Avail0 = CGRA->PE_Array[Current_PE_ID]->Is_WR_Port_Avail(Migration_Time+2+Current_PE_Additional_Pipeline, 0);
                            Current_PE_Data_Mem_RD_Avail = (Current_PE_Data_Mem_RD_Avail0 || Current_PE_Data_Mem_RD_Avail1 || Current_PE_Data_Mem_RD_Avail2) && Current_PE_Data_Mem_WR_Avail0;
                            Current_PE_Output_Avail = CGRA->PE_Array[Current_PE_ID]->Is_Output_Port_Avail(Migration_Time+3+Current_PE_Additional_Pipeline, Child_Index);
                            Next_PE_Input_Avail = CGRA->PE_Array[Next_PE_ID]->Is_Input_Port_Avail(Migration_Time+4+Current_PE_Additional_Pipeline);
                        }
                        else if(CGRA->Pipeline == MF){
                            Current_PE_Data_Mem_RD_Avail0 = CGRA->PE_Array[Current_PE_ID]->Is_RD_Port_Avail(Migration_Time+2+Current_PE_Additional_Pipeline, 0);
                            Current_PE_Data_Mem_RD_Avail1 = CGRA->PE_Array[Current_PE_ID]->Is_RD_Port_Avail(Migration_Time+2+Current_PE_Additional_Pipeline, 1);
                            Current_PE_Data_Mem_RD_Avail2 = CGRA->PE_Array[Current_PE_ID]->Is_RD_Port_Avail(Migration_Time+2+Current_PE_Additional_Pipeline, 2);
                            Current_PE_Data_Mem_WR_Avail0 = CGRA->PE_Array[Current_PE_ID]->Is_WR_Port_Avail(Migration_Time+2+Current_PE_Additional_Pipeline, 0);
                            Current_PE_Data_Mem_RD_Avail = (Current_PE_Data_Mem_RD_Avail0 || Current_PE_Data_Mem_RD_Avail1 || Current_PE_Data_Mem_RD_Avail2) && Current_PE_Data_Mem_WR_Avail0;
                            Current_PE_Output_Avail = CGRA->PE_Array[Current_PE_ID]->Is_Output_Port_Avail(Migration_Time+3+Current_PE_Additional_Pipeline, Child_Index);
                            Next_PE_Input_Avail = CGRA->PE_Array[Next_PE_ID]->Is_Input_Port_Avail(Migration_Time+4+Current_PE_Additional_Pipeline);
                        }
                        else if(CGRA->Pipeline == LHF){
                            Current_PE_Data_Mem_RD_Avail0 = CGRA->PE_Array[Current_PE_ID]->Is_RD_Port_Avail(Migration_Time+3+Current_PE_Additional_Pipeline, 0);
                            Current_PE_Data_Mem_RD_Avail1 = CGRA->PE_Array[Current_PE_ID]->Is_RD_Port_Avail(Migration_Time+3+Current_PE_Additional_Pipeline, 1);
                            Current_PE_Data_Mem_RD_Avail2 = CGRA->PE_Array[Current_PE_ID]->Is_RD_Port_Avail(Migration_Time+3+Current_PE_Additional_Pipeline, 2);
                            Current_PE_Data_Mem_WR_Avail0 = CGRA->PE_Array[Current_PE_ID]->Is_WR_Port_Avail(Migration_Time+3+Current_PE_Additional_Pipeline, 0);
                            Current_PE_Data_Mem_RD_Avail = (Current_PE_Data_Mem_RD_Avail0 || Current_PE_Data_Mem_RD_Avail1 || Current_PE_Data_Mem_RD_Avail2) && Current_PE_Data_Mem_WR_Avail0;
                            Current_PE_Output_Avail = CGRA->PE_Array[Current_PE_ID]->Is_Output_Port_Avail(Migration_Time+4+Current_PE_Additional_Pipeline, Child_Index);
                            Next_PE_Input_Avail = CGRA->PE_Array[Next_PE_ID]->Is_Input_Port_Avail(Migration_Time+6+Current_PE_Additional_Pipeline);
                        }
                        else if(CGRA->Pipeline == HF){
                            Current_PE_Data_Mem_RD_Avail0 = CGRA->PE_Array[Current_PE_ID]->Is_RD_Port_Avail(Migration_Time+3+Current_PE_Additional_Pipeline, 0);
                            Current_PE_Data_Mem_RD_Avail1 = CGRA->PE_Array[Current_PE_ID]->Is_RD_Port_Avail(Migration_Time+3+Current_PE_Additional_Pipeline, 1);
                            Current_PE_Data_Mem_RD_Avail2 = CGRA->PE_Array[Current_PE_ID]->Is_RD_Port_Avail(Migration_Time+3+Current_PE_Additional_Pipeline, 2);
                            Current_PE_Data_Mem_WR_Avail0 = CGRA->PE_Array[Current_PE_ID]->Is_WR_Port_Avail(Migration_Time+3+Current_PE_Additional_Pipeline, 0);
                            Current_PE_Data_Mem_RD_Avail = (Current_PE_Data_Mem_RD_Avail0 || Current_PE_Data_Mem_RD_Avail1 || Current_PE_Data_Mem_RD_Avail2) && Current_PE_Data_Mem_WR_Avail0;
                            Current_PE_Output_Avail = CGRA->PE_Array[Current_PE_ID]->Is_Output_Port_Avail(Migration_Time+7+Current_PE_Additional_Pipeline, Child_Index);
                            Next_PE_Input_Avail = CGRA->PE_Array[Next_PE_ID]->Is_Input_Port_Avail(Migration_Time+9+Current_PE_Additional_Pipeline);
                        }
                        else{
                            ERROR("Unknown pipeline intensity setup!\n");
                        }
                        
                        bool Next_Load_Path_Avail = true;
                        if(CGRA->Is_Load_PE(Next_PE_ID) || CGRA->Is_Store_PE(Next_PE_ID)){
                            if(CGRA->Pipeline == OLD){
                                Next_Load_Path_Avail = CGRA->PE_Array[Next_PE_ID]->Is_Load_Path_Avail(Migration_Time+8+Current_PE_Additional_Pipeline);
                            }
                            else if(CGRA->Pipeline == LF){
                                Next_Load_Path_Avail = CGRA->PE_Array[Next_PE_ID]->Is_Load_Path_Avail(Migration_Time+4+Current_PE_Additional_Pipeline);
                            }
                            else if(CGRA->Pipeline == MF){
                                Next_Load_Path_Avail = CGRA->PE_Array[Next_PE_ID]->Is_Load_Path_Avail(Migration_Time+4+Current_PE_Additional_Pipeline);
                            }
                            else if(CGRA->Pipeline == LHF){
                                Next_Load_Path_Avail = CGRA->PE_Array[Next_PE_ID]->Is_Load_Path_Avail(Migration_Time+6+Current_PE_Additional_Pipeline);
                            }
                            else if(CGRA->Pipeline == HF){
                                Next_Load_Path_Avail = CGRA->PE_Array[Next_PE_ID]->Is_Load_Path_Avail(Migration_Time+10+Current_PE_Additional_Pipeline);
                            }
                            else{
                                ERROR("Unknown pipeline intensity setup!\n");
                            }
                        }

                        bool Next_PE_Data_Mem_WR_Avail1;
                        bool Next_PE_Data_Mem_RD_Avail3;
                        bool Next_PE_Data_Mem_RD_Avail4;
                        bool Next_PE_Data_Mem_RD_Avail5;
                        if(CGRA->Pipeline == OLD){
                            Next_PE_Data_Mem_WR_Avail1 = CGRA->PE_Array[Next_PE_ID]->Is_WR_Port_Avail(Migration_Time+8+Current_PE_Additional_Pipeline+Next_PE_Additional_Pipeline, 1);
                            Next_PE_Data_Mem_RD_Avail3 = CGRA->PE_Array[Next_PE_ID]->Is_RD_Port_Avail(Migration_Time+8+Current_PE_Additional_Pipeline+Next_PE_Additional_Pipeline, 3);
                            Next_PE_Data_Mem_RD_Avail4 = CGRA->PE_Array[Next_PE_ID]->Is_RD_Port_Avail(Migration_Time+8+Current_PE_Additional_Pipeline+Next_PE_Additional_Pipeline, 4);
                            Next_PE_Data_Mem_RD_Avail5 = CGRA->PE_Array[Next_PE_ID]->Is_RD_Port_Avail(Migration_Time+8+Current_PE_Additional_Pipeline+Next_PE_Additional_Pipeline, 5);
                        }
                        else if(CGRA->Pipeline == LF){
                            Next_PE_Data_Mem_WR_Avail1 = CGRA->PE_Array[Next_PE_ID]->Is_WR_Port_Avail(Migration_Time+4+Current_PE_Additional_Pipeline+Next_PE_Additional_Pipeline, 1);
                            Next_PE_Data_Mem_RD_Avail3 = CGRA->PE_Array[Next_PE_ID]->Is_RD_Port_Avail(Migration_Time+4+Current_PE_Additional_Pipeline+Next_PE_Additional_Pipeline, 3);
                            Next_PE_Data_Mem_RD_Avail4 = CGRA->PE_Array[Next_PE_ID]->Is_RD_Port_Avail(Migration_Time+4+Current_PE_Additional_Pipeline+Next_PE_Additional_Pipeline, 4);
                            Next_PE_Data_Mem_RD_Avail5 = CGRA->PE_Array[Next_PE_ID]->Is_RD_Port_Avail(Migration_Time+4+Current_PE_Additional_Pipeline+Next_PE_Additional_Pipeline, 5);
                        }
                        else if(CGRA->Pipeline == MF){
                            Next_PE_Data_Mem_WR_Avail1 = CGRA->PE_Array[Next_PE_ID]->Is_WR_Port_Avail(Migration_Time+4+Current_PE_Additional_Pipeline+Next_PE_Additional_Pipeline, 1);
                            Next_PE_Data_Mem_RD_Avail3 = CGRA->PE_Array[Next_PE_ID]->Is_RD_Port_Avail(Migration_Time+4+Current_PE_Additional_Pipeline+Next_PE_Additional_Pipeline, 3);
                            Next_PE_Data_Mem_RD_Avail4 = CGRA->PE_Array[Next_PE_ID]->Is_RD_Port_Avail(Migration_Time+4+Current_PE_Additional_Pipeline+Next_PE_Additional_Pipeline, 4);
                            Next_PE_Data_Mem_RD_Avail5 = CGRA->PE_Array[Next_PE_ID]->Is_RD_Port_Avail(Migration_Time+4+Current_PE_Additional_Pipeline+Next_PE_Additional_Pipeline, 5);
                        }
                        else if(CGRA->Pipeline == LHF){
                            Next_PE_Data_Mem_WR_Avail1 = CGRA->PE_Array[Next_PE_ID]->Is_WR_Port_Avail(Migration_Time+7+Current_PE_Additional_Pipeline+Next_PE_Additional_Pipeline, 1);
                            Next_PE_Data_Mem_RD_Avail3 = CGRA->PE_Array[Next_PE_ID]->Is_RD_Port_Avail(Migration_Time+7+Current_PE_Additional_Pipeline+Next_PE_Additional_Pipeline, 3);
                            Next_PE_Data_Mem_RD_Avail4 = CGRA->PE_Array[Next_PE_ID]->Is_RD_Port_Avail(Migration_Time+7+Current_PE_Additional_Pipeline+Next_PE_Additional_Pipeline, 4);
                            Next_PE_Data_Mem_RD_Avail5 = CGRA->PE_Array[Next_PE_ID]->Is_RD_Port_Avail(Migration_Time+7+Current_PE_Additional_Pipeline+Next_PE_Additional_Pipeline, 5);
                        }
                        else if(CGRA->Pipeline == HF){
                            Next_PE_Data_Mem_WR_Avail1 = CGRA->PE_Array[Next_PE_ID]->Is_WR_Port_Avail(Migration_Time+10+Current_PE_Additional_Pipeline+Next_PE_Additional_Pipeline, 1);
                            Next_PE_Data_Mem_RD_Avail3 = CGRA->PE_Array[Next_PE_ID]->Is_RD_Port_Avail(Migration_Time+10+Current_PE_Additional_Pipeline+Next_PE_Additional_Pipeline, 3);
                            Next_PE_Data_Mem_RD_Avail4 = CGRA->PE_Array[Next_PE_ID]->Is_RD_Port_Avail(Migration_Time+10+Current_PE_Additional_Pipeline+Next_PE_Additional_Pipeline, 4);
                            Next_PE_Data_Mem_RD_Avail5 = CGRA->PE_Array[Next_PE_ID]->Is_RD_Port_Avail(Migration_Time+10+Current_PE_Additional_Pipeline+Next_PE_Additional_Pipeline, 5);
                        }
                        else{
                            ERROR("Unknown pipeline intensity setup!\n");
                        }

                        bool Next_PE_Data_Mem_WR_Avail = Next_PE_Data_Mem_WR_Avail1 && Next_PE_Data_Mem_RD_Avail3 && Next_PE_Data_Mem_RD_Avail4 && Next_PE_Data_Mem_RD_Avail5;
                        if(Current_PE_Data_Mem_RD_Avail && Current_PE_Output_Avail && Next_PE_Input_Avail && Next_PE_Data_Mem_WR_Avail && Next_Load_Path_Avail){
                            if(Mode==Impl){
                                if(CGRA->Pipeline == OLD){
                                    if(Current_PE_Data_Mem_RD_Avail0){
                                        CGRA->PE_Array[Current_PE_ID]->Reserve_RD_Port(Migration_Time+3+Current_PE_Additional_Pipeline, 0);
                                        CGRA->PE_Array[Current_PE_ID]->Set_Mem_Port(Migration_Time+3+Current_PE_Additional_Pipeline, 0, Src_OP_ID);
                                        CGRA->PE_Array[Current_PE_ID]->Set_Output_Mux(Migration_Time+5+Current_PE_Additional_Pipeline, Child_Index, 0);
                                    }
                                    else if(Current_PE_Data_Mem_RD_Avail1){
                                        CGRA->PE_Array[Current_PE_ID]->Reserve_RD_Port(Migration_Time+3+Current_PE_Additional_Pipeline, 1);
                                        CGRA->PE_Array[Current_PE_ID]->Set_Mem_Port(Migration_Time+3+Current_PE_Additional_Pipeline, 1, Src_OP_ID);
                                        CGRA->PE_Array[Current_PE_ID]->Set_Output_Mux(Migration_Time+5+Current_PE_Additional_Pipeline, Child_Index, 1);
                                    }
                                    else{ 
                                        CGRA->PE_Array[Current_PE_ID]->Reserve_RD_Port(Migration_Time+3+Current_PE_Additional_Pipeline, 2);
                                        CGRA->PE_Array[Current_PE_ID]->Set_Mem_Port(Migration_Time+3+Current_PE_Additional_Pipeline, 2, Src_OP_ID);
                                        CGRA->PE_Array[Current_PE_ID]->Set_Output_Mux(Migration_Time+5+Current_PE_Additional_Pipeline, Child_Index, 2);
                                    }

                                    CGRA->PE_Array[Current_PE_ID]->Reserve_Output_Port(Migration_Time+5+Current_PE_Additional_Pipeline, Child_Index);
                                    CGRA->PE_Array[Current_PE_ID]->Set_WR_Ena(Migration_Time+3+Current_PE_Additional_Pipeline, 0, 0);

                                    if(GL_Var::Print_Level>10){
                                        GL_Var::fTrace<< " Move operation " << Src_OP_ID << " forward: from " << " PE " << Current_PE_ID << " to " << " PE " << Next_PE_ID << " at time " << Migration_Time+4+Current_PE_Additional_Pipeline << std::endl;
                                    }
                                }
                                else if(CGRA->Pipeline == LF){
                                    if(Current_PE_Data_Mem_RD_Avail0){
                                        CGRA->PE_Array[Current_PE_ID]->Reserve_RD_Port(Migration_Time+2+Current_PE_Additional_Pipeline, 0);
                                        CGRA->PE_Array[Current_PE_ID]->Set_Mem_Port(Migration_Time+2+Current_PE_Additional_Pipeline, 0, Src_OP_ID);
                                        CGRA->PE_Array[Current_PE_ID]->Set_Output_Mux(Migration_Time+3+Current_PE_Additional_Pipeline, Child_Index, 0);
                                    }
                                    else if(Current_PE_Data_Mem_RD_Avail1){
                                        CGRA->PE_Array[Current_PE_ID]->Reserve_RD_Port(Migration_Time+2+Current_PE_Additional_Pipeline, 1);
                                        CGRA->PE_Array[Current_PE_ID]->Set_Mem_Port(Migration_Time+2+Current_PE_Additional_Pipeline, 1, Src_OP_ID);
                                        CGRA->PE_Array[Current_PE_ID]->Set_Output_Mux(Migration_Time+3+Current_PE_Additional_Pipeline, Child_Index, 1);
                                    }
                                    else{ 
                                        CGRA->PE_Array[Current_PE_ID]->Reserve_RD_Port(Migration_Time+2+Current_PE_Additional_Pipeline, 2);
                                        CGRA->PE_Array[Current_PE_ID]->Set_Mem_Port(Migration_Time+2+Current_PE_Additional_Pipeline, 2, Src_OP_ID);
                                        CGRA->PE_Array[Current_PE_ID]->Set_Output_Mux(Migration_Time+3+Current_PE_Additional_Pipeline, Child_Index, 2);
                                    }

                                    CGRA->PE_Array[Current_PE_ID]->Reserve_Output_Port(Migration_Time+3+Current_PE_Additional_Pipeline, Child_Index);
                                    CGRA->PE_Array[Current_PE_ID]->Set_WR_Ena(Migration_Time+2+Current_PE_Additional_Pipeline, 0, 0);

                                    if(GL_Var::Print_Level>10){
                                        GL_Var::fTrace<< " Move operation " << Src_OP_ID << " forward: from " << " PE " << Current_PE_ID << " to " << " PE " << Next_PE_ID << " at time " << Migration_Time+3+Current_PE_Additional_Pipeline << std::endl;
                                    }
                                }
                                else if(CGRA->Pipeline == MF){
                                    if(Current_PE_Data_Mem_RD_Avail0){
                                        CGRA->PE_Array[Current_PE_ID]->Reserve_RD_Port(Migration_Time+2+Current_PE_Additional_Pipeline, 0);
                                        CGRA->PE_Array[Current_PE_ID]->Set_Mem_Port(Migration_Time+2+Current_PE_Additional_Pipeline, 0, Src_OP_ID);
                                        CGRA->PE_Array[Current_PE_ID]->Set_Output_Mux(Migration_Time+3+Current_PE_Additional_Pipeline, Child_Index, 0);
                                    }
                                    else if(Current_PE_Data_Mem_RD_Avail1){
                                        CGRA->PE_Array[Current_PE_ID]->Reserve_RD_Port(Migration_Time+2+Current_PE_Additional_Pipeline, 1);
                                        CGRA->PE_Array[Current_PE_ID]->Set_Mem_Port(Migration_Time+2+Current_PE_Additional_Pipeline, 1, Src_OP_ID);
                                        CGRA->PE_Array[Current_PE_ID]->Set_Output_Mux(Migration_Time+3+Current_PE_Additional_Pipeline, Child_Index, 1);
                                    }
                                    else{ 
                                        CGRA->PE_Array[Current_PE_ID]->Reserve_RD_Port(Migration_Time+2+Current_PE_Additional_Pipeline, 2);
                                        CGRA->PE_Array[Current_PE_ID]->Set_Mem_Port(Migration_Time+2+Current_PE_Additional_Pipeline, 2, Src_OP_ID);
                                        CGRA->PE_Array[Current_PE_ID]->Set_Output_Mux(Migration_Time+3+Current_PE_Additional_Pipeline, Child_Index, 2);
                                    }

                                    CGRA->PE_Array[Current_PE_ID]->Reserve_Output_Port(Migration_Time+3+Current_PE_Additional_Pipeline, Child_Index);
                                    CGRA->PE_Array[Current_PE_ID]->Set_WR_Ena(Migration_Time+2+Current_PE_Additional_Pipeline, 0, 0);

                                    if(GL_Var::Print_Level>10){
                                        GL_Var::fTrace<< " Move operation " << Src_OP_ID << " forward: from " << " PE " << Current_PE_ID << " to " << " PE " << Next_PE_ID << " at time " << Migration_Time+3+Current_PE_Additional_Pipeline << std::endl;
                                    }
                                }
                                else if(CGRA->Pipeline == LHF){
                                    if(Current_PE_Data_Mem_RD_Avail0){
                                        CGRA->PE_Array[Current_PE_ID]->Reserve_RD_Port(Migration_Time+3+Current_PE_Additional_Pipeline, 0);
                                        CGRA->PE_Array[Current_PE_ID]->Set_Mem_Port(Migration_Time+3+Current_PE_Additional_Pipeline, 0, Src_OP_ID);
                                        CGRA->PE_Array[Current_PE_ID]->Set_Output_Mux(Migration_Time+4+Current_PE_Additional_Pipeline, Child_Index, 0);
                                    }
                                    else if(Current_PE_Data_Mem_RD_Avail1){
                                        CGRA->PE_Array[Current_PE_ID]->Reserve_RD_Port(Migration_Time+3+Current_PE_Additional_Pipeline, 1);
                                        CGRA->PE_Array[Current_PE_ID]->Set_Mem_Port(Migration_Time+3+Current_PE_Additional_Pipeline, 1, Src_OP_ID);
                                        CGRA->PE_Array[Current_PE_ID]->Set_Output_Mux(Migration_Time+4+Current_PE_Additional_Pipeline, Child_Index, 1);
                                    }
                                    else{ 
                                        CGRA->PE_Array[Current_PE_ID]->Reserve_RD_Port(Migration_Time+3+Current_PE_Additional_Pipeline, 2);
                                        CGRA->PE_Array[Current_PE_ID]->Set_Mem_Port(Migration_Time+3+Current_PE_Additional_Pipeline, 2, Src_OP_ID);
                                        CGRA->PE_Array[Current_PE_ID]->Set_Output_Mux(Migration_Time+4+Current_PE_Additional_Pipeline, Child_Index, 2);
                                    }

                                    CGRA->PE_Array[Current_PE_ID]->Reserve_Output_Port(Migration_Time+4+Current_PE_Additional_Pipeline, Child_Index);
                                    CGRA->PE_Array[Current_PE_ID]->Set_WR_Ena(Migration_Time+3+Current_PE_Additional_Pipeline, 0, 0);

                                    if(GL_Var::Print_Level>10){
                                        GL_Var::fTrace<< " Move operation " << Src_OP_ID << " forward: from " << " PE " << Current_PE_ID << " to " << " PE " << Next_PE_ID << " at time " << Migration_Time+4+Current_PE_Additional_Pipeline << std::endl;
                                    }
                                }
                                else if(CGRA->Pipeline == HF){
                                    if(Current_PE_Data_Mem_RD_Avail0){
                                        CGRA->PE_Array[Current_PE_ID]->Reserve_RD_Port(Migration_Time+3+Current_PE_Additional_Pipeline, 0);
                                        CGRA->PE_Array[Current_PE_ID]->Set_Mem_Port(Migration_Time+3+Current_PE_Additional_Pipeline, 0, Src_OP_ID);
                                        CGRA->PE_Array[Current_PE_ID]->Set_Output_Mux(Migration_Time+7+Current_PE_Additional_Pipeline, Child_Index, 0);
                                    }
                                    else if(Current_PE_Data_Mem_RD_Avail1){
                                        CGRA->PE_Array[Current_PE_ID]->Reserve_RD_Port(Migration_Time+3+Current_PE_Additional_Pipeline, 1);
                                        CGRA->PE_Array[Current_PE_ID]->Set_Mem_Port(Migration_Time+3+Current_PE_Additional_Pipeline, 1, Src_OP_ID);
                                        CGRA->PE_Array[Current_PE_ID]->Set_Output_Mux(Migration_Time+7+Current_PE_Additional_Pipeline, Child_Index, 1);
                                    }
                                    else{ 
                                        CGRA->PE_Array[Current_PE_ID]->Reserve_RD_Port(Migration_Time+3+Current_PE_Additional_Pipeline, 2);
                                        CGRA->PE_Array[Current_PE_ID]->Set_Mem_Port(Migration_Time+3+Current_PE_Additional_Pipeline, 2, Src_OP_ID);
                                        CGRA->PE_Array[Current_PE_ID]->Set_Output_Mux(Migration_Time+7+Current_PE_Additional_Pipeline, Child_Index, 2);
                                    }

                                    CGRA->PE_Array[Current_PE_ID]->Reserve_Output_Port(Migration_Time+7+Current_PE_Additional_Pipeline, Child_Index);
                                    CGRA->PE_Array[Current_PE_ID]->Set_WR_Ena(Migration_Time+3+Current_PE_Additional_Pipeline, 0, 0);

                                    if(GL_Var::Print_Level>10){
                                        GL_Var::fTrace<< " Move operation " << Src_OP_ID << " forward: from " << " PE " << Current_PE_ID << " to " << " PE " << Next_PE_ID << " at time " << Migration_Time+4+Current_PE_Additional_Pipeline << std::endl;
                                    }
                                }
                                else{
                                    ERROR("Unknown pipeline intensity setup!\n");
                                }
                            }


                            if(CGRA->Pipeline == OLD){
                                Migration_Time = Migration_Time+6+Current_PE_Additional_Pipeline;
                            }
                            else if(CGRA->Pipeline == LF){
                                Migration_Time = Migration_Time+3+Current_PE_Additional_Pipeline;
                            }
                            else if(CGRA->Pipeline == MF){
                                Migration_Time = Migration_Time+3+Current_PE_Additional_Pipeline;
                            }
                            else if(CGRA->Pipeline == LHF){
                                Migration_Time = Migration_Time+5+Current_PE_Additional_Pipeline;
                            }
                            else if(CGRA->Pipeline == HF){
                                Migration_Time = Migration_Time+8+Current_PE_Additional_Pipeline;
                            }
                            else{
                                ERROR("Unknown pipeline intensity setup!\n");
                            }

                            i++;
                            break;
                        }
                        else{
                            Migration_Time++;
                        }
                    }
                }
            }

            //Arrive in last PE data memory
            else{
                if(Mode == Impl){
                    if(CGRA->Pipeline == OLD){
                        CGRA->PE_Array[Current_PE_ID]->Reserve_Input_Port(Migration_Time+1);
                        CGRA->PE_Array[Current_PE_ID]->Set_Input_Mux(Migration_Time+1, Last_Parent_Index);
                        if(CGRA->Is_Load_PE(Current_PE_ID) || CGRA->Is_Store_PE(Current_PE_ID)){
                            CGRA->PE_Array[Current_PE_ID]->Reserve_Load_Path(Migration_Time+2);
                            CGRA->PE_Array[Current_PE_ID]->Set_Load_Mux(Migration_Time+2, 1);
                        }
                        CGRA->PE_Array[Current_PE_ID]->Reserve_WR_Port(Migration_Time+2+Current_PE_Additional_Pipeline, 1);
                        CGRA->PE_Array[Current_PE_ID]->Set_WR_Ena(Migration_Time+2+Current_PE_Additional_Pipeline, 1, 1);
                        CGRA->PE_Array[Current_PE_ID]->Set_Mem_Port(Migration_Time+2+Current_PE_Additional_Pipeline, 3, Src_OP_ID);
                        CGRA->PE_Array[Current_PE_ID]->Set_Mem_Port(Migration_Time+2+Current_PE_Additional_Pipeline, 4, Src_OP_ID);
                        CGRA->PE_Array[Current_PE_ID]->Set_Mem_Port(Migration_Time+2+Current_PE_Additional_Pipeline, 5, Src_OP_ID);

                        if(GL_Var::Print_Level>10){
                            GL_Var::fTrace << "Move operation " << Src_OP_ID << " last: from " << " PE " << Last_PE_ID << " to PE " << Current_PE_ID << " at time " << Migration_Time << std::endl;
                        }

                        //Keep the attach point which can be reused later
                        Attach_History Attach_Point;
                        Attach_Point.Attached_Time = Migration_Time + 2 + Current_PE_Additional_Pipeline;
                        Attach_Point.Attached_PE_ID = Current_PE_ID;
                        DFG->OP_Array[Src_OP_ID]->OP_Attach_History.push_back(Attach_Point);

                    }
                    else if(CGRA->Pipeline == LF){
                        CGRA->PE_Array[Current_PE_ID]->Reserve_Input_Port(Migration_Time+1);
                        CGRA->PE_Array[Current_PE_ID]->Set_Input_Mux(Migration_Time+1, Last_Parent_Index);
                        if(CGRA->Is_Load_PE(Current_PE_ID) || CGRA->Is_Store_PE(Current_PE_ID)){
                            CGRA->PE_Array[Current_PE_ID]->Reserve_Load_Path(Migration_Time+1);
                            CGRA->PE_Array[Current_PE_ID]->Set_Load_Mux(Migration_Time+1, 1);
                        }
                        CGRA->PE_Array[Current_PE_ID]->Reserve_WR_Port(Migration_Time+1+Current_PE_Additional_Pipeline, 1);
                        CGRA->PE_Array[Current_PE_ID]->Set_WR_Ena(Migration_Time+1+Current_PE_Additional_Pipeline, 1, 1);
                        CGRA->PE_Array[Current_PE_ID]->Set_Mem_Port(Migration_Time+1+Current_PE_Additional_Pipeline, 3, Src_OP_ID);
                        CGRA->PE_Array[Current_PE_ID]->Set_Mem_Port(Migration_Time+1+Current_PE_Additional_Pipeline, 4, Src_OP_ID);
                        CGRA->PE_Array[Current_PE_ID]->Set_Mem_Port(Migration_Time+1+Current_PE_Additional_Pipeline, 5, Src_OP_ID);

                        if(GL_Var::Print_Level>10){
                            GL_Var::fTrace << "Move operation " << Src_OP_ID << " last: from " << " PE " << Last_PE_ID << " to PE " << Current_PE_ID << " at time " << Migration_Time << std::endl;
                        }

                        //Keep the attach point which can be reused later
                        Attach_History Attach_Point;
                        Attach_Point.Attached_Time = Migration_Time + 1 + Current_PE_Additional_Pipeline;
                        Attach_Point.Attached_PE_ID = Current_PE_ID;
                        DFG->OP_Array[Src_OP_ID]->OP_Attach_History.push_back(Attach_Point);

                    }
                    else if(CGRA->Pipeline == MF){
                        CGRA->PE_Array[Current_PE_ID]->Reserve_Input_Port(Migration_Time+1);
                        CGRA->PE_Array[Current_PE_ID]->Set_Input_Mux(Migration_Time+1, Last_Parent_Index);
                        if(CGRA->Is_Load_PE(Current_PE_ID) || CGRA->Is_Store_PE(Current_PE_ID)){
                            CGRA->PE_Array[Current_PE_ID]->Reserve_Load_Path(Migration_Time+1);
                            CGRA->PE_Array[Current_PE_ID]->Set_Load_Mux(Migration_Time+1, 1);
                        }
                        CGRA->PE_Array[Current_PE_ID]->Reserve_WR_Port(Migration_Time+1+Current_PE_Additional_Pipeline, 1);
                        CGRA->PE_Array[Current_PE_ID]->Set_WR_Ena(Migration_Time+1+Current_PE_Additional_Pipeline, 1, 1);
                        CGRA->PE_Array[Current_PE_ID]->Set_Mem_Port(Migration_Time+1+Current_PE_Additional_Pipeline, 3, Src_OP_ID);
                        CGRA->PE_Array[Current_PE_ID]->Set_Mem_Port(Migration_Time+1+Current_PE_Additional_Pipeline, 4, Src_OP_ID);
                        CGRA->PE_Array[Current_PE_ID]->Set_Mem_Port(Migration_Time+1+Current_PE_Additional_Pipeline, 5, Src_OP_ID);

                        if(GL_Var::Print_Level>10){
                            GL_Var::fTrace << "Move operation " << Src_OP_ID << " last: from " << " PE " << Last_PE_ID << " to PE " << Current_PE_ID << " at time " << Migration_Time << std::endl;
                        }

                        //Keep the attach point which can be reused later
                        Attach_History Attach_Point;
                        Attach_Point.Attached_Time = Migration_Time + 1 + Current_PE_Additional_Pipeline;
                        Attach_Point.Attached_PE_ID = Current_PE_ID;
                        DFG->OP_Array[Src_OP_ID]->OP_Attach_History.push_back(Attach_Point);

                    }
                    else if(CGRA->Pipeline == LHF){
                        CGRA->PE_Array[Current_PE_ID]->Reserve_Input_Port(Migration_Time+1);
                        CGRA->PE_Array[Current_PE_ID]->Set_Input_Mux(Migration_Time+1, Last_Parent_Index);
                        if(CGRA->Is_Load_PE(Current_PE_ID) || CGRA->Is_Store_PE(Current_PE_ID)){
                            CGRA->PE_Array[Current_PE_ID]->Reserve_Load_Path(Migration_Time+1);
                            CGRA->PE_Array[Current_PE_ID]->Set_Load_Mux(Migration_Time+1, 1);
                        }
                        CGRA->PE_Array[Current_PE_ID]->Reserve_WR_Port(Migration_Time+2+Current_PE_Additional_Pipeline, 1);
                        CGRA->PE_Array[Current_PE_ID]->Set_WR_Ena(Migration_Time+2+Current_PE_Additional_Pipeline, 1, 1);
                        CGRA->PE_Array[Current_PE_ID]->Set_Mem_Port(Migration_Time+2+Current_PE_Additional_Pipeline, 3, Src_OP_ID);
                        CGRA->PE_Array[Current_PE_ID]->Set_Mem_Port(Migration_Time+2+Current_PE_Additional_Pipeline, 4, Src_OP_ID);
                        CGRA->PE_Array[Current_PE_ID]->Set_Mem_Port(Migration_Time+2+Current_PE_Additional_Pipeline, 5, Src_OP_ID);

                        if(GL_Var::Print_Level>10){
                            GL_Var::fTrace << "Move operation " << Src_OP_ID << " last: from " << " PE " << Last_PE_ID << " to PE " << Current_PE_ID << " at time " << Migration_Time << std::endl;
                        }

                        //Keep the attach point which can be reused later
                        Attach_History Attach_Point;
                        Attach_Point.Attached_Time = Migration_Time + 2 + Current_PE_Additional_Pipeline;
                        Attach_Point.Attached_PE_ID = Current_PE_ID;
                        DFG->OP_Array[Src_OP_ID]->OP_Attach_History.push_back(Attach_Point);

                    }
                    else if(CGRA->Pipeline == HF){
                        CGRA->PE_Array[Current_PE_ID]->Reserve_Input_Port(Migration_Time+1);
                        CGRA->PE_Array[Current_PE_ID]->Set_Input_Mux(Migration_Time+1, Last_Parent_Index);
                        if(CGRA->Is_Load_PE(Current_PE_ID) || CGRA->Is_Store_PE(Current_PE_ID)){
                            CGRA->PE_Array[Current_PE_ID]->Reserve_Load_Path(Migration_Time+2);
                            CGRA->PE_Array[Current_PE_ID]->Set_Load_Mux(Migration_Time+2, 1);
                        }
                        CGRA->PE_Array[Current_PE_ID]->Reserve_WR_Port(Migration_Time+2+Current_PE_Additional_Pipeline, 1);
                        CGRA->PE_Array[Current_PE_ID]->Set_WR_Ena(Migration_Time+2+Current_PE_Additional_Pipeline, 1, 1);
                        CGRA->PE_Array[Current_PE_ID]->Set_Mem_Port(Migration_Time+2+Current_PE_Additional_Pipeline, 3, Src_OP_ID);
                        CGRA->PE_Array[Current_PE_ID]->Set_Mem_Port(Migration_Time+2+Current_PE_Additional_Pipeline, 4, Src_OP_ID);
                        CGRA->PE_Array[Current_PE_ID]->Set_Mem_Port(Migration_Time+2+Current_PE_Additional_Pipeline, 5, Src_OP_ID);

                        if(GL_Var::Print_Level>10){
                            GL_Var::fTrace << "Move operation " << Src_OP_ID << " last: from " << " PE " << Last_PE_ID << " to PE " << Current_PE_ID << " at time " << Migration_Time << std::endl;
                        }

                        //Keep the attach point which can be reused later
                        Attach_History Attach_Point;
                        Attach_Point.Attached_Time = Migration_Time + 2 + Current_PE_Additional_Pipeline;
                        Attach_Point.Attached_PE_ID = Current_PE_ID;
                        DFG->OP_Array[Src_OP_ID]->OP_Attach_History.push_back(Attach_Point);

                    }
                    else{
                        ERROR("Unknown pipeline intensity setup!\n");
                    }

                }

                if(CGRA->Pipeline == OLD){
                    Complete_Time = Migration_Time + 2 + Current_PE_Additional_Pipeline;
                }
                else if(CGRA->Pipeline == LF){
                    Complete_Time = Migration_Time + 1 + Current_PE_Additional_Pipeline;
                }
                else if(CGRA->Pipeline == MF){
                    Complete_Time = Migration_Time + 1 + Current_PE_Additional_Pipeline;
                }
                else if(CGRA->Pipeline == LHF){
                    Complete_Time = Migration_Time + 2 + Current_PE_Additional_Pipeline;
                }
                else if(CGRA->Pipeline == HF){
                    Complete_Time = Migration_Time + 2 + Current_PE_Additional_Pipeline;
                }
                else{
                    ERROR("Unknown pipeline intensity setup!\n");
                }
                i++;

            }

        }

        Last_PE_ID = Current_PE_ID;

    }

    return Complete_Time;

}

int Scheduler::OP_Exe(const int &Target_OP_ID, \
        const std::vector<int> &Src_OP_IDs, \
        const int &Target_PE_ID, \
        const std::vector<int> &Arrival_Time, \
        const Exe_Mode &Mode){

    int Exe_Time;
    int Last_Arrival_Time = 0;
    for(int i=0; i<DFG->Max_Src_OP_Num; i++){
        if(Last_Arrival_Time < Arrival_Time[i]){
            Last_Arrival_Time = Arrival_Time[i];
        }
    }
    int Start_Time = Last_Arrival_Time;
    int OP_Cost = Get_Opcode_Cost(DFG->OP_Array[Target_OP_ID]->OP_Attribute.OP_Opcode);

    while(true){
        bool Src_RD_Avail = true;
        Src_RD_Avail = Src_RD_Avail && (CGRA->PE_Array[Target_PE_ID]->Is_RD_Port_Avail(Start_Time+1, 3));
        Src_RD_Avail = Src_RD_Avail && (CGRA->PE_Array[Target_PE_ID]->Is_RD_Port_Avail(Start_Time+1, 4));
        Src_RD_Avail = Src_RD_Avail && (CGRA->PE_Array[Target_PE_ID]->Is_RD_Port_Avail(Start_Time+1, 5));
        Src_RD_Avail = Src_RD_Avail && (CGRA->PE_Array[Target_PE_ID]->Is_WR_Port_Avail(Start_Time+1, 1));

        bool ALU_Input_Avail;
        bool ALU_Output_Avail;
        bool Data_Mem_WR_Avail;
        if(CGRA->Pipeline == OLD){
            ALU_Input_Avail = CGRA->PE_Array[Target_PE_ID]->Is_ALU_Input_Avail(Start_Time+3);
            ALU_Output_Avail = CGRA->PE_Array[Target_PE_ID]->Is_ALU_Output_Avail(Start_Time+6+OP_Cost-3);
            Data_Mem_WR_Avail = CGRA->PE_Array[Target_PE_ID]->Is_WR_Port_Avail(Start_Time+7+OP_Cost-3, 0);
            Data_Mem_WR_Avail = Data_Mem_WR_Avail && (CGRA->PE_Array[Target_PE_ID]->Is_RD_Port_Avail(Start_Time+7+OP_Cost-3, 0));
            Data_Mem_WR_Avail = Data_Mem_WR_Avail && (CGRA->PE_Array[Target_PE_ID]->Is_RD_Port_Avail(Start_Time+7+OP_Cost-3, 1));
            Data_Mem_WR_Avail = Data_Mem_WR_Avail && (CGRA->PE_Array[Target_PE_ID]->Is_RD_Port_Avail(Start_Time+7+OP_Cost-3, 2));
        }
        else if(CGRA->Pipeline == LF){
            ALU_Input_Avail = CGRA->PE_Array[Target_PE_ID]->Is_ALU_Input_Avail(Start_Time+2);
            ALU_Output_Avail = CGRA->PE_Array[Target_PE_ID]->Is_ALU_Output_Avail(Start_Time+3+OP_Cost-3);
            Data_Mem_WR_Avail = CGRA->PE_Array[Target_PE_ID]->Is_WR_Port_Avail(Start_Time+3+OP_Cost-3, 0);
            Data_Mem_WR_Avail = Data_Mem_WR_Avail && (CGRA->PE_Array[Target_PE_ID]->Is_RD_Port_Avail(Start_Time+3+OP_Cost-3, 0));
            Data_Mem_WR_Avail = Data_Mem_WR_Avail && (CGRA->PE_Array[Target_PE_ID]->Is_RD_Port_Avail(Start_Time+3+OP_Cost-3, 1));
            Data_Mem_WR_Avail = Data_Mem_WR_Avail && (CGRA->PE_Array[Target_PE_ID]->Is_RD_Port_Avail(Start_Time+3+OP_Cost-3, 2));
        }
        else if(CGRA->Pipeline == MF){
            ALU_Input_Avail = CGRA->PE_Array[Target_PE_ID]->Is_ALU_Input_Avail(Start_Time+2);
            ALU_Output_Avail = CGRA->PE_Array[Target_PE_ID]->Is_ALU_Output_Avail(Start_Time+4+OP_Cost-3);
            Data_Mem_WR_Avail = CGRA->PE_Array[Target_PE_ID]->Is_WR_Port_Avail(Start_Time+4+OP_Cost-3, 0);
            Data_Mem_WR_Avail = Data_Mem_WR_Avail && (CGRA->PE_Array[Target_PE_ID]->Is_RD_Port_Avail(Start_Time+4+OP_Cost-3, 0));
            Data_Mem_WR_Avail = Data_Mem_WR_Avail && (CGRA->PE_Array[Target_PE_ID]->Is_RD_Port_Avail(Start_Time+4+OP_Cost-3, 1));
            Data_Mem_WR_Avail = Data_Mem_WR_Avail && (CGRA->PE_Array[Target_PE_ID]->Is_RD_Port_Avail(Start_Time+4+OP_Cost-3, 2));
        }
        else if(CGRA->Pipeline == LHF){
            ALU_Input_Avail = CGRA->PE_Array[Target_PE_ID]->Is_ALU_Input_Avail(Start_Time+2);
            ALU_Output_Avail = CGRA->PE_Array[Target_PE_ID]->Is_ALU_Output_Avail(Start_Time+5+OP_Cost-3);
            Data_Mem_WR_Avail = CGRA->PE_Array[Target_PE_ID]->Is_WR_Port_Avail(Start_Time+5+OP_Cost-3, 0);
            Data_Mem_WR_Avail = Data_Mem_WR_Avail && (CGRA->PE_Array[Target_PE_ID]->Is_RD_Port_Avail(Start_Time+5+OP_Cost-3, 0));
            Data_Mem_WR_Avail = Data_Mem_WR_Avail && (CGRA->PE_Array[Target_PE_ID]->Is_RD_Port_Avail(Start_Time+5+OP_Cost-3, 1));
            Data_Mem_WR_Avail = Data_Mem_WR_Avail && (CGRA->PE_Array[Target_PE_ID]->Is_RD_Port_Avail(Start_Time+5+OP_Cost-3, 2));
        }
        else if(CGRA->Pipeline == HF){
            ALU_Input_Avail = CGRA->PE_Array[Target_PE_ID]->Is_ALU_Input_Avail(Start_Time+5);
            ALU_Output_Avail = CGRA->PE_Array[Target_PE_ID]->Is_ALU_Output_Avail(Start_Time+8+OP_Cost-3);
            Data_Mem_WR_Avail = CGRA->PE_Array[Target_PE_ID]->Is_WR_Port_Avail(Start_Time+9+OP_Cost-3, 0);
            Data_Mem_WR_Avail = Data_Mem_WR_Avail && (CGRA->PE_Array[Target_PE_ID]->Is_RD_Port_Avail(Start_Time+9+OP_Cost-3, 0));
            Data_Mem_WR_Avail = Data_Mem_WR_Avail && (CGRA->PE_Array[Target_PE_ID]->Is_RD_Port_Avail(Start_Time+9+OP_Cost-3, 1));
            Data_Mem_WR_Avail = Data_Mem_WR_Avail && (CGRA->PE_Array[Target_PE_ID]->Is_RD_Port_Avail(Start_Time+9+OP_Cost-3, 2));
        }
        else{
            ERROR("Unknown pipeline intensity setup!\n");
        }


        if(Src_RD_Avail && ALU_Input_Avail && ALU_Output_Avail && Data_Mem_WR_Avail){
            break;
        }
        else{
            Start_Time++;
        }
    }
    if(CGRA->Pipeline == OLD){
        Exe_Time = Start_Time+7+OP_Cost-3;
    }
    else if(CGRA->Pipeline == LF){
        Exe_Time = Start_Time+3+OP_Cost-3;
    }
    else if(CGRA->Pipeline == MF){
        Exe_Time = Start_Time+4+OP_Cost-3;
    }
    else if(CGRA->Pipeline == LHF){
        Exe_Time = Start_Time+5+OP_Cost-3;
    }
    else if(CGRA->Pipeline == HF){
        Exe_Time = Start_Time+9+OP_Cost-3;
    }
    else{
        ERROR("Unknown pipeline intensity setup!\n");
    }

    if(Mode == Impl){
        if(GL_Var::Print_Level>10){
            if(CGRA->Pipeline == OLD){
                GL_Var::fTrace << "Operation " << Target_OP_ID << " starts execution on " << " PE " << Target_PE_ID << " at time " << Start_Time+3 << std::endl;
            }
            else if(CGRA->Pipeline == LF){
                GL_Var::fTrace << "Operation " << Target_OP_ID << " starts execution on " << " PE " << Target_PE_ID << " at time " << Start_Time+2 << std::endl;
            }
            else if(CGRA->Pipeline == MF){
                GL_Var::fTrace << "Operation " << Target_OP_ID << " starts execution on " << " PE " << Target_PE_ID << " at time " << Start_Time+2 << std::endl;
            }
            else if(CGRA->Pipeline == LHF){
                GL_Var::fTrace << "Operation " << Target_OP_ID << " starts execution on " << " PE " << Target_PE_ID << " at time " << Start_Time+2 << std::endl;
            }
            else if(CGRA->Pipeline == HF){
                GL_Var::fTrace << "Operation " << Target_OP_ID << " starts execution on " << " PE " << Target_PE_ID << " at time " << Start_Time+5 << std::endl;
            }
            else{
                ERROR("Unknown pipeline intensity setup!\n");
            }
        }

        Target_PE_Refresh(Src_OP_IDs, Target_OP_ID, Start_Time, Target_PE_ID);
        Target_OP_Refresh(Src_OP_IDs, Target_OP_ID, Target_PE_ID, Exe_Time);

    }

    return Exe_Time;

}

int Scheduler::Nearest_Attached_PE(const int &Src_OP_ID, \
        const int &Dst_PE_ID, int &Src_Ready_Time){

    int Nearest_Attached_PE_ID = NaN;
    int Min_Dist = INT_MAX;

    std::list<Attach_History>::iterator Lit;
    std::list<Attach_History>::iterator Lit_Head;
    std::list<Attach_History>::iterator Lit_Tail;
    Lit_Head = DFG->OP_Array[Src_OP_ID]->OP_Attach_History.begin();
    Lit_Tail = DFG->OP_Array[Src_OP_ID]->OP_Attach_History.end();
    for(Lit = Lit_Head; Lit != Lit_Tail; Lit++){
        int PE_ID_Tmp = (*Lit).Attached_PE_ID;
        int Dist_Tmp = CGRA->Get_Dist(PE_ID_Tmp, Dst_PE_ID);
        if(Dist_Tmp < Min_Dist){
            Min_Dist = Dist_Tmp;
            Nearest_Attached_PE_ID = PE_ID_Tmp;
            Src_Ready_Time = (*Lit).Attached_Time;
        }
    }

    // 0 is available for each PE
    if(DFG->OP_Array[Src_OP_ID]->OP_ID == 0){
        Nearest_Attached_PE_ID = Dst_PE_ID;
    }

    if(Nearest_Attached_PE_ID == NaN){
        ERROR("Unexpected nearest PE!");
    }

    return Nearest_Attached_PE_ID;

}

/*
int Scheduler::DistCal(const int &src_op, const int &dst_op){
    if(src_op==0 || dst_op==0){
        return 0;
    }
    else{
        int src_PE_id=DFG->OP_Array[src_op]->vertex_attribute.execution_PE_id;
        int dst_PE_id=DFG->OP_Array[dst_op]->vertex_attribute.execution_PE_id;
        return CGRA->PE_pair_distance[src_PE_id][dst_PE_id];
    }
}
*/

/*
int Scheduler::DynamicOperationSelection(){

    //A single input operand is needed to make the target operation ready
    list<int> ready_input_set1; 

    //Two input operands are needed to make the target operation ready
    list<int> ready_input_set2;

    //Three input operands are needed to make the target operation ready
    list<int> ready_input_set3;

    //Operations that have all the source operands ready
    list<int> ready_operation_set;
    list<int> possible_distance;

    for(int i=0; i<DFG->vertex_num; i++){
        Vertex* vp=DFG->OP_Array[i];
        if(vp->vertex_type!=InputData && vp->vertex_attribute.vertex_state==DataUnavail){
            vector<Vertex*>::iterator it;

            //Test whether all the source operands are ready
            bool all_src_ready=true;
            for(it=vp->parents.begin(); it!=vp->parents.end(); it++){
                if((*it)->vertex_attribute.vertex_state==DataUnavail){
                    all_src_ready=false;
                    break;
                }
            }

            if(all_src_ready){
                //If source operands are in out side memory, the operands should be ready to load.
                //If the source operands are all available, the target operation is considered to be ready.
                int out_op_num=0;
                for(it=vp->parents.begin(); it!=vp->parents.end(); it++){
                    if((*it)->vertex_attribute.vertex_state==DataInOutMem){
                        out_op_num++;
                    }
                }
                if(out_op_num==0){
                    ready_operation_set.push_back(i);
                    vector<int> Src_OP_IDs;
                    for(it=vp->parents.begin(); it!=vp->parents.end(); it++){
                        Src_OP_IDs.push_back((*it)->vertex_id);
                    }
                    int sum_dist=DistCal(Src_OP_IDs[0], Src_OP_IDs[1]);
                    sum_dist+=DistCal(Src_OP_IDs[0], Src_OP_IDs[2]);
                    sum_dist+=DistCal(Src_OP_IDs[1], Src_OP_IDs[2]);
                    possible_distance.push_back(sum_dist);
                }
                else if(out_op_num==1){
                    for(it=vp->parents.begin();it!=vp->parents.end(); it++){
                        if((*it)->vertex_attribute.vertex_state==DataInOutMem){
                            ready_input_set1.push_back((*it)->vertex_id);
                        }
                    }
                }
                else if(out_op_num==2){
                    for(it=vp->parents.begin();it!=vp->parents.end(); it++){
                        if((*it)->vertex_attribute.vertex_state==DataInOutMem){
                            ready_input_set2.push_back((*it)->vertex_id);
                        }
                    }
                }
                else if(out_op_num==3){
                    for(it=vp->parents.begin();it!=vp->parents.end(); it++){
                        if((*it)->vertex_attribute.vertex_state==DataInOutMem){
                            ready_input_set3.push_back((*it)->vertex_id);
                        }
                    }
                }
                else{
                    PRINT("Unexpected vertex state! %d=", out_op_num);
                }
            }
        }
    }

    if(!ready_operation_set.empty()){
        list<int>::iterator it1;
        list<int>::iterator it2;
        int min_dist=INT_MAX;
        int selected_op=NaN;
        it2=possible_distance.begin();
        for(it1=ready_operation_set.begin(); it1!=ready_operation_set.end(); it1++){
            if((*it2)<min_dist){
                min_dist=(*it2);
                selected_op=(*it1);
            }
            it2++;
        }
        return selected_op;
    }
    else if(!ready_input_set1.empty()){
        return ready_input_set1.front();
    }
    else if(!ready_input_set2.empty()){
        return ready_input_set2.front();
    }
    else if(!ready_input_set3.empty()){
        return ready_input_set3.front();
    }
    else{
        ERROR("No operation left for scheduling!");
    }

}
*/

/*
int Scheduler::StaticOperationSelection(){

    list<int> candidate_operation_set;
    int highest_priority=0;

    //Find out the highest priority of operations that have not been executed or fethced.
    for(int i=0; i<DFG->vertex_num; i++){
        Vertex* vertex_tmp=DFG->OP_Array[i];
        if(vertex_tmp->vertex_attribute.vertex_state==DataInOutMem || vertex_tmp->vertex_attribute.vertex_state==DataUnavail){
            int priority_tmp=vertex_tmp->vertex_attribute.scheduling_priority;
            if(priority_tmp>highest_priority){
                highest_priority=priority_tmp;
            }
        }
    }

    //Put these operations with highest available priority in a list
    for(int i=0; i<DFG->vertex_num; i++){
        Vertex* vertex_tmp=DFG->OP_Array[i];
        if(vertex_tmp->vertex_attribute.vertex_state==DataInOutMem || vertex_tmp->vertex_attribute.vertex_state==DataUnavail){
            int priority_tmp=vertex_tmp->vertex_attribute.scheduling_priority;
            if(priority_tmp==highest_priority){
                candidate_operation_set.push_back(i);
            }
        }
    }
    if(candidate_operation_set.empty()==true){
        ERROR("No Candidates available before scheduling is completed!");
    }

    //Selected the operation with most children first. Note that it may require larger data memory because data will not be
    //consumed as soon as possible. If data memory is the bottleneck, chooing the operation with least children may help.
    int min_children_num=INT_MAX;
    int selected_OP_ID;
    list<int>::iterator iter_tmp;
    for(iter_tmp=candidate_operation_set.begin(); iter_tmp!=candidate_operation_set.end(); iter_tmp++){
        int children_num=DFG->OP_Array[*iter_tmp]->children.size();
        if(min_children_num>children_num){
            min_children_num=children_num;
            selected_OP_ID=*iter_tmp;
        }
    }
    return selected_OP_ID;

}
*/

void Scheduler::Target_PE_Refresh(const std::vector<int> &Src_OP_IDs, const int &Target_OP_ID, const int &Start_Time, const int &Target_PE_ID){

    int Current_Exe_Time;
    int OP_Cost = Get_Opcode_Cost(DFG->OP_Array[Target_OP_ID]->OP_Attribute.OP_Opcode);
    Opcode Opcode_Tmp = DFG->OP_Array[Target_OP_ID]->OP_Attribute.OP_Opcode;
    if(CGRA->Pipeline == OLD){
        CGRA->PE_Array[Target_PE_ID]->Reserve_RD_Port(Start_Time+1, 3);
        CGRA->PE_Array[Target_PE_ID]->Reserve_RD_Port(Start_Time+1, 4);
        CGRA->PE_Array[Target_PE_ID]->Reserve_RD_Port(Start_Time+1, 5);
        CGRA->PE_Array[Target_PE_ID]->Set_WR_Ena(Start_Time+1, 1, 0);
        CGRA->PE_Array[Target_PE_ID]->Set_Mem_Port(Start_Time+1, 3, Src_OP_IDs[0]);
        CGRA->PE_Array[Target_PE_ID]->Set_Mem_Port(Start_Time+1, 4, Src_OP_IDs[1]);
        CGRA->PE_Array[Target_PE_ID]->Set_Mem_Port(Start_Time+1, 5, Src_OP_IDs[2]);

        CGRA->PE_Array[Target_PE_ID]->Reserve_ALU_Input(Start_Time+3);
        CGRA->PE_Array[Target_PE_ID]->Set_ALU_Opcode(Start_Time+3, Opcode_Tmp);
        CGRA->PE_Array[Target_PE_ID]->Set_ALU_Output_Mux(Start_Time+5+OP_Cost-3, Opcode_Tmp);
        CGRA->PE_Array[Target_PE_ID]->Reserve_ALU_Output(Start_Time+6+OP_Cost-3);

        CGRA->PE_Array[Target_PE_ID]->Reserve_WR_Port(Start_Time+7+OP_Cost-3, 0);
        CGRA->PE_Array[Target_PE_ID]->Set_WR_Ena(Start_Time+7+OP_Cost-3, 0, 1);
        CGRA->PE_Array[Target_PE_ID]->Set_Mem_Port(Start_Time+7+OP_Cost-3, 0, Target_OP_ID);
        CGRA->PE_Array[Target_PE_ID]->Set_Mem_Port(Start_Time+7+OP_Cost-3, 1, Target_OP_ID);
        CGRA->PE_Array[Target_PE_ID]->Set_Mem_Port(Start_Time+7+OP_Cost-3, 2, Target_OP_ID);

        Current_Exe_Time = Start_Time+7+OP_Cost-3;
    }
    else if(CGRA->Pipeline == LF){
        CGRA->PE_Array[Target_PE_ID]->Reserve_RD_Port(Start_Time+1, 3);
        CGRA->PE_Array[Target_PE_ID]->Reserve_RD_Port(Start_Time+1, 4);
        CGRA->PE_Array[Target_PE_ID]->Reserve_RD_Port(Start_Time+1, 5);
        CGRA->PE_Array[Target_PE_ID]->Set_Mem_Port(Start_Time+1, 3, Src_OP_IDs[0]);
        CGRA->PE_Array[Target_PE_ID]->Set_Mem_Port(Start_Time+1, 4, Src_OP_IDs[1]);
        CGRA->PE_Array[Target_PE_ID]->Set_Mem_Port(Start_Time+1, 5, Src_OP_IDs[2]);
        CGRA->PE_Array[Target_PE_ID]->Set_WR_Ena(Start_Time+1, 1, 0);

        CGRA->PE_Array[Target_PE_ID]->Reserve_ALU_Input(Start_Time+2);
        CGRA->PE_Array[Target_PE_ID]->Set_ALU_Opcode(Start_Time+2, Opcode_Tmp);
        CGRA->PE_Array[Target_PE_ID]->Reserve_ALU_Output(Start_Time+3+OP_Cost-3);
        CGRA->PE_Array[Target_PE_ID]->Set_ALU_Output_Mux(Start_Time+2+OP_Cost-3, Opcode_Tmp);

        CGRA->PE_Array[Target_PE_ID]->Reserve_WR_Port(Start_Time+3+OP_Cost-3, 0);
        CGRA->PE_Array[Target_PE_ID]->Set_WR_Ena(Start_Time+3+OP_Cost-3, 0, 1);
        CGRA->PE_Array[Target_PE_ID]->Set_Mem_Port(Start_Time+3+OP_Cost-3, 0, Target_OP_ID);
        CGRA->PE_Array[Target_PE_ID]->Set_Mem_Port(Start_Time+3+OP_Cost-3, 1, Target_OP_ID);
        CGRA->PE_Array[Target_PE_ID]->Set_Mem_Port(Start_Time+3+OP_Cost-3, 2, Target_OP_ID);

        Current_Exe_Time = Start_Time+3+OP_Cost-3;
    }
    else if(CGRA->Pipeline == MF){
        CGRA->PE_Array[Target_PE_ID]->Reserve_RD_Port(Start_Time+1, 3);
        CGRA->PE_Array[Target_PE_ID]->Reserve_RD_Port(Start_Time+1, 4);
        CGRA->PE_Array[Target_PE_ID]->Reserve_RD_Port(Start_Time+1, 5);
        CGRA->PE_Array[Target_PE_ID]->Set_Mem_Port(Start_Time+1, 3, Src_OP_IDs[0]);
        CGRA->PE_Array[Target_PE_ID]->Set_Mem_Port(Start_Time+1, 4, Src_OP_IDs[1]);
        CGRA->PE_Array[Target_PE_ID]->Set_Mem_Port(Start_Time+1, 5, Src_OP_IDs[2]);
        CGRA->PE_Array[Target_PE_ID]->Set_WR_Ena(Start_Time+1, 1, 0);

        CGRA->PE_Array[Target_PE_ID]->Reserve_ALU_Input(Start_Time+2);
        CGRA->PE_Array[Target_PE_ID]->Set_ALU_Opcode(Start_Time+2, Opcode_Tmp);
        CGRA->PE_Array[Target_PE_ID]->Reserve_ALU_Output(Start_Time+4+OP_Cost-3);
        CGRA->PE_Array[Target_PE_ID]->Set_ALU_Output_Mux(Start_Time+3+OP_Cost-3, Opcode_Tmp);

        CGRA->PE_Array[Target_PE_ID]->Reserve_WR_Port(Start_Time+4+OP_Cost-3, 0);
        CGRA->PE_Array[Target_PE_ID]->Set_WR_Ena(Start_Time+4+OP_Cost-3, 0, 1);
        CGRA->PE_Array[Target_PE_ID]->Set_Mem_Port(Start_Time+4+OP_Cost-3, 0, Target_OP_ID);
        CGRA->PE_Array[Target_PE_ID]->Set_Mem_Port(Start_Time+4+OP_Cost-3, 1, Target_OP_ID);
        CGRA->PE_Array[Target_PE_ID]->Set_Mem_Port(Start_Time+4+OP_Cost-3, 2, Target_OP_ID);

        Current_Exe_Time = Start_Time+4+OP_Cost-3;
    }
    else if(CGRA->Pipeline == LHF){
        CGRA->PE_Array[Target_PE_ID]->Reserve_RD_Port(Start_Time+1, 3);
        CGRA->PE_Array[Target_PE_ID]->Reserve_RD_Port(Start_Time+1, 4);
        CGRA->PE_Array[Target_PE_ID]->Reserve_RD_Port(Start_Time+1, 5);
        CGRA->PE_Array[Target_PE_ID]->Set_Mem_Port(Start_Time+1, 3, Src_OP_IDs[0]);
        CGRA->PE_Array[Target_PE_ID]->Set_Mem_Port(Start_Time+1, 4, Src_OP_IDs[1]);
        CGRA->PE_Array[Target_PE_ID]->Set_Mem_Port(Start_Time+1, 5, Src_OP_IDs[2]);
        CGRA->PE_Array[Target_PE_ID]->Set_WR_Ena(Start_Time+1, 1, 0);

        CGRA->PE_Array[Target_PE_ID]->Set_ALU_Opcode(Start_Time+2, Opcode_Tmp);
        CGRA->PE_Array[Target_PE_ID]->Reserve_ALU_Input(Start_Time+2);
        CGRA->PE_Array[Target_PE_ID]->Reserve_ALU_Output(Start_Time+5+OP_Cost-3);
        CGRA->PE_Array[Target_PE_ID]->Set_ALU_Output_Mux(Start_Time+4+OP_Cost-3, Opcode_Tmp);

        CGRA->PE_Array[Target_PE_ID]->Reserve_WR_Port(Start_Time+5+OP_Cost-3, 0);
        CGRA->PE_Array[Target_PE_ID]->Set_WR_Ena(Start_Time+5+OP_Cost-3, 0, 1);
        CGRA->PE_Array[Target_PE_ID]->Set_Mem_Port(Start_Time+5+OP_Cost-3, 0, Target_OP_ID);
        CGRA->PE_Array[Target_PE_ID]->Set_Mem_Port(Start_Time+5+OP_Cost-3, 1, Target_OP_ID);
        CGRA->PE_Array[Target_PE_ID]->Set_Mem_Port(Start_Time+5+OP_Cost-3, 2, Target_OP_ID);

        Current_Exe_Time = Start_Time+5+OP_Cost-3;
    }
    else if(CGRA->Pipeline == HF){
        CGRA->PE_Array[Target_PE_ID]->Reserve_RD_Port(Start_Time+1, 3);
        CGRA->PE_Array[Target_PE_ID]->Reserve_RD_Port(Start_Time+1, 4);
        CGRA->PE_Array[Target_PE_ID]->Reserve_RD_Port(Start_Time+1, 5);
        CGRA->PE_Array[Target_PE_ID]->Set_WR_Ena(Start_Time+1, 1, 0);
        CGRA->PE_Array[Target_PE_ID]->Set_Mem_Port(Start_Time+1, 3, Src_OP_IDs[0]);
        CGRA->PE_Array[Target_PE_ID]->Set_Mem_Port(Start_Time+1, 4, Src_OP_IDs[1]);
        CGRA->PE_Array[Target_PE_ID]->Set_Mem_Port(Start_Time+1, 5, Src_OP_IDs[2]);

        CGRA->PE_Array[Target_PE_ID]->Reserve_ALU_Input(Start_Time+5);
        CGRA->PE_Array[Target_PE_ID]->Set_ALU_Opcode(Start_Time+5, Opcode_Tmp);
        CGRA->PE_Array[Target_PE_ID]->Reserve_ALU_Output(Start_Time+8+OP_Cost-3);
        CGRA->PE_Array[Target_PE_ID]->Set_ALU_Output_Mux(Start_Time+7+OP_Cost-3, Opcode_Tmp);

        CGRA->PE_Array[Target_PE_ID]->Reserve_WR_Port(Start_Time+9+OP_Cost-3, 0);
        CGRA->PE_Array[Target_PE_ID]->Set_WR_Ena(Start_Time+9+OP_Cost-3, 0, 1);
        CGRA->PE_Array[Target_PE_ID]->Set_Mem_Port(Start_Time+9+OP_Cost-3, 0, Target_OP_ID);
        CGRA->PE_Array[Target_PE_ID]->Set_Mem_Port(Start_Time+9+OP_Cost-3, 1, Target_OP_ID);
        CGRA->PE_Array[Target_PE_ID]->Set_Mem_Port(Start_Time+9+OP_Cost-3, 2, Target_OP_ID);

        Current_Exe_Time = Start_Time+9+OP_Cost-3;
    }
    else{
        ERROR("Unknown pipeline intensity setup!\n");
    }

    if(CGRA->PE_Array[Target_PE_ID]->Max_Active_Time < Current_Exe_Time){
        CGRA->PE_Array[Target_PE_ID]->Max_Active_Time = Current_Exe_Time;
    }

}

/*
void Scheduler::FetchSourceOperation(const int &Target_PE_ID, const vector<int> &Src_OP_IDs, const vector<list<int> > &source_routing_paths, const vector<int> &Start_Time, vector<int> &source_ready_time){

    for(int i=0; i<INST_OP_NUM-1; i++){
        source_ready_time[i]=OperationTransmission(Start_Time[i], Src_OP_IDs[i], source_routing_paths[i], Implementation);
    }

}
*/

void Scheduler::Target_OP_Refresh(const std::vector<int> &Src_OP_IDs, const int &Target_OP_ID, const int &Target_PE_ID, const int &Exe_Time){

    DFG->OP_Array[Target_OP_ID]->OP_Attribute.OP_State = Avail;
    DFG->OP_Array[Target_OP_ID]->OP_Attribute.OP_Avail_Time = Exe_Time;
    DFG->OP_Array[Target_OP_ID]->OP_Attribute.Exe_PE_ID = Target_PE_ID;
    int Src0 = DFG->OP_Array[Src_OP_IDs[0]]->OP_Val;
    int Src1 = DFG->OP_Array[Src_OP_IDs[1]]->OP_Val;
    int Src2 = DFG->OP_Array[Src_OP_IDs[2]]->OP_Val;
    Opcode Opcode_Tmp = DFG->OP_Array[Target_OP_ID]->OP_Attribute.OP_Opcode;
    DFG->OP_Array[Target_OP_ID]->OP_Val = OP_Compute(Opcode_Tmp, Src0, Src1, Src2);

    Attach_History Attach_Point;
    Attach_Point.Attached_Time = Exe_Time;
    Attach_Point.Attached_PE_ID = Target_PE_ID;
    DFG->OP_Array[Target_OP_ID]->OP_Attach_History.push_back(Attach_Point);

}

bool Scheduler::Is_Scheduling_Completed(){

    bool Scheduling_Completed = true;

    //Check final execution
    for(int i=0; i<DFG->OP_Num; i++){
        if(DFG->OP_Array[i]->OP_Type == IM && DFG->OP_Array[i]->OP_Attribute.OP_State != Avail){
            Scheduling_Completed = false;
            break;
        }
        else if((DFG->OP_Array[i]->OP_Type == OUTVAR || DFG->OP_Array[i]->OP_Type == IMOUT) && (DFG->OP_Array[i]->OP_Attribute.OP_State != Avail)){
            Scheduling_Completed = false;
            break;
        }
    }

    return Scheduling_Completed;

}

void Scheduler::Scheduling_Stat(){

    std::vector<float> Output_Port_Util;
    std::vector<float> Data_Mem_RD_Util;
    std::vector<float> Data_Mem_WR_Util;
    std::vector<float> ALU_Util;
    Output_Port_Util.resize(CGRA->CGRA_Scale);
    Data_Mem_RD_Util.resize(CGRA->CGRA_Scale);
    Data_Mem_WR_Util.resize(CGRA->CGRA_Scale);
    ALU_Util.resize(CGRA->CGRA_Scale);

    for(int i=0; i<CGRA->CGRA_Scale; i++){
        int Output_Port_Util_Cnt = 0;
        int ALU_Util_Cnt = 0;
        int Data_Mem_RD_Util_Cnt = 0;
        int Data_Mem_WR_Util_Cnt = 0;
        int Output_Degree = CGRA->PE_Array[i]->Output_Degree;
        for(int j=0; j<=Scheduling_Complete_Time; j++){
            for(int p=0; p<6; p++){
                if(!(CGRA->PE_Array[i]->Is_RD_Port_Avail(j, p))){
                    Data_Mem_RD_Util_Cnt++;
                }
            }

            for(int p=0; p<4; p++){
                if(!(CGRA->PE_Array[i]->Is_Output_Port_Avail(j, p))){
                    Output_Port_Util_Cnt++;
                }
            }

            if(!(CGRA->PE_Array[i]->Is_ALU_Input_Avail(j))){
                ALU_Util_Cnt++;
            }

            for(int p=0; p<2; p++){
                if(!(CGRA->PE_Array[i]->Is_WR_Port_Avail(j, p))){
                    Data_Mem_WR_Util_Cnt++;
                }
            }
        }
        Data_Mem_RD_Util[i] = 1.0*Data_Mem_RD_Util_Cnt/Scheduling_Complete_Time;
        Data_Mem_WR_Util[i] = 1.0*Data_Mem_WR_Util_Cnt/Scheduling_Complete_Time;
        Output_Port_Util[i] = 1.0*Output_Port_Util_Cnt/(Scheduling_Complete_Time*Output_Degree);
        ALU_Util[i] = 1.0*ALU_Util_Cnt/Scheduling_Complete_Time;
    }

    //Print resource utilization information
    if(false){
        std::cout << setiosflags(std::ios::left);
        std::cout << std::setfill(' ') << std::setw(6) << "PE";
        std::cout << std::setfill(' ') << std::setw(15) << "output port";
        std::cout << std::setfill(' ') << std::setw(15) << "Data Mem Read";
        std::cout << std::setfill(' ') << std::setw(16) << "Data Mem Write";
        std::cout << std::setfill(' ') << std::setw(18) << "ALU";
        std::cout << "\n";
        for(int i=0; i<CGRA->CGRA_Scale; i++){
            std::cout << std::setfill(' ') << std::setw(6) << i;
            std::cout << std::setfill(' ') << std::setw(15) << std::setprecision(4) << Output_Port_Util[i];
            std::cout << std::setfill(' ') << std::setw(15) << std::setprecision(4) << Data_Mem_RD_Util[i];
            std::cout << std::setfill(' ') << std::setw(16) << std::setprecision(4) << Data_Mem_WR_Util[i];
            std::cout << std::setfill(' ') << std::setw(18) << std::setprecision(4) << ALU_Util[i] << "\n";
        }
    }

}

void Scheduler::Data_Mem_Analysis(){

    //Analyze the data memory capacity
    std::vector<int> Data_Mem_Capacity;
    Data_Mem_Capacity.resize(CGRA->CGRA_Scale);
    for(int i=0; i<CGRA->CGRA_Scale; i++){
        Data_Mem_Capacity[i] = 0;
    }

    std::vector<int> Create_Time;
    std::vector<int> Destroy_Time;
    Create_Time.resize(DFG->OP_Num);
    Destroy_Time.resize(DFG->OP_Num);

    for(int i=0; i<CGRA->CGRA_Scale; i++){
        for(int j=0; j<DFG->OP_Num; j++){
            Create_Time[j] = NaN;
            Destroy_Time[j] = NaN;
        }
        Create_Time[0] = 0;

        //Refresh the OP create time in memory write port
        for(int j=0; j<Scheduling_Complete_Time; j++){
            for(int p=0; p<2; p++){
                int WR_OP;
                if(p==0){
                    WR_OP = CGRA->PE_Array[i]->Get_OP_Of_Mem_Port(j, 0);
                }
                else{
                    WR_OP = CGRA->PE_Array[i]->Get_OP_Of_Mem_Port(j, 3);
                }


                bool WR_Reserved = !(CGRA->PE_Array[i]->Is_WR_Port_Avail(j, p));
                if(WR_Reserved){
                    if(Create_Time[WR_OP]==NaN || Create_Time[WR_OP]>j){
                        Create_Time[WR_OP]=j;
                    }

                }
            }

            //Refresh the creatation time & destroy time in memory reading port
            for(int p=0; p<6; p++){
                bool RD_Reserved = !(CGRA->PE_Array[i]->Is_RD_Port_Avail(j, p));
                int RD_OP = CGRA->PE_Array[i]->Get_OP_Of_Mem_Port(j, p);
                if(RD_Reserved){
                    if(Destroy_Time[RD_OP]==NaN || Destroy_Time[RD_OP]<j){
                        Destroy_Time[RD_OP] = j;
                    }
                    if(IO_Placement_Scheme == Pre_Placement && (DFG->OP_Array[RD_OP]->Is_Input())){
                        Create_Time[RD_OP] = 0;
                    }

                }
            }
        }

        //Check the create time and destroy time to see if there are conflictions.
        for(int j=0; j<DFG->OP_Num; j++){
            if(Create_Time[j]==NaN && Destroy_Time[j]>0){
                std::cout << "OP ID is " << j << " , it has " << DFG->OP_Array[j]->OP_Children.size() << "children and " << DFG->OP_Array[j]->OP_Parents.size() << "parents!";
                std::cout << "Execution PE id is " << DFG->OP_Array[j]->OP_Attribute.Exe_PE_ID << std::endl;
                std::cout << "Executed time is " << DFG->OP_Array[j]->OP_Attribute.OP_Avail_Time << std::endl;
                if(DFG->OP_Array[j]->Is_Input()){
                    std::cout << "Input Operation" << std::endl;
                }
                else if(DFG->OP_Array[j]->OP_Type == OUTVAR){
                    std::cout << "Output operation" << std::endl;
                }
                else{
                    std::cout << "Intermediate operation" << std::endl;
                }
                ERROR("Error!\n");

            }
        }

        std::vector<int> Data_Mem_Trace;
        Data_Mem_Trace.resize(Scheduling_Complete_Time);
        for(int j=0; j<Scheduling_Complete_Time; j++){
            Data_Mem_Trace[j] = 0;
        }

        int OP_In_Data_Mem_Cnt = 0;
        for(int j=0; j<Scheduling_Complete_Time; j++){
            for(int p=0; p<DFG->OP_Num; p++){
                if(Create_Time[p]==j){
                    OP_In_Data_Mem_Cnt++;
                }
                if(Destroy_Time[p]==j){
                    OP_In_Data_Mem_Cnt--;
                }
            }
            Data_Mem_Trace[j] = OP_In_Data_Mem_Cnt;
            if(Data_Mem_Capacity[i] < OP_In_Data_Mem_Cnt){
                Data_Mem_Capacity[i] = OP_In_Data_Mem_Cnt;
            }
        }


        Data_Mem_Addr_Gen(Create_Time, Destroy_Time, Data_Mem_Capacity[i], i);
    }

    //print data memory capacity of each PE
    std::cout << "PE minimum data memory requirements: ";
    for(int i=0; i<CGRA->CGRA_Scale; i++){
        std::cout << Data_Mem_Capacity[i] << " ";
    }
    std::cout << std::endl;

}

/*---------------------------------------------------------------------
  Address allocation/release
  1) When an operand is written to the data memory for the first time.
  2) When operands are constants and they are initialized into the 
  data memory. Here only op_id==0 is considered as constant and it is 
  stored in data memory[0] in all PEs. 
  3) When the data operands are referenced for the last time, the address 
  will be released.
 -----------------------------------------------------------------------*/
void Scheduler::Data_Mem_Addr_Gen(const std::vector<int> &Create_Time, const std::vector<int> &Destroy_Time, const int &Data_Mem_Capacity, const int &PE_ID){


    std::map<int, int> OP_To_Addr;
    OP_To_Addr[0] = 0;

    // To be fixed...
    std::list<int> Addr_Avail;
    for(int i=1; i<Data_Mem_Capacity+10; i++){
        Addr_Avail.push_back(i);
    }

    //Allocate address to data initialized in data memory
    for(int i=1; i<DFG->OP_Num; i++){
        if(Create_Time[i]==0){
            OP_To_Addr[i] = Addr_Avail.front();
            Addr_Avail.pop_front();
        }
    }

    for(int i=0; i<Scheduling_Complete_Time; i++){
        for(int p=0; p<2; p++){
            int WR_OP;
            if(p==0){
                WR_OP = CGRA->PE_Array[PE_ID]->Get_OP_Of_Mem_Port(i, 0);
            }
            else{
                WR_OP = CGRA->PE_Array[PE_ID]->Get_OP_Of_Mem_Port(i, 3);
            }

            //Allocate address when the data is first writen into data memory
            bool WR_Reserved = !(CGRA->PE_Array[PE_ID]->Is_WR_Port_Avail(i,p));

            if(WR_Reserved){
                if(OP_To_Addr.count(WR_OP)==0){
                    if(p==0){
                        CGRA->PE_Array[PE_ID]->Set_Mem_Port_Addr(i, 0, Addr_Avail.front());
                        CGRA->PE_Array[PE_ID]->Set_Mem_Port_Addr(i, 1, Addr_Avail.front());
                        CGRA->PE_Array[PE_ID]->Set_Mem_Port_Addr(i, 2, Addr_Avail.front());
                    }
                    else{
                        CGRA->PE_Array[PE_ID]->Set_Mem_Port_Addr(i, 3, Addr_Avail.front());
                        CGRA->PE_Array[PE_ID]->Set_Mem_Port_Addr(i, 4, Addr_Avail.front());
                        CGRA->PE_Array[PE_ID]->Set_Mem_Port_Addr(i, 5, Addr_Avail.front());
                    }
                    OP_To_Addr[WR_OP] = Addr_Avail.front();
                    Addr_Avail.pop_front();
                }
                else if(OP_To_Addr.count(WR_OP)>0){
                    if(p==0){
                        CGRA->PE_Array[PE_ID]->Set_Mem_Port_Addr(i, 0, OP_To_Addr[WR_OP]);
                        CGRA->PE_Array[PE_ID]->Set_Mem_Port_Addr(i, 1, OP_To_Addr[WR_OP]);
                        CGRA->PE_Array[PE_ID]->Set_Mem_Port_Addr(i, 2, OP_To_Addr[WR_OP]);
                    }
                    else{
                        CGRA->PE_Array[PE_ID]->Set_Mem_Port_Addr(i, 3, OP_To_Addr[WR_OP]);
                        CGRA->PE_Array[PE_ID]->Set_Mem_Port_Addr(i, 4, OP_To_Addr[WR_OP]);
                        CGRA->PE_Array[PE_ID]->Set_Mem_Port_Addr(i, 5, OP_To_Addr[WR_OP]);
                    }
                }
                else{
                    ERROR("Unexpected write operation!");
                }
            }
        }

        std::list<int> OP_To_Release;
        std::list<int>::iterator Lit;
        for(int p=0; p<6; p++){
            bool RD_Reserved = !(CGRA->PE_Array[PE_ID]->Is_RD_Port_Avail(i, p));
            int RD_OP = CGRA->PE_Array[PE_ID]->Get_OP_Of_Mem_Port(i, p);
            if(RD_Reserved){
                if(OP_To_Addr.count(RD_OP)==0){
                    std::cout << "OP_ID is " << RD_OP << std::endl;
                    std::cout << "Executed at PE " << DFG->OP_Array[RD_OP]->OP_Attribute.Exe_PE_ID << std::endl;
                    std::cout << "Current PE is " << PE_ID << std::endl;
                    std::cout << "Execution Time is " << DFG->OP_Array[RD_OP]->OP_Attribute.OP_Avail_Time << std::endl;
                    std::cout << "Current time is " << i << std::endl;
                    std::cout << "Port index is " << p << std::endl;
                    std::cout << "IO Buffer Addr is " << OP_To_Addr[RD_OP] << std::endl;
                    ERROR("The operation has never been stored at all!");
                }

                if(OP_To_Addr.count(RD_OP)>0){
                    if(Destroy_Time[RD_OP]==NaN){
                        ERROR("Unexpected cases!");
                    }

                    CGRA->PE_Array[PE_ID]->Set_Mem_Port_Addr(i, p, OP_To_Addr[RD_OP]);
                    if(Destroy_Time[RD_OP]==i){
                        bool No_Replica = true;
                        for(Lit = OP_To_Release.begin(); Lit != OP_To_Release.end(); Lit++){
                            if(RD_OP == *Lit){
                                No_Replica = false;
                                break;
                            }
                        }
                        if(No_Replica && RD_OP!=0){
                            OP_To_Release.push_back(RD_OP);
                        }
                    }
                }
                else{
                    ERROR("Unexpected cases!");
                }
            }
        }

        //Put the released address back for reuse
        while(!OP_To_Release.empty()){
            int Released_OP = OP_To_Release.front();
            int Released_Addr = OP_To_Addr[Released_OP];
            Addr_Avail.push_front(Released_Addr);
            OP_To_Release.pop_front();
            OP_To_Addr.erase(Released_OP);
        }
    }

}

bool Scheduler::OP_Computation_Check(){

    bool Verify_Passed = true;
    std::vector<int> Theoretical_OP_Result;
    std::vector<int> Simulated_OP_Result;
    DFG->DFG_Calculation(Theoretical_OP_Result);

    Simulated_OP_Result.resize(DFG->OP_Num);
    for(int i=0; i<DFG->OP_Num; i++){
        Simulated_OP_Result[i] = DFG->OP_Array[i]->OP_Val;
    }


    for(int i=0; i<DFG->OP_Num; i++){
        if(Theoretical_OP_Result[i] != Simulated_OP_Result[i]){
            ERROR("Calculation of Operation[%d] is wrong! Theoretical result is:%d, simulated result is:%d\n", i, Theoretical_OP_Result[i], Simulated_OP_Result[i]);
            Verify_Passed = false;
        }
    }

    if(Verify_Passed){
        std::cout << "Scheduling algorithm obtains the results as expected!\n" << std::endl;
    }
    else{
        PRINT("Operation results are NOT correct!");
    }

    return Verify_Passed;

}

void Scheduler::Inst_Mem_Dump_Coe(){

    for(int i=0; i<CGRA->CGRA_Scale; i++){

        std::ostringstream os;
        os << "./result/PE-" << "inst-" << i << ".coe";
        std::string fName = os.str();
        std::ofstream fHandle;
        fHandle.open(fName.c_str());
        if(!fHandle.is_open()){
            ERROR("Fail to create the PE-inst-.coe");
        }

        fHandle << "memory_initialization_radix=2;" << std::endl;
        fHandle << "memory_initialization_vector=" << std::endl;

        std::list<int> Bit_List;
        std::list<int>::reverse_iterator Rit;
        int Dec_Data;
        int Data_Width;
        std::string Bin_Str;
        for(int j=0; j<Scheduling_Complete_Time; j++){

            // DFG execution status: 100
            fHandle << "100";

            //load-mux, 1->input from neighboring PEs. 0->input from IO Buffer.
            if(CGRA->Is_Load_PE(i) || CGRA->Is_Store_PE(i)){
                fHandle << CGRA->PE_Array[i]->Get_Load_Mux(j);
            }
            else{
                fHandle << "0";
            }

            //PE input mux
            Dec_Data = CGRA->PE_Array[i]->Get_Input_Mux(j);
            Data_Width = 2;
            Dec_To_Bin_Str(Dec_Data, Data_Width, Bin_Str);
            fHandle << Bin_Str;
            
            //PE bypass mux
            Dec_Data=CGRA->PE_Array[i]->Get_Bypass_Mux(j);
            Data_Width = 2;
            Dec_To_Bin_Str(Dec_Data, Data_Width, Bin_Str);
            fHandle << Bin_Str;
            
            //Memory ena
            fHandle << CGRA->PE_Array[i]->Get_WR_Ena(j, 1);
            fHandle << CGRA->PE_Array[i]->Get_WR_Ena(j, 0);

            //Memory addr
            int Port_Index[6]={3,4,5,0,1,2}; 
            Data_Width = 8;
            for(int l=0; l<6; l++){
                Dec_Data = CGRA->PE_Array[i]->Get_Mem_Port_Addr(j, Port_Index[l]);
                if(Dec_Data == NaN){
                    Dec_Data = 0;
                }
                Dec_To_Bin_Str(Dec_Data, Data_Width, Bin_Str);
                fHandle << Bin_Str;
            }

            //ALU Opcode
            Opcode Opcode_Tmp = CGRA->PE_Array[i]->Get_ALU_Opcode(j);
            Dec_Data = Opcode_To_Int(Opcode_Tmp);
            Data_Width = 4;
            Dec_To_Bin_Str(Dec_Data, Data_Width, Bin_Str);
            fHandle << Bin_Str;
            
            //Store mux
            Data_Width = 2;
            if(CGRA->Is_Load_PE(i) || CGRA->Is_Store_PE(i)){
                Dec_Data = CGRA->PE_Array[i]->Get_Store_Mux(j);
            }
            else{
                Dec_Data = 0;
            }
            Dec_To_Bin_Str(Dec_Data, Data_Width, Bin_Str);
            fHandle << Bin_Str; 

            //PE Output Mux
            for(int l=0; l<4; l++){
                Dec_Data = CGRA->PE_Array[i]->Get_Output_Mux(j, l);
                Data_Width = 2;
                Dec_To_Bin_Str(Dec_Data, Data_Width, Bin_Str);
                fHandle << Bin_Str;
            }

            fHandle << std::endl;
        }
        fHandle.close();
    }
}


void Scheduler::Addr_Buffer_Dump_Mem(){

    std::string fName = "./result/Addr-Buffer.mem";
    std::ofstream fHandle;
    fHandle.open(fName.c_str());
    if(!fHandle.is_open()){
        ERROR("Fail to create Addr-Buffer.mem\n");
    }

    char Char_Bin_Vec[100];
    for(int i=0; i<(CGRA->In_Buffer_Num+CGRA->Out_Buffer_Num); i++){
        int Start_Addr = (i * CGRA->Addr_Buffer_Depth * CGRA->Addr_Buffer_Width)/8;

        char Char_Hex_Addr[20];
        if(i==0){
            sprintf(Char_Hex_Addr, "@00000000");
        }
        else{
            sprintf(Char_Hex_Addr, "@%08X", Start_Addr);
        }
        fHandle << Char_Hex_Addr << std::endl;

        std::ostringstream os;
        os << "./result/" << "Addr-Buffer-" << i << ".coe";
        std::string fName1 = os.str();
        std::ifstream fHandle1;
        fHandle1.open(fName1.c_str());
        if(!fHandle1.is_open()){
            ERROR("Failed to open the Addr-Buffer.coe");
        }

        int Line_Num = 0;
        while(fHandle1.getline(Char_Bin_Vec,100)){
            Line_Num++;
            if(Line_Num<3){
                continue;
            }

            char Vec_Tmp0[100];
            char Vec_Tmp1[100];
            Vec_Tmp0[0] = '0';
            Vec_Tmp0[1] = '0';
            Vec_Tmp0[2] = '0';

            Vec_Tmp1[0] = '0';
            Vec_Tmp1[1] = '0';
            Vec_Tmp1[2] = '0';

            for(int k=3; k<12; k++){
                Vec_Tmp0[k] = Char_Bin_Vec[k+6];
                Vec_Tmp1[k] = Char_Bin_Vec[k-3];
            }

            for(int k=12; k<100; k++){
                Vec_Tmp0[k] = Char_Bin_Vec[18];
                Vec_Tmp1[k] = Char_Bin_Vec[18];
            }

            // Transform the bin to be hex
            for(int k=0; k<(CGRA->Addr_Buffer_Width/2+3)/4; k++){
                int Index = k*4;
                fHandle << Bin_To_Hex(&Vec_Tmp0[Index]);
            }
            fHandle << std::endl;

            for(int k=0; k<(CGRA->Addr_Buffer_Width/2+3)/4; k++){
                int Index = k*4;
                fHandle << Bin_To_Hex(&Vec_Tmp1[Index]);
            }
            fHandle << std::endl;

        }
        fHandle1.close();
    }
    fHandle.close();
}


void Scheduler::Inst_Mem_Dump_Mem(){

    std::string fName="./result/Inst.mem";
    std::ofstream fHandle;
    fHandle.open(fName.c_str());
    if(!fHandle.is_open()){
        ERROR("Failed to create Inst.mem\n");
    }

    char Char_Bin_Vec[100];
    for(int i=0; i<CGRA->CGRA_Scale; i++){
        int Start_Addr = (i * PE::Inst_Mem_Depth * PE::Inst_Mem_Width)/8;

        char Char_Hex_Addr[20];
        if(i==0){
            sprintf(Char_Hex_Addr, "@00000000");
        }
        else{
            sprintf(Char_Hex_Addr, "@%08X", Start_Addr);
        }
        fHandle << Char_Hex_Addr << std::endl;

        std::ostringstream os;
        os << "./result/PE-" << "inst-" << i << ".coe";
        std::string fName1 = os.str();
        std::ifstream fHandle1;
        fHandle1.open(fName1.c_str());
        if(!fHandle1.is_open()){
            ERROR("Failed to open the PE.coe");
        }

        if(PE::Inst_Mem_Depth==1024){
            int Line_Num = 0;
            while(fHandle1.getline(Char_Bin_Vec,100)){
                Line_Num++;
                if(Line_Num<3){
                    continue;
                }

                // ----------------------------------------------------------------------------------------
                // You can't initialize the ROM block correctly using the raw data. The following steps 
                // will show how the 72bit raw data should be reorganized before it goes to data2mem command. 
                // I figured it out by comparing the original data and the dumped data from bitstream. It 
                // took me quite a fucking long time.
                // ----------------------------------------------------------------------------------------

                // Exchange the higher 36bits and lower 36bits
                char Vec_Tmp0[100];
                char Vec_Tmp1[100];
                for(int k=0; k<PE::Inst_Mem_Width/2; k++){
                    Vec_Tmp0[k] = Char_Bin_Vec[k + PE::Inst_Mem_Width/2];
                    Vec_Tmp0[k + PE::Inst_Mem_Width/2] = Char_Bin_Vec[k];
                }

                // Exchanged data: (1bit, 8bit), (1bit, 8bit), ... The first bit will be stored in BRAM
                // parity section, while the following 8bit will be stored in BRAM data section.
                // Put the 4 parity bit together and leave the rest untouched. The new format should be
                // (4bit), (8bit), (8bit), ...
                int Basic_Len = 9;
                int Kmax = PE::Inst_Mem_Width/2/Basic_Len;
                for(int k=0; k<Kmax; k++){
                    Vec_Tmp1[k] = Vec_Tmp0[k*Basic_Len];
                    Vec_Tmp1[k + PE::Inst_Mem_Width/2] = Vec_Tmp0[k*Basic_Len + PE::Inst_Mem_Width/2];
                    for(int Index=1; Index<Basic_Len; Index++){
                        Vec_Tmp1[Kmax + k*(Basic_Len-1)+Index-1] = Vec_Tmp0[k*Basic_Len + Index];
                        Vec_Tmp1[Kmax + k*(Basic_Len-1)+Index-1+PE::Inst_Mem_Width/2] = Vec_Tmp0[k*Basic_Len + Index + PE::Inst_Mem_Width/2];
                    }
                }

                // Transform the bin to be hex
                for(int k=0; k<PE::Inst_Mem_Width/4; k++){
                    int Index = k*4;
                    fHandle << Bin_To_Hex(&Vec_Tmp1[Index]);
                }
                fHandle << std::endl;
            }

            fHandle1.close();
        }
        else if(PE::Inst_Mem_Depth == 512){
            int Line_Num=0;
            while(fHandle1.getline(Char_Bin_Vec, 100)){
                Line_Num++;
                if(Line_Num<3){
                    continue;
                }

                // Exchange the higher 36bits and lower 36bits
                char Vec_Tmp[100];

                // Exchanged data: (1bit, 8bit), (1bit, 8bit), ... The first bit will be stored in BRAM
                // parity section, while the following 8bit will be stored in BRAM data section.
                // Put the 4 parity bit together and leave the rest untouched. The new format should be
                // (4bit), (8bit), (8bit), ...
                int Basic_Len=9;
                int Kmax = PE::Inst_Mem_Width/Basic_Len;
                for(int k=0; k<Kmax; k++){
                    Vec_Tmp[k] = Char_Bin_Vec[k*Basic_Len];
                    for(int Index=1; Index<Basic_Len; Index++){
                        Vec_Tmp[Kmax+k*(Basic_Len-1)+Index-1] = Char_Bin_Vec[k*Basic_Len+Index];
                    }
                }

                // Transform the bin to be hex
                for(int k=0; k<PE::Inst_Mem_Width/4; k++){
                    int Index = k*4;
                    fHandle << Bin_To_Hex(&Vec_Tmp[Index]);
                }
                fHandle << std::endl;
            }
            fHandle1.close();
        }

    }

    fHandle.close();

}

/*
void Scheduler::DataMemoryInit(map<int, int> &OpToAddr, const int &PE_id, const int &memory_capacity){
    //Data memory initialization
    vector<int> memory_data;
    memory_data.resize(memory_capacity);
    for(int i=0; i<memory_capacity; i++){
        memory_data[i]=0;
    }

    //Get the initial value of data memory
    for(int i=0; i<GL_Var::maximum_operation_num; i++){
        if(OpToAddr.count(i)>0){
            int addr=OpToAddr[i];
            memory_data[addr]=DFG->OP_Array[i]->vertex_value;
        }
    }

    //Open a file to store initialized data of the data memory
    ostringstream os;
    os<<"./result/PE-"<<"mem-"<<PE_id<<".coe";
    string fName=os.str();
    ofstream fHandle;
    fHandle.open(fName.c_str());
    if(!fHandle.is_open()){
        ERROR("Fail to create the PE-mem.coe");
    }

    //Transform the decimal data to be binary data and store them in the file.
    vector<int>Bit_List;
    for(int i=0; i<memory_capacity; i++){
        int dec_data=memory_data[i];
        int width=16;
        while(dec_data!=1 && dec_data!=0){
            Bit_List.push_back(dec_data%2);
            dec_data=dec_data/2;
            width--;
        }
        Bit_List.push_back(dec_data);
        width--;
        while(width!=0){
            int tmp=0;
            Bit_List.push_back(tmp);
            width--;
        }

        vector<int>::reverse_iterator it;
        for(it=Bit_List.rbegin(); it!=Bit_List.rend(); it++){
            fHandle<<(*it);
        }
        fHandle<<endl;
        Bit_List.clear();
    }
}
*/

/*
void Scheduler::DataMemoryDumpMem(){
    const int DataMemDepth=1024;
    const int DataMemWidth=16;
    const int readWidth=20;

    string fMemName="./result/data.mem";
    ofstream fMemHandle;
    fMemHandle.open(fMemName.c_str());
    if(!fMemHandle.is_open()){
        ERROR("Fail to create data.mem\n");
    }

    char vec[readWidth];
    for(int i=0; i<CGRA->CGRA_Scale; i++){
        int intAddr=i*DataMemDepth*DataMemWidth/8;

        char hexAddr[20];
        if(i==0){
            sprintf(hexAddr, "@00000000");
        }
        else{
            sprintf(hexAddr, "@0000%X", intAddr);
        }
        fMemHandle << hexAddr <<endl;

        ostringstream os;
        os<<"./result/PE-"<<"mem-"<<i<<".coe";
        string fName=os.str();
        ifstream fHandle;
        fHandle.open(fName.c_str());
        if(!fHandle.is_open()){
            ERROR("Fail to open the PE-mem.coe");
        }

        while(fHandle.getline(vec,readWidth)){

            char mVec[DataMemWidth];
            for(int k=0; k<DataMemWidth; k++){
                mVec[k]=vec[k];
            }

            // Dump the bin to be hex
            for(int k=0; k<DataMemWidth/4; k++){
                int id=k*4;
                fMemHandle << Bin2Hex(&mVec[id]);
            }
            fMemHandle << endl;
        }
        fHandle.close();
    }
    fMemHandle.close();
}
*/

void Scheduler::Computation_Result_Dump(){
    
    std::ostringstream os;
    os << "./result/dst-op.txt";
    std::string fName = os.str();
    std::ofstream fHandle;
    fHandle.open(fName.c_str());
    if(!fHandle.is_open()){
        ERROR("Fail to create the dst-op.txt");
    }

    for(int i=0; i<DFG->OP_Num; i++){
        fHandle << DFG->OP_Array[i]->OP_ID << " ";
        fHandle << DFG->OP_Array[i]->OP_Val << " ";
        fHandle << std::endl;
    }

    fHandle.close();

}

void Scheduler::Load_IO_Mapping(std::vector<int> &Raw_Data, int &Row, int &Col){

    std::ostringstream oss; 
    oss << "./config/block-io.txt";
    std::string fName = oss.str();
    std::ifstream fHandle;
    fHandle.open(fName.c_str());
    if(!fHandle.is_open()){
        ERROR("%s open error!", fName.c_str());
    }

    while(!fHandle.eof()){
        if(fHandle.fail()){
            break;
        }

        int Tmp;
        fHandle >> Tmp;
        Raw_Data.push_back(Tmp);
    }

    fHandle.clear();
    fHandle.seekg(0, std::ios::beg);

    Row=0;
    std::string Unused;
    while(std::getline(fHandle, Unused)){
        Row++;
    }

    Col = Raw_Data.size()/Row;
    fHandle.close();

}

void Scheduler::Addr_Buffer_Dump_Coe(){

    // Load op->addr information to an array, each column indicates the corresponding iteration.
    // The first column represents the op id and the rest columns represent addr of different iterations.
    int Row, Col;
    std::vector<int> Raw_Data;
    Load_IO_Mapping(Raw_Data, Row, Col);
    std::map<int, int> OP_ID_To_Row_Index; //Map kernel op id to row index of the addr array.
    for(int i=0; i<Row; i++){
        OP_ID_To_Row_Index[Raw_Data[i*Col+0]] = i;
    }

    int IO_PE[2] = {0,1};
    for(int i=0; i<(CGRA->In_Buffer_Num+CGRA->Out_Buffer_Num); i++){

        std::ostringstream os;
        os << "./result/" << "Addr-Buffer-" << i << ".coe";
        std::string fName = os.str();
        std::ofstream fHandle;
        fHandle.open(fName.c_str());
        if(!fHandle.is_open()){
            ERROR("Fail to create the Addr-Buffer-.coe");
        }

        fHandle << "memory_initialization_radix=2;" << std::endl;
        fHandle << "memory_initialization_vector=" << std::endl;
        int IO_PE_ID = IO_PE[i];
        std::vector<unsigned int> IO_Buffer_Addr;
        IO_Buffer_Addr.resize(Scheduling_Complete_Time + 2);
        std::vector<int> IO_Activity; //0->load, 1->store, 2->idle
        IO_Activity.resize(Scheduling_Complete_Time + 2);

        for(int j=0; j<Scheduling_Complete_Time + 1; j++){
            IO_Buffer_Addr[j] = 0;
            IO_Activity[j] = 2;
        }

        for(int Kit=1; Kit<Col; Kit++){
            for(int j=1; j<Scheduling_Complete_Time; j++){

                bool Load_Active = !(CGRA->PE_Array[IO_PE_ID]->Is_Load_Path_Avail(j));
                int Load_Mux = CGRA->PE_Array[IO_PE_ID]->Get_Load_Mux(j);
                bool Store_Active = !(CGRA->PE_Array[IO_PE_ID]->Is_Store_Path_Avail(j));
                if(Load_Active && Load_Mux==0){
                    if(IO_Activity[j-1]==1){
                        ERROR("Unexpected load state!\n");
                    }
                    IO_Activity[j-1] = 0;
                    int Load_OP = CGRA->PE_Array[IO_PE_ID]->Get_Load_OP(j-1);
                    int Row_Index = OP_ID_To_Row_Index[Load_OP];
                    IO_Buffer_Addr[j-1] = Raw_Data[Row_Index * Col + Kit];
                }

                if(Store_Active){
                    if(IO_Activity[j] == 0){
                        ERROR("Unexpected store state!\n");
                    }
                    IO_Activity[j+2]=1;
                    int Store_OP = CGRA->PE_Array[IO_PE_ID]->Get_Store_OP(j);
                    int Row_Index = OP_ID_To_Row_Index[Store_OP];
                    IO_Buffer_Addr[j+2] = Raw_Data[Row_Index * Col + Kit];
                }

            }

            int Dec_Data;
            int Data_Width; 
            std::string Bin_Str;
            for(int j=0; j<Scheduling_Complete_Time+2; j++){

                if(IO_Activity[j]==0){
                    fHandle << "10";
                }
                else if(IO_Activity[j]==1){
                    fHandle << "11";
                }
                else{
                    fHandle << "00";
                }

                /* ----------------------------------------------------------------
                 * computation status.
                 * 001 kernel computation is done, and CPU will be acknowledged.
                 * 010 One iteration of the CGRA computation is done. The next iteration 
                 *     will continue after a few cycles' preparation (controlling delay).
                 * 100 CGRA computation is on going.
                 * --------------------------------------------------------------*/
                if(j==(Scheduling_Complete_Time+1) && Kit==Col-1){
                    fHandle << "001";
                }
                else if(j==(Scheduling_Complete_Time+1) && Kit<Col-1){
                    fHandle << "010";
                }
                else{
                    fHandle << "100"; // The CGRA kernel iterates only once.
                }

                //Transform decimal addr to 13-bit binary 
                Dec_Data = IO_Buffer_Addr[j];
                Data_Width = 13;
                if(Dec_Data==NaN){
                    Dec_Data=0;
                }
                Dec_To_Bin_Str(Dec_Data, Data_Width, Bin_Str);
                fHandle << Bin_Str;
                fHandle << std::endl;
            }

            //When the kernel iterates more than once, additional 5 lines should be added.
            if(Row>2 && Kit<Col-1){
                fHandle << "0010000000000000000" << std::endl;
                fHandle << "0010000000000000000" << std::endl;
                fHandle << "0010000000000000000" << std::endl;
                fHandle << "0010000000000000000" << std::endl;
                fHandle << "0010000000000000000" << std::endl;
                fHandle << "0010000000000000000" << std::endl;
            }

        }
        fHandle.close();

    }

}

char Scheduler::Bin_To_Hex(char* Bin_Vec){
    char Hex_Char;
    if(Bin_Vec[0]=='0' && Bin_Vec[1]=='0' && Bin_Vec[2]=='0' && Bin_Vec[3]=='0'){
        Hex_Char = '0';
    }
    else if(Bin_Vec[0]=='0' && Bin_Vec[1]=='0' && Bin_Vec[2]=='0' && Bin_Vec[3]=='1'){
        Hex_Char = '1';
    }
    else if(Bin_Vec[0]=='0' && Bin_Vec[1]=='0' && Bin_Vec[2]=='1' && Bin_Vec[3]=='0'){
        Hex_Char = '2';
    }
    else if(Bin_Vec[0]=='0' && Bin_Vec[1]=='0' && Bin_Vec[2]=='1' && Bin_Vec[3]=='1'){
        Hex_Char = '3';
    }
    else if(Bin_Vec[0]=='0' && Bin_Vec[1]=='1' && Bin_Vec[2]=='0' && Bin_Vec[3]=='0'){
        Hex_Char = '4';
    }
    else if(Bin_Vec[0]=='0' && Bin_Vec[1]=='1' && Bin_Vec[2]=='0' && Bin_Vec[3]=='1'){
        Hex_Char = '5';
    }
    else if(Bin_Vec[0]=='0' && Bin_Vec[1]=='1' && Bin_Vec[2]=='1' && Bin_Vec[3]=='0'){
        Hex_Char = '6';
    }
    else if(Bin_Vec[0]=='0' && Bin_Vec[1]=='1' && Bin_Vec[2]=='1' && Bin_Vec[3]=='1'){
        Hex_Char = '7';
    }
    else if(Bin_Vec[0]=='1' && Bin_Vec[1]=='0' && Bin_Vec[2]=='0' && Bin_Vec[3]=='0'){
        Hex_Char = '8';
    }
    else if(Bin_Vec[0]=='1' && Bin_Vec[1]=='0' && Bin_Vec[2]=='0' && Bin_Vec[3]=='1'){
        Hex_Char = '9';
    }
    else if(Bin_Vec[0]=='1' && Bin_Vec[1]=='0' && Bin_Vec[2]=='1' && Bin_Vec[3]=='0'){
        Hex_Char = 'A';
    }
    else if(Bin_Vec[0]=='1' && Bin_Vec[1]=='0' && Bin_Vec[2]=='1' && Bin_Vec[3]=='1'){
        Hex_Char = 'B';
    }
    else if(Bin_Vec[0]=='1' && Bin_Vec[1]=='1' && Bin_Vec[2]=='0' && Bin_Vec[3]=='0'){
        Hex_Char = 'C';
    }
    else if(Bin_Vec[0]=='1' && Bin_Vec[1]=='1' && Bin_Vec[2]=='0' && Bin_Vec[3]=='1'){
        Hex_Char = 'D';
    }
    else if(Bin_Vec[0]=='1' && Bin_Vec[1]=='1' && Bin_Vec[2]=='1' && Bin_Vec[3]=='0'){
        Hex_Char = 'E';
    }
    else{
        Hex_Char = 'F';
    }

    return Hex_Char;
}

int Scheduler::File_Line_Cnt(const std::string &fName){
    int Line_Cnt=0;
    std::ifstream fHandle;
    fHandle.open(fName.c_str());
    if(!fHandle.is_open()){
        ERROR("File open failed!\n");
    }
    char Line_Vec[200];
    while(fHandle.getline(Line_Vec, 200)){
        Line_Cnt++;
    }
    return Line_Cnt;
}

void Scheduler::Dec_To_Bin_Str(const int &Dec_Data, const int &Data_Width, std::string &Bin_Str){

    unsigned int Dec_Data_Tmp = (unsigned int)(Dec_Data);

    if(Data_Width==2){
        switch(Dec_Data_Tmp){
            case 0:
                Bin_Str = "00";
                break;
            case 1:
                Bin_Str = "01";
                break;
            case 2:
                Bin_Str = "10";
                break;
            case 3:
                Bin_Str = "11";
                break;
            default:
                ERROR("Unexpected decimal value!");
                break;
        }
    }
    else if(Data_Width==4){
        switch(Dec_Data_Tmp){
            case 0:
                Bin_Str = "0000";
                break;
            case 1:
                Bin_Str = "0001";
                break;
            case 2:
                Bin_Str = "0010";
                break;
            case 3:
                Bin_Str = "0011";
                break;
            case 4:
                Bin_Str = "0100";
                break;
            case 5:
                Bin_Str = "0101";
                break;
            case 6:
                Bin_Str = "0110";
                break;
            case 7:
                Bin_Str = "0111";
                break;
            case 8:
                Bin_Str = "1000";
                break;
            case 9:
                Bin_Str = "1001";
                break;
            case 10:
                Bin_Str = "1010";
                break;
            case 11:
                Bin_Str = "1011";
                break;
            case 12:
                Bin_Str = "1100";
                break;
            case 13:
                Bin_Str = "1101";
                break;
            case 14:
                Bin_Str = "1110";
                break;
            case 15:
                Bin_Str = "1111";
                break;
            default:
                ERROR("Unexpected Dec_Data!\n");
                break;
        }
    }
    else {
        int Data_Tmp = Dec_Data_Tmp;
        int Width_Tmp = Data_Width;
        if(Data_Tmp >= pow(2, Data_Width)){
            ERROR("Decimal data is too large for the given data width!\n");
        }
        std::list<int> Bit_List;
        std::list<int>::reverse_iterator Rit;

        while(Data_Tmp != 1 && Data_Tmp != 0){
            Bit_List.push_back(Data_Tmp%2);
            Data_Tmp = Data_Tmp/2;
            Width_Tmp--;
        }
        Bit_List.push_back(Data_Tmp);
        Width_Tmp--;

        while(Width_Tmp != 0){
            Bit_List.push_back(0);
            Width_Tmp--;
        }

        std::ostringstream os;
        for(Rit=Bit_List.rbegin(); Rit!=Bit_List.rend(); Rit++){
            os << (*Rit);
        }
        Bit_List.clear();
        Bin_Str = os.str();
    }

}

void Scheduler::IO_Buffer_Dump_Coe(){

    for(int i=0; i<(CGRA->In_Buffer_Num + CGRA->Out_Buffer_Num); i++){

        std::ostringstream oss;
        oss << "./result/IO-Buffer-"<<i<<".coe";
        std::string fName=oss.str();
        std::ofstream fHandle;
        fHandle.open(fName.c_str());
        if(!fHandle.is_open()){
            ERROR("Failed to create the IO-Buffer-.coe");
        }
        fHandle << "memory_initialization_radix=2;" << std::endl;
        fHandle << "memory_initialization_vector=" << std::endl;

        std::vector<Operand*>::iterator Vit;
        for(Vit = DFG->OP_Array.begin(); Vit != DFG->OP_Array.end(); Vit++){
            int Dec_Data = (*Vit)->OP_Val;
            int Data_Width = 32;
            if((*Vit)->IO_Buffer_ID == i){
                std::string Bin_Str;
                Dec_To_Bin_Str(Dec_Data, Data_Width, Bin_Str);
                fHandle << Bin_Str << std::endl;
            }
        }
        fHandle.close();

    }

}

/*
void Scheduler::IO_Buffer_Dump_Head_File(){

    std::ostringstream oss;
    oss << "./result/IO.h";
    std::string fName=oss.str();
    std::ofstream fHandle;
    fHandle.open(fName.c_str());
    if(!fHandle.is_open()){
        ERROR("Failed to create the IO.h");
    }

    fHandle << "#ifndef _IO_H_" << std::endl;
    fHandle << "#define _IO_H_" << std::endl << std::endl;

    // Const input
    int Const_Cnt = 0;
    int Invar_Cnt = 0;
    int Out_Cnt = 0;
    std::vector<Operand*>::iterator Vit;
    for(Vit = DFG->OP_Array.begin(); Vit != DFG->OP_Array.end(); Vit++){
        if((*Vit)->OP_Type == INCONST){
            Const_Cnt++;
        }
        if((*Vit)->OP_Type == INVAR){
            Invar_Cnt++;
        }
        if((*Vit)->OP_Type == OUTVAR || (*Vit)->OP_Type == IMOUT){
            Out_Cnt++;
        }
    }

    int Cnt = 0;
    fHandle << "int Const_In[" << Const_Cnt <<"] = { " ;
    for(Vit = DFG->OP_Array.begin(); Vit != DFG->OP_Array.end(); Vit++){
        Cnt++;
        if((*Vit)->OP_Type == INCONST && Cnt == Const_Cnt){
            fHandle << (*Vit)->OP_Val;
        }
        else if((*Vit)->OP_Type == INCONST){
            fHandle << (*Vit)->OP_Val << ", ";
        }
    }
    fHandle << " };" << std::endl;

    fHandle << "int Data_In[" << Invar_Cnt << "];" << std::endl;
    fHandle << "int Data_Out[" << Out_Cnt << "];" << std::endl << std::endl;
    fHandle << "#endif" << std::endl;
    fHandle.close();

}
*/
