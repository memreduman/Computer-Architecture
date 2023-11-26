#include "assembler.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <sstream>
#include <regex>
#include <algorithm>
#include <cctype>
#include <bitset>
#include <iomanip>

#define DATA_MEMORY_START (uint32_t) 0x00000000
#define INSTRUCTION_MEMORY_START (uint32_t) 0x00000000

#pragma warning(disable : 4996) //disabled strcpy error for next line


instructions inst[] = {
    //R-type instructions
    {"add","R",1,0 },{"sub","R",1,1 },{"mul","R",1,2 },{"sll","R",1,3 },{"srl","R",1,4 },{"sra","R",1,5 },{"xor","R",1,6 },
    {"and","R",1,7 },{"or","R",1,8 },{"ma","R",1,9 },
    //I-type instructions
    {"addi","I",2,-1},{"subi","I",3,-1},{"muli","I",4,-1},{"slli","I",5,-1},{"srli","I",6,-1},{"srai","I",7,-1},
    {"xori","I",8,-1},{"andi","I",9,-1},{"ori","I",10,-1},
    {"lw","I",11,-1},{"lh","I",12,-1},{"lb","I",13,-1},{"lhu","I",14,-1},{"lbu","I",15,-1},{"lth","I",16,-1},
    {"jalr","I",17,-1},
    //SB-type instructions
    {"beq","SB",18,-1},{"bne","SB",19,-1},{"bge","SB",20,-1},{"blt","SB",21,-1},{"sw","SB",22,-1},{"sb","SB",23,-1},
    {"sh","SB",24,-1},{"saj","SB",25,-1},
    //J-type instructions
    {"jal","J",26,-1}
};

//// This structs defines how many registers need for each instruction type, to avoid syntax error 
struct inst_details {
    char type[5];
    int expected_register;
};

inst_details i_details[4] = {
    {"R",3},
    {"I",2},
    {"SB",2},
    {"J",1},
};
///

uint32_t inst_count=sizeof(inst)/sizeof(instructions);

const char* registers[] = {
    "x0",
    "x1",
    "x2",
    "x3",
    "x4",
    "x5",
    "x6",
    "x7",
    "x8",
    "x9",
    "x10",
    "x11",
    "x12",
    "x13",
    "x14",
    "x15",
    "x16",
    "x17",
    "x18",
    "x19",
    "x20",
    "x21",
    "x22",
    "x23",
    "x24",
    "x25",
    "x26",
    "x27",
    "x28",
    "x29",
    "x30",
    "x31"
};



uint32_t datasection_index=0;
uint32_t datasection[100];

uint32_t labelindex=0;
label label_list[100];

uint32_t instruction_memory_address[500];
uint32_t instr_index = 0;

bool comments_removed_flag=0;
bool data_section_extracted_flag=0;
bool labes_extracted_flag=0;

using namespace std;



void make_lower_case(const string& inputFilename, const string& outputFilename) {

    ifstream inputFile(inputFilename);
    ofstream outputFile(outputFilename);
    if (!inputFile.is_open()) {
        cerr << "Error: Unable to open input file: " << inputFilename << endl;
        return;
    }

    if (!outputFile.is_open()) {
        cerr << "Error: Unable to open output file: " << outputFilename << endl;
        return;
    }

    string line;
    while (getline(inputFile, line)) {
        transform(line.begin(), line.end(), line.begin(), ::tolower);
        outputFile << line << endl;
    }

    inputFile.close();
    outputFile.close();


}

void removeSpacesFromFile(const string& inputFilename, const string& outputFilename) {
    ifstream inputFile(inputFilename);
    ofstream outputFile(outputFilename);
    if (!inputFile.is_open()) {
        cerr << "Error: Unable to open input file: " << inputFilename << endl;
        return;
    }

    if (!outputFile.is_open()) {
        cerr << "Error: Unable to open output file: " << outputFilename << endl;
        return;
    }

    char ch;
    while (inputFile.get(ch)) {
        if (ch != ' ') {
            outputFile.put(ch);
        }
    }

    inputFile.close();
    outputFile.close();
}

