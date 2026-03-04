#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>

using namespace std;

//The 4 Simplex registers
int A=0, B=0, PC=0, SP=0;

//64KB of virtual memory
const int mem_size = 65536;
vector<int> memory(mem_size, 0);

bool obj_process(string &filename, ostream &out){
    ifstream file(filename, ios::binary);
    if(!file.is_open()){
        cerr<<"Error: Could not open the file\n";
        out<<"Error: Could not open the file\n";

        return false;
    }

    int inst, idx=0;
    while(file.read(reinterpret_cast<char*> (&inst), sizeof(inst))){
        if(idx>=mem_size){
            cerr<<"Error: Program exceeds memory limit (64KB)\n";
            out<<"Error: Program exceeds memory limit (64KB)\n";

            return false;
        }
        memory[idx]=inst;
        idx++;
    }

    file.close();
    return true;
}

void memory_dump(ostream &out){
    out<<"\n------Memory Dump------\n";
    for(int i=0; i<mem_size; i++){
        if(memory[i]!=0){
            out<<hex<<uppercase<<setfill('0')<<setw(8)<<i<<" : "<<setw(8)<<memory[i]<<dec<<"\n";
        }
    }
    out<<"------------------------\n\n";
}

bool valid_addr(int addr, ostream &out){
    if(addr<0 || addr>=mem_size){
        cerr<<"\nExecution Error: Segmentation Fault. Invalid Memory Access At Address "<<addr<<" (PC: "<<PC-1<<").\n";
        out<<"\nExecution Error: Segmentation Fault. Invalid Memory Access At Address "<<addr<<" (PC: "<<PC-1<<").\n";

        return false;
    }

    return true;
}

void execute(ostream &out){
    int ct=0;

    while(true){
        if(PC<0 || PC>=mem_size){
            cerr<<"\nExecution Error: Program Counter Out Of Bounds ("<<PC<<").\n";
            out<<"\nExecution Error: Program Counter Out Of Bounds ("<<PC<<").\n";
            break;
        }

        //get instruction from virtual memory
        int inst=memory[PC];

        int opcode=inst&0xff, operand=inst>>8;

        //increment PC implicitly before performing action
        PC++;

        switch (opcode) {
            case 0:
                //ldc
                B = A; 
                A = operand;
                break;
            case 1:
                //adc
                A = A + operand;
                break;
            case 2:
                //ldl
                if(!valid_addr(SP+operand, out)) return;
                B = A;
                A = memory[SP + operand];
                break;
            case 3:
                //stl
                if(!valid_addr(SP+operand, out)) return;
                memory[SP + operand] = A;
                A = B;
                break;
            case 4:
                //ldnl
                if(!valid_addr(A+operand, out)) return;
                A = memory[A + operand];
                break;
            case 5:
                //stnl
                if(!valid_addr(A+operand, out)) return;
                memory[A + operand] = B;
                break;
            case 6:
                //add
                A = B + A;
                break;
            case 7:
                //sub
                A = B - A;
                break;
            case 8:
                //shl
                if (A<0 || A>31) {
                    cerr<<"\nExecution Error: Invalid left shift amount ("<<A<<") at PC "<<(PC - 1)<<".\n";
                    out<<"\nExecution Error: Invalid left shift amount ("<<A<<") at PC "<<(PC - 1)<<".\n";

                    return; 
                }
                A = B << A;
                break;
            case 9:
                //shr
                if (A<0 || A>31) {
                    cerr<<"\nExecution Error: Invalid right shift amount ("<<A<<") at PC "<<(PC - 1)<<".\n";
                    out<<"\nExecution Error: Invalid right shift amount ("<<A<<") at PC "<<(PC - 1)<<".\n";

                    return; 
                }
                A = B >> A;
                break;
            case 10:
                //adj
                SP = SP + operand;
                break;
            case 11:
                //a2sp
                SP = A;
                A = B;
                break;
            case 12:
                //sp2a
                B = A;
                A = SP;
                break;
            case 13:
                //call
                B = A;
                A = PC;
                PC = PC + operand;
                break;
            case 14:
                //return
                PC = A;
                A = B;
                break;
            case 15:
                //brz
                if (A == 0) {
                    PC = PC + operand;
                }
                break;
            case 16:
                //brlz
                if (A < 0) {
                    PC = PC + operand;
                }
                break;
            case 17:
                //br
                PC = PC + operand;
                break;
            case 18:
                //HALT
                cout << "Program execution halted gracefully.\n";
                cout << "Total instructions executed: " << ct << "\n\n";

                out << "Program execution halted gracefully.\n";
                out << "Total instructions executed: " << ct << "\n\n";
                return;
            default:
                cerr << "\nExecution Error: Unknown opcode " << opcode << " at PC " << (PC - 1) << "\n";
                out << "\nExecution Error: Unknown opcode " << opcode << " at PC " << (PC - 1) << "\n";

                return;
        }
        ct++;
    }
}

int main(){
    string inputfile;
    cout<<"Enter the name of the binary object file (including the .obj extension): ";
    cin>>inputfile;

    while(inputfile.length() < 4 || inputfile.substr(inputfile.length() - 4) != ".obj"){
        cout<<"Incorrect File Name. Please Try Again\n";
        cout<<"Enter the name of the binary object file (including the .obj extension): ";
        cin>>inputfile;
    }

    size_t dot = inputfile.find_last_of('.');
    string logname = inputfile.substr(0, dot) + ".log";
    ofstream logfile(logname);

    cout<<"\n";
    cout<<"Loading "<<inputfile<<"...\n\n";

    if(!obj_process(inputfile, logfile)){
        cerr<<"Emulator aborted due to file loading error\n\n";
        logfile<<"Emulator aborted due to file loading error\n\n";
        return 1;
    }

    cout<<"Executing Program...\n\n";
    execute(logfile);

    char dump;
    cout<<"Would You Like To See The Memory Dump? (Y/N): ";
    cin>>dump;

    if(dump=='y'||dump=='Y'){
        memory_dump(cout);
    }
    memory_dump(logfile);

    cout<<"Final Register States: "<<"\n";
    cout<<"A: "<<A<<"\n";
    cout<<"B: "<<B<<"\n";
    cout<<"PC: "<<PC<<"\n";
    cout<<"SP: "<<SP<<"\n";

    logfile<<"Final Register States: "<<"\n";
    logfile<<"A: "<<A<<"\n";
    logfile<<"B: "<<B<<"\n";
    logfile<<"PC: "<<PC<<"\n";
    logfile<<"SP: "<<SP<<"\n";
    return 0;
}

