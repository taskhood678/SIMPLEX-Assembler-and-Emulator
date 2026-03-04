#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <cstdlib>
#include <iomanip>
using namespace std;

struct instruction{
    int opcode; 
    bool use_operand;
    //Check for if we use an operand for an instruction
};

//global instruction table (Opcode = -1 for "data" and -2 for "SET" since they are special instructions)
const map<string, instruction> inst_table = {
    {"data",   {-1, true}}, 
    {"ldc",    { 0, true}}, 
    {"adc",    { 1, true}}, 
    {"ldl",    { 2, true}}, 
    {"stl",    { 3, true}}, 
    {"ldnl",   { 4, true}}, 
    {"stnl",   { 5, true}}, 
    {"add",    { 6, false}}, 
    {"sub",    { 7, false}}, 
    {"shl",    { 8, false}}, 
    {"shr",    { 9, false}}, 
    {"adj",    { 10, true}}, 
    {"a2sp",   { 11, false}}, 
    {"sp2a",   { 12, false}}, 
    {"call",   { 13, true}}, 
    {"return", { 14, false}}, 
    {"brz",    { 15, true}}, 
    {"brlz",   { 16, true}}, 
    {"br",     { 17, true}}, 
    {"HALT",   { 18, false}}, 
    {"SET",    {-2, true}}
};

//global symbol table to store label names and their memory addresses
map<string, int> symbol_table;

//function to remove comments and white spaces
string clean(string line){
    //remove comments
    size_t comment = line.find(';');
    if(comment != string::npos){
        line = line.substr(0, comment);
    }

    //remove leading whitespaces
    size_t start = line.find_first_not_of(" \t\r\n");
    if(start == string::npos) return ""; //entire line is whitespace

    //remove trailing whitespaces
    size_t end = line.find_last_not_of(" \t\r\n");
    return line.substr(start, end-start+1);
}

//First pass logic
bool first_pass(string& filename, ostream& log){
    ifstream file(filename);
    if(!file.is_open()){
        log<<"Error: Could Not Open The File "<<filename<<"\n";
        return false;
    }

    string line;
    int pc=0, line_num=0;
    //pc: Program Counter
    bool error=false;

    while(getline(file, line)){
        line_num++;
        line=clean(line);

        if(line.empty()) continue;

        string label="";
        size_t colon=line.find(':');
        if(colon!=string::npos){
            label=line.substr(0, colon);

            size_t endspace = label.find_last_not_of(" \t");
            if(endspace!=string::npos) label=label.substr(0, endspace+1);

            line = line.substr(colon+1);
        }

        stringstream ss(line);

        if(label!=""){
            //runs if label is found
            if(!isalpha(label[0])){
                //first character of label is not an alphabet
                log<<"Error at line "<<line_num<<": Bogus Label Name '"<<label<<"'\n";
                error=true;
            } else {
                bool valid=true;
                for(auto i: label){
                    if(!isalnum(i)){
                        //some character within label is not alphanumeric
                        log<<"Error at line "<<line_num<<": Bogus Label Name '"<<label<<"'\n";
                        error=true;
                        valid=false;
                        break;
                    }
                }
                if(valid && symbol_table.count(label)){
                    //duplicate label error
                    log<<"Error at line "<<line_num<<": Duplicate Label Definition '"<<label<<"'\n";
                    error=true;
                } else if(valid && !symbol_table.count(label)){
                    symbol_table[label]=pc;
                }
            }

            string second;
            if(ss>>second){
                //updates pc if instruction found just after label declaration
                if(inst_table.count(second)){
                    if(second!="SET") pc++;
                    else {
                        string operand;
                        if(ss>>operand){
                            char* endptr;
                            long value = strtol(operand.c_str(), &endptr, 0);
                            //check if operand is valid
                            if(*endptr == '\0') symbol_table[label] = value;
                            else {log<<"Error at line "<<line_num<<": Invalid SET operand\n"; error=true;}
                        } else {
                            log<<"Error at line "<<line_num<<": SET missing operand\n";
                            error=true;
                        }
                    }
                } else {
                    log<<"Error at line "<<line_num<<": Unknown Instruction\n";
                    error=true;
                }
            }
        } else {
            string first;
            ss>>first;
            //instruction line found; updates pc or gives error message accordingly
            if(inst_table.count(first)){
                if(first!="SET") pc++;
                else {log<<"Warning at line "<<line_num<<": SET instruction used without label\n";}
            } else {
                log<<"Error at line "<<line_num<<": Unknown Instruction\n";
                error=true;
            }
        }
    }

    file.close();
    return !error;
}