void removeCommentsFromFile(const string& inputFilename, const string& outputFilename)
{
    ifstream inputFile;
    ofstream outputFile;
    inputFile.open(inputFilename);
    outputFile.open(outputFilename);
    string line;

    if (!inputFile.is_open()) {
        cerr << "Error: Unable to open input file: " << inputFilename << endl;
        return;
    }

    if (!outputFile.is_open()) {
       cerr << "Error: Unable to open output file: " << outputFilename << endl;
        return;
    }

    
    while (getline(inputFile, line)) {
        size_t commentStart = line.find("//");

        if (commentStart != string::npos) {
            line.erase(commentStart);
        }

        if (!line.empty()) {
            outputFile << line << endl;
        }
    }

    inputFile.close();
    outputFile.close();
    
    ifstream inputFile2;
    ofstream outputFile2;
    inputFile2.open(outputFilename);
    string debug = outputFilename + "_debug";
    outputFile2.open(debug);

    if (!inputFile2.is_open()) {
        cerr << "Error: Unable to open input file: " << inputFilename << endl;
        return;
    }

    if (!outputFile2.is_open()) {
        cerr << "Error: Unable to open output file: " << outputFilename << endl;
        return;
    }

    size_t commentStart;
    size_t commentEnd;
    bool in_comment_section = false;

    while (getline(inputFile2, line)) {
        line = string(line).erase(0, line.find_first_not_of(" \t\n\r\f\v"));
        line = string(line).erase(line.find_last_not_of(" \t\n\r\f\v") + 1);

        commentStart = line.find("/*");
        if (commentStart != string::npos) {

            commentEnd = line.find("*/");
            if (commentEnd != string::npos) {
                line.erase(commentStart, ((commentEnd + 2) - commentStart));
            }
            else {
                in_comment_section = true;
                continue;
            }

        }
        commentEnd = line.find("*/");
        if (in_comment_section) {
            if (commentEnd != string::npos) {
                line.erase(0, (commentEnd + 2));
                in_comment_section = false;
            }
            else {
                continue;
            }

        }

        if (!line.empty()) {
            outputFile2 << line << endl;
        }
    }
    inputFile2.close();
    outputFile2.close();

    remove(outputFilename.c_str());
    rename(debug.c_str(), outputFilename.c_str());
    comments_removed_flag = true;
    
}

void findExactWord(const string& inputFilename, const string& outputFilename) {
    ifstream inputFile(inputFilename);
    ofstream outputFile(outputFilename);

    if (!inputFile.is_open()) {
        cerr << "Error: Unable to open input file: " << inputFilename << endl;
        return;
    }

    if (!outputFile.is_open()) {
        cerr << "Error: Unable to open output file: " << outputFilename << endl;
        return;
    }

    string line;
    string word;
    string search_word;
    size_t commentStart;
    size_t commentEnd;
    bool in_comment_section = false;
    while (getline(inputFile, line)) {
        line = string(line).erase(0, line.find_first_not_of(" \t\n\r\f\v"));
        line = string(line).erase(line.find_last_not_of(" \t\n\r\f\v") + 1);

        commentStart = line.find("/*");
        if (commentStart != string::npos) {

            commentEnd = line.find("*/");
            if (commentEnd != string::npos) {
                line.erase(commentStart, ((commentEnd + 2) - commentStart));
            }
            else {
                in_comment_section = true;
                continue;
            }
            
        }
        commentEnd = line.find("*/");
        if (in_comment_section) {
            if (commentEnd != string::npos) {
                line.erase(0, (commentEnd + 2));
                in_comment_section = false;
            }
            else {
                continue;
            }

        }

        if(!line.empty()) {
            outputFile << line << endl;
        }
    }
    inputFile.close();
    outputFile.close();
    
}

void find_directives_data(const string& inputFilename, const string& outputFilename) {

    if (!comments_removed_flag) {
        cerr << "Error in find_directives_data function" << endl;
        cerr << "First use remove comments function !" << endl;
        exit(1);
    }


    ifstream inputFile(inputFilename);
    ofstream outputFile(outputFilename);

    if (!inputFile.is_open()) {
        cerr << "Error: Unable to open input file: " << inputFilename << endl;
        return;
    }

    if (!outputFile.is_open()) {
        cerr << "Error: Unable to open output file: " << outputFilename << endl;
        return;
    }

    string line;
    string word;
    bool in_data_section = false;

    while (getline(inputFile, line)) {
        // Remove leading and trailing whitespace from the line
        line = string(line).erase(0, line.find_first_not_of(" \t\n\r\f\v"));
        line = string(line).erase(line.find_last_not_of(" \t\n\r\f\v") + 1);

        if (line.empty()) {
            // Skip empty lines
            continue;
        }

        if (line.find(".data") != string::npos) {
            in_data_section = true;
            continue;
        }
        else if (line.find(".text") != string::npos) {
            // .data section is over
            in_data_section = false;
        }
        // Find labels inside data section
        else if (in_data_section && line.find(":")!= string::npos) {  
             // Remove whitespace from the line
            line.erase(std::remove_if(line.begin(), line.end(), [](char c) {
                return std::isspace(c);
                }), line.end());
             size_t end_label_pos = line.find(":");
             string temp_name = line.substr(0, end_label_pos);
             strcpy(label_list[labelindex].name, temp_name.c_str());
             label_list[labelindex].address = DATA_MEMORY_START + (datasection_index) * 4;
             labelindex++;

        }
        // Find . directives
        else if ( (in_data_section==true) && ( line[0] == '.') ) {
            istringstream iss(line);
            while (iss >> word) {
                if (word == ".word") {
                    if (line.find("0x") != string::npos) {
                        string hex_str;
                        iss >> hex_str;
                        datasection[datasection_index++] = stoi(hex_str, nullptr, 16);
                    }
                    else if (line.find("0b") != string::npos) {
                        string bin_str;
                        iss >> bin_str;
                        bin_str.erase(0, 2);
                        datasection[datasection_index++] = stoi(bin_str, nullptr, 2);
                    }
                    else  iss >> datasection[datasection_index++];
                   
                }
                // if(word == ".byte")
            }
        }
        // Quit if there is no "." on next line
        else if (line[0] != '.') {
            in_data_section = false;
        }

        if (!in_data_section) {
            outputFile << line << endl;
        }
    }
    inputFile.close();
    outputFile.close();
    data_section_extracted_flag = true;
}

void find_labels_text(const string& inputFilename, const string& outputFilename) {
    ifstream inputFile(inputFilename);
    ofstream outputFile(outputFilename);

    if (!inputFile.is_open()) {
        cerr << "Error: Unable to open input file: " << inputFilename << endl;
        return;
    }

    if (!outputFile.is_open()) {
        cerr << "Error: Unable to open output file: " << outputFilename << endl;
        return;
    }

    
    string line;
    bool in_text_section = false;
    uint32_t pc_count = INSTRUCTION_MEMORY_START;
    while (getline(inputFile, line)) {
        // Remove leading and trailing whitespace from the line
        line = string(line).erase(0, line.find_first_not_of(" \t\n\r\f\v"));
        line = string(line).erase(line.find_last_not_of(" \t\n\r\f\v") + 1);

        if (line.empty()) {
            // Skip empty lines
            continue;
        }

        if (line == ".text") {
            in_text_section = true;
        }
        // Find labels inside data section
        else if (in_text_section && line.find(":") != string::npos) {
            // Remove whitespace from the line
            //line.erase(remove_if(line.begin(), line.end(), isspace), line.end());
            size_t end_label_pos = line.find(":");
            string temp_name = line.substr(0, end_label_pos);
            strcpy(label_list[labelindex].name, temp_name.c_str());
            label_list[labelindex].address =  (pc_count) * 4;
            labelindex++;

            if (line.length() > (end_label_pos+1)) {
                //string label = line.substr(0, end_label_pos + 1);
                string rest = line.substr(end_label_pos + 1);
                //Remove leading and trailing whitespace from the line
                rest = string(rest).erase(0, rest.find_first_not_of(" \t\n\r\f\v"));
                rest = string(rest).erase(rest.find_last_not_of(" \t\n\r\f\v") + 1);
                outputFile << rest << "\n";
                pc_count++;
            }

                


        }
        else {
            pc_count++;
            outputFile << line << endl;
        }
    }

    inputFile.close();
    outputFile.close();
    labes_extracted_flag = true;
}

void insert_labels_addresses(const string& inputFilename, const string& outputFilename) {
    ifstream inputFile(inputFilename);
    ofstream outputFile(outputFilename);

    if (!inputFile.is_open()) {
        cerr << "Error: Unable to open input file: " << inputFilename << endl;
        return;
    }

    if (!outputFile.is_open()) {
        cerr << "Error: Unable to open output file: " << outputFilename << endl;
        return;
    }

    string line;
    string word;
    uint32_t pc_count = INSTRUCTION_MEMORY_START;
    while (getline(inputFile, line))
    {
        if (line.empty())
        {
            // Skip empty lines
            continue;
        }

        //Insert whitespace after each comma if there is not
        for (size_t i = 0; i < line.size(); i++) {
            if (line[i] == ',' && i < line.size() - 1 && line[i + 1] != ' ') {
                line.insert(i + 1, " ");
            }
        }


        
        // Divide the line into tokens
        istringstream iss(line);
        vector<string> tokens;
        string token;
        while (iss >> token) {
            tokens.push_back(token);
        }
        for (int i = 0; i < (labelindex + 1); i++) {
            bool label_found = std::find(tokens.begin(), tokens.end(), label_list[i].name) != tokens.end();
            for (auto& token : tokens) {
                if (token == label_list[i].name) {
                    //string temp_address = to_string(label_list[i].address);
                    int relative_address=0;
                    if (label_list[i].address > pc_count) {
                        relative_address = (int)((pc_count+4) - label_list[i].address) / 4;
                    }
                    else if(label_list[i].address < pc_count){
                        relative_address = (int)(label_list[i].address - (pc_count+4)) / 4;
                    }
                    else {
                        relative_address = 0;
                    }

                    string temp_address = to_string(relative_address);
                    token = temp_address;
                }
            }
        }
        for (const auto& token : tokens) {
           outputFile << token << ' ';
        }
        outputFile << endl;
        pc_count+=4;
    }

    inputFile.close();
    outputFile.close();
}