bool second_pass(string& filename, string& object_name, string& lstname, ostream& log){
    map<string, bool> used_label;

    ifstream infile(filename);
    ofstream objfile(object_name, ios::binary);
    ofstream lstfile(lstname);

    if(!infile.is_open()) {log<<"Error: Could Not Open Input File\n"; return false;}
    if(!objfile.is_open()) {log<<"Error: Could Not Open Binary Object File\n"; return false;}
    if(!lstfile.is_open()) {log<<"Error: Could Not Open Listing File\n"; return false;}

    string line;
    int pc=0, line_num=0;
    bool error=false;

    while(getline(infile, line)){
        line_num++;
        string orig=line;

        line=clean(line);
        if(line.empty()) continue;

        size_t colon=line.find(':');
        if(colon!=string::npos){
            line=line.substr(colon+1);
        }

        stringstream ss(line);
        string word;

        if(!(ss>>word)){
            //If a label doesn't have any instruction after it, then save line directly in listing file
            lstfile << hex << uppercase << setfill('0') << setw(8) << pc << "        " << orig << "\n";
            continue;
        }

        if(word=="SET"){
            lstfile<<"                 "<<orig<<"\n";
            continue;
        }

        if(inst_table.count(word)){
            instruction inst = inst_table.at(word);
            int operand=0;

            if(inst.use_operand){
                string operandstr;
                if(ss>>operandstr){
                    //an operand found
                    if(isalpha(operandstr[0])){
                        //operand is a label
                        if(symbol_table.count(operandstr)){
                            operand=symbol_table[operandstr];
                            used_label[operandstr]=true;
                        } else {
                            log<<"Error at line "<<line_num<<": Undefined label "<<operandstr<<"\n";
                            error=true;
                        }

                        if(inst.opcode==13 || (inst.opcode>=15 && inst.opcode<=17)){
                            operand=operand-(pc+1);
                        }
                    } else {
                        //operand is a number
                        char* endptr;
                        operand=strtol(operandstr.c_str(), &endptr, 0);

                        if(*endptr!='\0'){
                            log<<"Error at line "<<line_num<<": Invalid Numeric Operand '"<<operandstr<<"'\n";
                            error=true;
                        }
                    }
                } else {
                    log<<"Error at line "<<line_num<<": Missing Operand Value\n";
                    error=true;
                }
            }

            string extra;
            if(ss>>extra){
                log<<"Error at line "<<line_num<<": Unexpected extra operand '"<<extra<<"'\n";
                error=true;
            }

            //generate machine code
            unsigned int code=0;

            if(inst.opcode==-1) code=operand;
            else{
                //bitwise AND operation used to avoid conflict between negative operand and opcode
                code = ((operand&0xFFFFFF)<<8) | (inst.opcode&0xFF);
            }
            
            //Entering machine code in listing and object files
            lstfile << hex << uppercase << setfill('0') << setw(8) << pc << " " << setw(8) << code << " " << orig << "\n";
            objfile.write(reinterpret_cast<const char*>(&code), sizeof(code));
            pc++;
        } else {
            log<<"Error at line "<<line_num<<": Unknown Instruction '"<<word<<"'\n";
            error=true;
        }
    }

    for(auto it: symbol_table){
        if(!used_label[it.first]){
            log<<"Warning. Unused Label '"<<it.first<<"' Declared\n";
        }
    }

    infile.close();
    objfile.close();
    lstfile.close();
    return !error;
}


int main(){
    string inputfile;
    cout<<"Enter the name of the assembly file to process (include .asm at the end): ";
    cin>>inputfile;

    size_t dot = inputfile.find_last_of('.');
    while(dot == string::npos){
        cout<<"Incorrect file name format. Please try again\n";
        cout<<"Enter the name of the assembly file to process (include .asm at the end): ";
        cin>>inputfile;
        
        dot = inputfile.find_last_of('.');
    }

    string basename = inputfile.substr(0, dot);

    string objfile = basename+".obj";
    string lstfile = basename+".lst";
    string logname = basename+".log";

    ofstream logfile(logname);

    cout<<"\n"<<"Assembling "<<inputfile<<"...\n\n";

    cout<<"Executing First Pass...\n";
    bool pass1 = first_pass(inputfile, logfile);

    cout<<"\n";
    cout<<"Executing Second Pass...\n";
    bool pass2 = second_pass(inputfile, objfile, lstfile, logfile);

    cout<<"\n";
    if(!pass1 || !pass2){
        cerr<<"Assembler aborted due to errors in the source code\n\n";
        logfile<<"Assembler aborted due to errors in the source code\n\n";
        remove(objfile.c_str());
        remove(lstfile.c_str());
        return 1;
    }

    cout<<"Assembly Successfull!\n";
    cout<<"Object File Generated: "<<objfile<<"\n";
    cout<<"Listing File Generated: "<<lstfile<<"\n";
}