//BURADA KALDIM
// Syntax errorları eklemelisin onun dışında syntax doğruysa binarye çeviriyor
void assemble_line(const std::string& inputFilename, const std::string& outputFilename) {
    ifstream inputFile(inputFilename);
    ofstream outputFile(outputFilename);

    if (!inputFile.is_open()) {
        cerr << "Error: Unable to open input file: " << inputFilename << endl;
        return;
    }

    if (!outputFile.is_open()) {
        cerr << "Error: Unable to open output file: " << outputFilename << endl;
        return;
    }

        /*
        *  All the comments, directives and etc are removed until here,so
        *  Input file only consists of instructions.
        *  Therefore, the syntax check can be done in here !
        *  Do syntax check !
        *  Each line mush have instruction if the any instruction cannot be detected, there must be syntax error !
        *  Instruction syntax checking is done ! OK !
        *  Register name checking is done ! OK !
        *  Imm17 and Imm22 need to be checked !
        */

        // Erase the commas in the line
        //line.erase(std::remove(line.begin(), line.end(), ','), line.end());

    string line;
    string word;
    uint32_t number_line = 1;
    while (getline(inputFile,line)) {

        if (line.empty())
        {
            // Skip empty lines
            continue;
        }

        // Erase the commas in the line
        line.erase(std::remove(line.begin(), line.end(), ','), line.end());

        istringstream iss(line);
        vector<string> assembled_line;
        string token;
        bool instruction_detected = false;
        int required_register_number = NULL;
        int l = 0;
        string temp_token;
        //Take first token and find the matched instruction   
        iss >> token;
        for (int i = 0; i < inst_count; i++){

            if (!token.compare(inst[i].name)) {
                instruction_detected = true;
                //Take the insturction opcode convert it to bit as a string and push back to assembled line
                assembled_line.push_back(bitset<7>(inst[i].opcode).to_string());
                
                for (int j = 0; j< 4 ; j++) { 
                //Detect how many registers used in instruction
                    if ( !(strcmp(inst[i].type, i_details[j].type)) ) {
                        required_register_number = i_details[j].expected_register;
                    }
                    
                    
                }

                if (required_register_number == NULL) {
                    cerr << "The assemble line function could not find the required register number" << endl;
                    inputFile.close();
                    outputFile.close();
                    exit(1);
                }

                /*  
                * 
                */

                for (int k = 0; k < required_register_number; k++)
                {
                    //Insert registers
                    // Detect rd,Imm17(rs) instruction type
                    iss >> token;
                    size_t l_paranthesis = token.find('(');
                    if (l_paranthesis != string::npos) {
                        if (token.find(')') == string::npos) {
                            cerr << "Error: Missing paranthesis ')' ! Check line [" << number_line << "]" << endl;
                            inputFile.close();
                            outputFile.close();
                            exit(1);
                        }
                        temp_token = token;
                        token = token.substr(l_paranthesis + 1, token.find(')') - l_paranthesis - 1);
                    }


                    bool register_found = false;
                    while (l < 32) {
                        if ( ! token.compare(registers[l]) ) {
                            assembled_line.push_back(bitset<5>(l).to_string());
                            register_found = true;
                        }
                        l++;
                    }
                    l = 0;
                    if (!register_found) {
                        cerr << "Error: incorrect register name found ! Check line [" << number_line << "]" << endl;
                        inputFile.close();
                        outputFile.close();
                        exit(1);
                    }
                }

                size_t l_paranthesis = line.find('(');
                if (l_paranthesis != string::npos) {
                    token = temp_token;
                    token = token.substr(0, token.find('('));
                }
                else {
                    iss >> token;
                }

                switch (required_register_number)
                {
                //Insert Imm17-22
                    case 2:
                        if (token.find("0x") != string::npos) {
                            string hex_imm17_str;
                            hex_imm17_str = token;
                            int32_t hex_imm17_int = stoi(hex_imm17_str, nullptr, 16);
                            assembled_line.push_back(bitset<15>(hex_imm17_int).to_string());
                        }
                        else if (line.find("0b") != string::npos) {
                            string hex_imm17_str;
                            hex_imm17_str = token;
                            hex_imm17_str.erase(0, 2);
                            int32_t hex_imm17_int = stoi(hex_imm17_str, nullptr, 2);
                            assembled_line.push_back(bitset<15>(hex_imm17_int).to_string());
                        }
                        else {
                            string hex_imm17_str;
                            hex_imm17_str = token;
                            int32_t hex_imm17_int = stoi(hex_imm17_str);
                            assembled_line.push_back(bitset<15>(hex_imm17_int).to_string());
                        }
                        break;
                    case 1:
                        if (token.find("0x") != string::npos) {
                            string hex_imm22_str;
                            hex_imm22_str = token;
                            int32_t hex_imm22_int = stoi(hex_imm22_str, nullptr, 16);
                            assembled_line.push_back(bitset<20>(hex_imm22_int).to_string());
                        }
                        else if (line.find("0b") != string::npos) {
                            string hex_imm22_str;
                            hex_imm22_str = token;
                            hex_imm22_str.erase(0, 2);
                            int32_t hex_imm22_int = stoi(hex_imm22_str, nullptr, 2);
                            assembled_line.push_back(bitset<20>(hex_imm22_int).to_string());
                        }
                        else {
                            string hex_imm22_str;
                            hex_imm22_str = token;
                            int32_t hex_imm22_int = stoi(hex_imm22_str);
                            assembled_line.push_back(bitset<20>(hex_imm22_int).to_string());
                        }
                        break;
                default:
                    break;
                }
                
                //If the instruction on the line have function code, assemble it as well
                if (inst[i].funct != -1 )
                {
                    assembled_line.push_back(bitset<12>(inst[i].funct).to_string());
                }

                for (const auto& token : assembled_line) {
                    outputFile << token << ' ';
                }
                outputFile << endl;
            }
        }

        if (!instruction_detected)
        {
            cerr << "Error: No instruction found ! Check line [" << number_line << "]" << endl;
            inputFile.close();
            outputFile.close();
            exit(1);
        }
        number_line++;
    }
    inputFile.close();
    outputFile.close();
}

void bixtohex(const std::string& inputFilename, const std::string& outputFilename) {
    ifstream inputFile(inputFilename);
    ofstream outputFile(outputFilename);

    if (!inputFile.is_open()) {
        cerr << "Error: Unable to open input file: " << inputFilename << endl;
        return;
    }

    if (!outputFile.is_open()) {
        cerr << "Error: Unable to open output file: " << outputFilename << endl;
        return;
    }

    string line;
    string hexString;
    string eightBitChunk;
    stringstream hexStream;
    while (getline(inputFile, line)) {
       line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());

       if (line.length() % 8 != 0) {
           cerr << "Error: Binary string length must be a multiple of 8." << endl;
           exit(1);
       }
       
       for (size_t i = 0; i < line.length(); i += 8) {
           eightBitChunk = line.substr(i, 8);

           // Convert 8-bit chunk to an integer
           bitset<8> bits(eightBitChunk);
           unsigned long hexValue = bits.to_ulong();

           // Convert integer to hex string
           hexStream << hex << std::setw(2) << std::setfill('0') << hexValue;
           // Append the hex string to the result with a space
           hexString += hexStream.str() + " ";
           hexStream.str("");
           hexStream.clear();
       }
       hexString.pop_back();
       outputFile << hexString << endl;
       hexString = "";
       hexString.clear();
    }

    inputFile.close();
    outputFile.close();
}


void do_assembly(const string& inputFilename, const string& outputFilename) {

    string temp = "debug";

    make_lower_case(inputFilename, outputFilename);

    removeCommentsFromFile(outputFilename, temp);
    
    remove(outputFilename.c_str());
    rename(temp.c_str(), outputFilename.c_str());
    
    find_directives_data(outputFilename, temp);
    remove(outputFilename.c_str());
    rename(temp.c_str(), outputFilename.c_str());
    
    find_labels_text(outputFilename, temp);
    remove(outputFilename.c_str());
    rename(temp.c_str(), outputFilename.c_str());

    insert_labels_addresses(outputFilename, temp);
    remove(outputFilename.c_str());
    rename(temp.c_str(), outputFilename.c_str());
    
    assemble_line(outputFilename, temp);
    remove(outputFilename.c_str());
    rename(temp.c_str(), outputFilename.c_str());
    
    bixtohex(outputFilename, temp);
    remove(outputFilename.c_str());
    rename(temp.c_str(), outputFilename.c_str());
    
}